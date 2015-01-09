<?php

/*
 * @file
 * Influxis Class
 * Calls Influxis API
 * https://api.influxis.com/v1
 */

class influxis {

  private $suername;
  private $password;
  private $base_url;
  private $video_url;
  private $post;
  private $delete;
  private $process;
  static private $instance;

  public function __construct($user, $pass, $base, $video) {
    $this->credentials = $user . ":" . $pass;
    $this->base_url = $base;
    $this->video_url = $video;
    $this->post = TRUE;
    $this->delete = FALSE;
    $this->process = array(
      0 => 'error',
      1 => 'uploading',
      2 => 'encoding',
      3 => 'complete', 
      4 => 'ready to delete',
      5 => 'deleted'
    );
  }

  /*
   * Singleton
   * @param:  string $username, string $password, string $base url, string $video url
   */
  public static function getInstance($user, $pass, $base, $video) {
    if (!isset(self::$instance)) {
      self::$instance = new self($user, $pass, $base, $video);
    }
    return self::$instance;
  }

   /*
    * Gets process stage
    * @param: optional int $code of process
    * @return: string of code or array of all possible process states
   */
   public function getProcess($code = NULL) {
      if (isset($code)) {
         return $this->process[$code];
      } 
      else {
         return $this->process;
      }
   }

   /*
    * Returns video url
   */
   public function getVideoUrl() {
      return $this->video_url;
   }

   /*
    * set the type of api call (get or bool)
    * @param:  boolean $bool
    */
   public function setPost($bool) {
      $this->post = $bool;
   }

   /*	
    * Call influxis sibling api
    * @param: int $id of sibling
    * @return: Class {bool success, string url, bool deleted, string filename}
    */
   public function getSibling($id) {
      $this->setPost(FALSE);
      $this->setDelete(FALSE);
      $data = array(
         "SiblingEncodeId" => $id, 
      );
      $res = $this->call_api("/sibling?SiblingEncodeId=" . $id);
      $curl = json_decode($res->curl_exec);
      $response = new stdclass;
      $response->success = FALSE;
      $response->url = NULL;
      $response->filename = NULL;
      $response->deleted = FALSE;
      if ($res->success) {
         $response->success = TRUE;
         //keep it at this for now
         if (isset($curl->siblingGroup)) {
            $response->url = $curl->siblingGroup->scaleBitlyUrl;
            $response->deleted = $curl->siblingGroup->isRemoved;
            $response->filename = $curl->siblingGroup->encodeMediaGroup[0]->fileName;
         }
      }
      return $response;
   }

   /*
    * set if using delete api call 
    * @param:  boolean $bool
   */
   public function setDelete($bool) {
      $this->delete = $bool;
   }

   /*
    * Delete video
    * @param: int $id of sibling
    * @return:  Class {bool success, bool processing}
   */
   public function deleteSibling($id) {
      $this->setPost(true);
      $this->setDelete(true);
      $data = array(
         "SiblingEncodeId" => $id, 
      );

      $res = $this->call_api("/sibling", json_encode($data));
      $curl = json_decode($res->curl_exec);

      $response = new stdclass;
      $response->success = FALSE;
      $response->processing = NULL;
      if ($res->success) {
         $response->success = TRUE;
         //keep it at this for now
         $response->processing = $curl->stillProcessing;
      }
      return $response;
   }

   /*
    * Get video token for uploading (one of two calls to upload video to influxis)
    * @param: string $filename, bool $overwite
    * @return:  Class {bool success, string token, int source}
   */
   public function uploadFile($filename, $overwrite) {
      $data = array(
         "FileName" => $filename,
         "Overwrite" => $overwrite,
      );
      $result = $this->call_api("/upload/any", json_encode($data));

      $response = new stdClass;
      $response->success = FALSE;
      if ($result->success) {
         $response->success = TRUE;
         $curl = json_decode($result->curl_exec);
         $response->token = $curl->token;
         $response->source = $curl->sourceMediaId;
      }
      return $response;
   }

   /*
    * Upload video (second of two calls to upload video to influxis) - asynchronous
    * @param:  string $token, string $file path of file to be uplaoded
    * @return:  Class {bool success}
   */
   public function uploadTokenAsync($token, $file) {
      $url = $this->video_url . ':8080/uploadhandler/FileUploadHandler?mode=encode';
      $command = "curl  --header 'Influxis-Upload-Token: " . $token . "' --form 'File=@" . $file . "' " . $url . "";
      //set wether curl command should send output to txt file or not
      $command .= " > /dev/null &";

      //call exec to make the token curl async
      //this is done due to the timeout on browsers when waiting for this api call to complete
      //especially on browsers passing via proxy
      $exec = exec($command);

      $response = new stdClass;
      $response->success = TRUE;
      return $response;
   }

   /*
    * Upload video (second of two calls to upload video to influxis) - NON-asynchronous
    * @param:  string $token, string $file path of file to be uplaoded
    * @return:  Class {bool success}
   */
   public function uploadToken($token, $file) {
      $url = $this->video_url . ':8080/uploadhandler/FileUploadHandler?mode=encode';
      $ch = curl_init($url);
      $data = array('File' => "@$file");
      curl_setopt($ch, CURLOPT_POST, TRUE);
      curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
      curl_setopt($ch, CURLOPT_HTTPHEADER, array(
         'Influxis-Upload-Token: ' . $token,
      ));

      // Get the response and close the channel.
      $result->curl_exec = curl_exec($ch);
      $result->httpcode = curl_getinfo($ch, CURLINFO_HTTP_CODE);

      if ($result->httpcode >= 200 && $result->httpcode < 400) {
         $result->success = TRUE;
      }
      $result->error = curl_error($ch);
      curl_close($ch);

      $response = new stdClass;
      $response->success = FALSE;
      if ($result->success) {
         $response->success = TRUE;
      }
      return $response;
   }


   /*
    * Encode video
    * @param:  int $sourceMediaId, array $params of all encoding variables
    * @return: Class {bool success, bool uploaded, int sibling, int job, int media
   */
   public function encode($sourceMediaId, $params) {
      $this->setPost(TRUE);
      $result = $this->call_api("/encode/source/managed", json_encode($params));
      $curl = json_decode($result->curl_exec);
      $response = new stdClass;
      $response->success = FALSE;
      $response->uploaded = FALSE;
      $response->job = NULL;
      $response->media = NULL;
      $response->sibling = NULL;
      if ($result->success) {
         $response->success = TRUE;
        if (sizeof($curl->badSources) == 0) {
            $response->uploaded = TRUE;
            $queue = $curl->siblingGroupResults[0]->encodeResults[0]->queuedInfo;
            $response->sibling = $curl->siblingGroupResults[0]->siblingEncodeId;
            $response->job = $queue->encodeJobId;
            $response->media= $queue->encodeMediaId;
         }
      }
      return $response;
   }

   /*
    * Get status of an encode job
    * @param: int $job id of encoding job
    * @return: Class {bool success, bool isComplete}  
   */
   public function getStatus($job) {
      $this->setPost(FALSE);
      $result = $this->call_api("/encode/status?EncodeJobId=" . $job);
      $curl = json_decode($result->curl_exec);
      $response = new stdClass;
      $response->success = FALSE;
      $response->isComplete = FALSE;
      $response->curl = $curl;
      if ($result->success) {
         $response->success = TRUE;
         $response->isComplete = $curl->jobStatus->isComplete;
      }
      return $response;
   }

   /*
    * Call Influxis api
    * @param: string $service_name of api to call, option array $post_data of parameters for call
   */
   private function call_api($service_name, $post_data = NULL) {
      $response = new stdClass;
      $response->success = FALSE;
      $url = $this->base_url . $service_name;

      $contentLength = strlen($post_data);

      $ch = curl_init();
      curl_setopt($ch, CURLOPT_URL, $url);
      // Override the default headers
      curl_setopt($ch, CURLOPT_HTTPHEADER, array('Content-Type: application/json', 'Accept: application/json', "Expect: 100-continue"));
      // 0 do not include header in output, 1 include header in output
      curl_setopt($ch, CURLOPT_HEADER, 0);   
      // Set username and password
      if ($this->credentials != "") {
         curl_setopt($ch, CURLOPT_USERPWD, $this->credentials);
      }
      curl_setopt($ch, CURLOPT_TIMEOUT, 30); 
        
      // if you are not running with SSL or if you don't have valid SSL
      $verify_peer = FALSE;
      curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, $verify_peer);
        
      // Disable HOST (the site you are sending request to) SSL Verification,
      // if Host can have certificate which is invalid / expired / not signed by authorized CA.
      $verify_host = FALSE;
      curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, $verify_host);
        
      // Set the post variables (remove for get)
      if ($this->post) {
         curl_setopt($ch, CURLOPT_POST, 1);
         curl_setopt($ch, CURLOPT_POSTFIELDS, $post_data);
      }
      if ($this->delete) {
         curl_setopt($ch, CURLOPT_CUSTOMREQUEST, "DELETE");
      }
        
      // Set so curl_exec returns the result instead of outputting it.
      curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);

      // Get the response and close the channel.
      $response->curl_exec = curl_exec($ch);
      $response->httpcode = curl_getinfo($ch, CURLINFO_HTTP_CODE);

      if ($response->httpcode >= 200 && $response->httpcode < 400) {
         $response->success = TRUE;
      }
      $response->error = curl_error($ch);
      curl_close($ch);
        
      return $response;
}



   /*
    * Array of encoding select options available
    * @param:  string $name option, optional int $value of an option name
   */
   public function options($name, $value = NULL) {
      $opts = array(
         'influxis_container' => array(
            0 => 'MP4',
            1 => 'WebM',
            2 => 'Ogg'
         ),
         'influxis_video' => array(
            0 => 'H264',
            1 => 'VP8',
            2 => 'Theora'
         ),
         'influxis_audio' =>  array(
            0 => 'AAC',
            1 => 'MP3',
            2 => 'Vorbis'
         ),
         'influxis_container_mixer' => array(
            0 => 'FFmpeg_mp4',
            1 => 'FFmpeg_mkv',
            2 => 'FFmpeg_ogg',
            3 => 'mp4box',
            4 => 'mkvtoolnix'
         ),
         'influxis_video_encoder' => array(
            0 => 'FFmpeg_libx264',
            1 => 'FFmpeg_libvpx',
            2 => 'FFmpeg_libtheora',
            3 => 'ffmpeg2theora',
            4 => 'vpxenc',
         ),
         'influxis_audio_encoder' => array(
            0 => 'FFmpeg_aac',
            1 => 'FFmpeg_faac',
            2 => 'FFmpeg_libvorbis',
            3 => 'ffmpeg2theora',
         ),
         'influxis_h264_profile' => array(
            0 => 'Baseline',
            1 => 'Main',
            2 => 'High',
            3 => 'High10',
            4 => 'High422',
            5 => 'High444',
         ), 
         'influxis_level' => array(
            0 => 'L1',
            1 => 'L1b',
            2 => 'L11',
            3 => 'L12',
            4 => 'L13',
            5 => 'L2',
            6 => 'L21',
            7 => 'L22',
            8 => 'L3',
            9 => 'L31',
            10 => 'L32',
            11 => 'L4',
            12 => 'L41',
            13 => 'L42',
            14 => 'L5',
            15 => 'L51',
            16 => 'L52',
         ),
         'influxis_preset' => array(
            0 => 'UltraFast',
            1 => 'SuperFast',
            2 => 'VeryFast',
            3 => 'Faster',
            4 => 'Fast',
            5 => 'Medium',
            6 => 'Slow',
            7 => 'Slower',
            8 => 'VerySlow',
            9 => 'Placebo',
         ),
         'influxis_tune' => array(
            0 => 'Film', 
            1 => 'Animation',
            2 => 'Grain',
            3 => 'StillImage',
            4 => 'PSNR',
            5 => 'SSIM',
            6 => 'FastDecode',
            7 => 'ZeroLatency', 
         ),
         'influxis_b_adapt' => array(
            0 => 'Disabled',
            1 => 'Fast',
            2 => 'Optimal',
         ),
         'influxis_b_pyramid' => array(
            0 => 'None',
            1 => 'Strict',
            2 => 'Normal',
         ),
         'influxis_adaptive_quatization' => array(
            0 => 'Disable',
            1 => 'Enabled',
            2 => 'Auto',
         ),
         'influxis_direct' => array(
            0 => 'None',
            1 => 'Spatial',
            2 => 'Temporal',
            3 => 'Auto',
         ),
         'influxis_weight_p' => array(
            0 => 'Disabled',
            1 => 'Simple',
            2 => 'Smart',
         ),
         'influxis_motion_estimation' => array(
            0 => 'Diamond',
            1 => 'Hexagon',
            2 => 'UnevenMultiHex',
            3 => 'Exhaustive',
            4 => 'TransformedExhaustive',
         ),
         'influxis_sub_motion_estimation' => array(
            0 => 'FullPel',
            1 => 'QPelSAD1',
            2 => 'QPelSATD2',
            3 => 'HPelonMBthenQPel',
            4 => 'QPelAlways',
            5 => 'MultiQpelAndBiDiME',
            6 => 'RDonIP',
            7 => 'RDonAll',
            8 => 'RDrefineIP',
            9 => 'RDrefineAll',
            10 => 'QPRD',
            11 => 'FullRD',
         ),
         'influxis_trellis' => array(
            0 => 'Disabled',
            1 => 'EnabledFinal',
            2 => 'EnabledAll',
         ),
         'influxis_deadline' => array(
            0 => 'Best',
            1 => 'Good', 
            2 => 'Realtime', 
         ),
         'influxis_error_resilient' => array(
            0 => 'Default',
            1 => 'Partitions'
         ),
      );

      if ($value == NULL) {
         return (isset($opts[$name]) ? $opts[$name] : NULL);
      }
      else {
         return (isset($opts[$name]) && isset($opts[$name][$value]) ? $opts[$name][$value] : NULL);
      }
   }

}
