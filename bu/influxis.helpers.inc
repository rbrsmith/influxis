<?php

/*
 * Get influxis instance
*/
function _influxis_() {
	$username =  variable_get('influxis_username');
	$password_enc = variable_get('influxis_password');
	$base = variable_get('influxis_baseurl');
	$video = variable_get('influxis_videourl');
	if($username == null || $password_enc == null || $base == null || $video == null) {
		_influxis_error("Cannot load influxis, please ensure account settings are configured correctly");
		return null;
	} else {
		$encrypt = new Encrypt();
		$raw = $encrypt->reveal($password_enc);
		return influxis::getInstance($username, $raw, $base, $video);
	}
}

/*
 * Send video to influxis for updating
*/
function _influxis_upload($fid, $nid) {
	$file = file_load($fid);

	$file_url = drupal_realpath($file->uri);

	$fms = _influxis_();
	if($fms == null) return;
	$fileUpload = $fms->uploadFile($file->filename, true);
	if(!$fileUpload->success || $fileUpload->token == null || $fileUpload->source == null) {
		_influxis_error("Unable to upload video to influxis.  Error in token creation.  Filename: " . $file->filename);
		return;
	}

	$tokenUpload = $fms->uploadTokenAsync($fileUpload->token, $file_url);
	if(!$tokenUpload->success) {
		_influxis_error("Unable to upload video to influxis.  Error in file upload.  Filename: " . $file->filename);
		return;
	}
	//video has been sent to influxis for uploading
	drupal_set_message(t("Video sent to influxis for uploading"));

	$db_table = variable_get('influxis_db', 'influxis');
	$query = db_insert($db_table)
	//		->key(array('nid' => $nid))
	->fields(array(
			'fid' => $file->fid,
			'nid' => $nid,
			'status' => 1,
			'source' => $fileUpload->source,
	));

	try {
		$query->execute();
	} catch(PDOException $e) {
		_influxis_error("Error with inserting into DB " . $e->getMessage());
	}
}


/*
 * Get all records in influxis table
* @param condition:  singular array, or multi array, of conditions - ex array('nid', 5, '=')
* @return: array of all records matching condition
*/
function _influxis_get_records($condition = null) {
	$db_table = variable_get('influxis_db', 'influxis');

	$query = db_select($db_table, 'i')
	->fields('i');

	if(isset($condition)){
		//multiple conditions
		if(is_array($condition[0])) {
			foreach($condition as $c) {
				$query->condition($c[0], $c[1], $c[2]);
			}
			//singular condition
		} else {
			$query->condition($condition[0], $condition[1], $condition[2]);
		}
	}

	try {
		$res = $query->execute();
		return (($results = $res->fetchAll()) == 0 ? null : $results);
	} catch(PDOException $e) {
		_influxis_error("Error getting status from DB " . $e->getMessage());
	}
}


/*
 * Update the status field of an influxis video
* @param: int status, int fid of file, optiona array fields of all other fields to be updated
*/
function _influxis_update_status($status, $condition_fid, $fields = array()){
	watchdog("influxis", "Updating fid, " . $condition_fid .", to status " . $status);
	$fields['status'] = $status;

	$db_table = variable_get('influxis_db', 'influxis');
	$query = db_update($db_table)
	->fields($fields)
	->condition('fid', $condition_fid);
	try{
		$query->execute();
	} catch(PDOException $e){
		echo "Error";
		var_dump($e);
		drupal_set_message(t("Could not update database"),"error");
	}
}


/*
 * update all video status
*/
function _influxis_update_videos(){
	$results = _influxis_get_records();
	foreach($results as $record){
		_influxis_update_video($record);

	}
	return $results;
}

/*
 * Update a single records status - triggering any uploading or encoding to be done
* @param: database class record from influxis table
*/
function _influxis_update_video(&$record){
	$fms = _influxis_();
	if($fms == null) return;
	//video is being uploaded
	if($record->status == 1) {
		//send video to be encoded
		$params = _influxis_get_encoding($record->source);
		$res = $fms->encode($record->source, $params);
		if(!$res->success) {
			_influxis_error("Could not encode video for " . $record->fid);
			_influxis_update_status(0, $record->fid);
			$record->status == 0;
		} else if($res->uploaded) {
			_influxis_update_status(2, $record->fid, array(
			'job' => $res->job,
			'media' => $res->media,
			'sibling' => $res->sibling,
			));
			$record->status = 2;
		}
		//if video in in encoding state
	} else if($record->status == 2){
		//check videos status
		$res = $fms->getStatus($record->job);
		if(!$res->success) {
			_influxis_error("Could not get status of video for " . $record->fid);
		} else if($res->isComplete){
			_influxis_update_status(3, $record->fid);
			$record->status = 3;
		}
		//video is marked for deletion
	} else if($record->status == 4) {
		if($record->sibling == null){
			_influxis_error("Could not remove video from influxis - No sibling for " . $record->fid);
		} else {
			$res = _influxis_delete_video($record->fid, $record->sibling);
			if($res) {
				_influxis_update_status(5, $record->fid);
				$record->status = 5;
			}
		}
	}

	//TODO put condition so this is not always called (ie deleted videos)
	//regardless of state if we have a sibling id but no url we'll try to get url
	if(isset($record->sibling) && $record->status != 5) {
		$res = $fms->getSibling($record->sibling);
		if($res->success){
			if($res->deleted) {
				_influxis_update_status(5, $record->fid);
				$record->status = 5;
			} else {
				_influxis_update_url($res->url, "rtmp", $record->fid);
				_influxis_update_url($fms->getVideoUrl() . "/ManagedContent/".$res->filename, "http", $record->fid);
			}
		} else {
			_influxis_error("Could not get delete status of video for " . $record->fid);
		}
	}
}

/*
 * Update url for a record
* @param string url to change, string type of url (rtmp, http), int fid of file
*/
function _influxis_update_url($url, $type, $fid){
	$column = $type . "Url";
	$fields = array($column => $url);

	$db_table = variable_get('influxis_db', 'influxis');

	$query = db_update($db_table)
	->fields($fields)
	->condition('fid', $fid);
	try{
		$query->execute();
	} catch(PDOException $e){
		_influxis_error("Could not update database");
	}

}

/*
 * mark a file for deletiong
* @param: int fid of file
*/
function _influxis_prepare_to_delete($fid){
	_influxis_update_status(4, $fid);
}

/*
 * Delete a video from influxis
* @param: int fid of file, int sibling id of video
*/
function _influxis_delete_video($fid, $sibling){
	$fms = _influxis_();
	if($fms == null) return;
	$delete_sib_res = $fms->deleteSibling($sibling);
	if($delete_sib_res->success) {
		if($delete_sib_res->processing) {
			_influxis_error("Could not remove video from influxis - still processing. Fid = " . $fid);
			return false;
		} else {
			drupal_set_message(t("Influxis video deleted"));
			return true;
		}
	} else {
		_influxis_update_status(0, $fid);
		_influxis_error("Could not remove video from influxis - Error in API. Fid = " . $fid);
		return false;
	}
}


/*
 *	Wrapper for setting message in drupal
*	@param:  string msg to display
*/
function _influxis_error($msg){
	drupal_set_message(t($msg), "error");
	watchdog("influxis", t($msg));
}


/*
 * get encoding variables
* @param: int source id of video
*/
function _influxis_get_encoding($source) {

	$fms = _influxis_();
	if($fms == null) return;
	$VPXSettings = array(
			"ConstantRateFactor" => variable_get('influxis_constant_rate_factor'),
			"QualityMin" => variable_get('influxis_quality_min'),
			"QualityMax" => variable_get('influxis_quality_max'),
			"CpuUsed" => variable_get('influxis_cpu_used'),
			"RCLookAhead" => variable_get('influxis_rc_look_ahead'),
			"Deadline" => $fms->options('influxis_deadline', variable_get('influxis_deadline')),
			"ErrorResilient" => $fms->options('influxis_error_resilient', variable_get('influxis_error_resilient')),
			"MaxIntraRate" => variable_get('influxis_max_intra_rate'),
			"Profile" => variable_get('influxis_vpx_profile'),
			"Sharpness" => variable_get('influxis_sharpness'),
			"NoiseReduction" => variable_get('influxis_noise_reduction'),
			"BufSize" => variable_get('influxis_buf_size'),
	);
	$VP8Settings = array(
			"MinKeyFrameInterval" => variable_get('influxis_min_key_frame_interval'),
			"MaxKeyFrameInterval" => variable_get('influxis_max_key_frame_interval'),
			"VPXSettings" => $VPXSettings,
	);
	$X264Settings = array(
			"Preset" => $fms->options('influxis_preset', variable_get('influxis_preset')),
			"Tune" => $fms->options('influxis_tune', variable_get('influxis_tune')),
			"IntraRefresh" => variable_get('influxis_inta_refresh'),
			"BAdapt" => $fms->options('influxis_b_adapt', variable_get('influxis_b_adapt')),
			"BBias" => variable_get('influxis_b_bias'),
			"BPyramid" => $fms->options('influxis_b_pyramid', variable_get('influxis_b_pyramid')),
			"ConstantRateFactor" => variable_get('influxis_constant_rate_factor'),
			"ConstantRateFactorMax" => variable_get('influxis_constant_rate_factor_max'),
			"VBVMaxRate" => variable_get('influxis_vbv_max_rate'),
			"VBVBufSize" => variable_get('influxis_vbv_buf_size'),
			"VBVInit" => variable_get('influxis_vbv_init'),
			"AdaptiveQuantization" => $fms->options('influxis_adaptive_quantization', variable_get('influxis_adaptive_quantization')),
			"AdaptiveQuantizationStrength" => variable_get('influxis_adaptive_quantization_strength'),
			"Partitions" => variable_get('influxis_partitions'),
			"Direct" => $fms->options('influxis_direct', variable_get('influxis_direct')),
			"NoWeightB" => variable_get('influxis_no_weight_b'),
			"WeightP" => $fms->options('influxis_weight_p', variable_get('influxis_weight_p')),
			"MotionEstimation" => $fms->options('influxis_motion_estimation', variable_get('influxis_motion_estimation')),
			"MotionEstimationRange" => variable_get('influxis_motion_estimation_range'),
			"SubMotionEstimation" =>$fms->options('influxis_sub_motion_estimation', variable_get('influxis_sub_motion_estimation')),
			"PsyRDOStrength" => variable_get('influxis_psy_rdo_strength'),
			"NoPsyRDO" => variable_get('influxis_no_psy_rdo'),
			"Trellis" => $fms->options('influxis_trellis', variable_get('influxis_trellis')),
			"NoFastPSkip" => variable_get('influxis_no_fast_p_skip'),
			"NoDCTDecimate" => variable_get('influxis_no_dct_decimate'),
			"NoiseReduction" => variable_get('influxis_noise_reduction'),
	);

	$H264Settings = array(
			"Profile" => $fms->options('influxis_h264_profile', variable_get('influxis_h264_profile')),
			"Level" => $fms->options('influxis_level', variable_get('influxis_level')),
			"MinKeyFrameInterval" => variable_get('influxis_min_key_frame_interval'),
			"MaxKeyFrameInterval" => variable_get('influxis_max_key_frame_interval'),
			"MaxBFrames" => variable_get('influxis_max_b_frames'),
			"DecodedPictureBuffer" => variable_get('influxis_decoded_picture_buffer'),
			"NoDeblock" => variable_get('influxis_no_deblock'),
			"Deblock" => variable_get('influxis_deblock'),
			"OpenGop" => variable_get('influxis_open_hop'),
			"X264Settings" => $X264Settings,
	);
	$Profile1 = array(
			"StateData" => variable_get('influxis_state_data'),
			"Container" => $fms->options('influxis_container', variable_get('influxis_container')),
			"Video" => $fms->options('influxis_video', variable_get('influxis_video')),
			"Audio" => $fms->options('influxis_audio', variable_get('influxis_audio')),
			"VideoBitrate" => variable_get('influxis_video_bitrate'),
			"AudioBitrate" => variable_get('influxis_audio_bitrate'),
			"MinVideoBitrate" => variable_get('influxis_min_video_bitrate'),
			"MaxVideoBitrate" => variable_get('influxis_max_video_bitrate'),
			"Width" => variable_get('influxis_width'),
			"Height" => variable_get('influxis_height'),
			"FrameRate" => variable_get('influxis_frame_rate'),
			"FrameRateNumerator" => variable_get('influxis_frame_rate_numerator'),
			"FrameRateDenominator" => variable_get('influxis_frame_rate_denominator'),
			"AudioChannels" => variable_get('influxis_audio_channels'),
			"AudioSampleRate" => variable_get('influxis_audio_sample_rate'),
			"ContainerMuxer" => $fms->options('influxis_continuer_mixer', variable_get('influxis_continuer_mixer')),
			"VideoEncoder" => $fms->options('influxis_video_encoder', variable_get('influxis_video_encoder')),
			"AudioEncoder" => $fms->options('influxis_audio_encoder', variable_get('influxis_audio_encoder')),

	);
	
	$Profile1['H264Settings'] = (variable_get('influxis_use_h264_settings') == 0 ? null : $H264Settings);
	$Profile1['VP8Settings'] = (variable_get('influxis_use_vp8_settings') == 0 ? null : $VP8Settings);

	$Encode1 = array(
			"SourceMediaId" => $source,
			"Profiles" => array(0=>$Profile1)
	);

	$encodingParam = array(
			"Encodes" => array(0=>$Encode1),
			//			"StatusCallbackUrl" => ""
	);
	return $encodingParam;
}



?>