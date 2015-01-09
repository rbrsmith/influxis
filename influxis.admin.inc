<?php
/*
 * @file 
 * File used for administrative settings
 * 
 */



/*
 * General page for admin settings
 */
function influxis_settings_page($op = NULL) {
  $output = drupal_get_form('influxis_account_settings');
  return $output;
}

/*
 * Returns account settings admin form
 */
function influxis_account_settings() {
  $form = array();
  $form['influxis_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Influxis Username'),
    '#size' => 15,
    '#maxlength' => 64,
    '#default_value' => variable_get('influxis_username', ''),
    '#required' => TRUE,
  );
  $form['influxis_password'] = array(
    '#type' => 'password',
    '#title' => t('Influxis Password'),
    '#size' => 15,
    '#maxlength' => 64,
    '#required' => FALSE,
  );

  $form['influxis_baseurl'] = array(
    '#type' => 'textfield',
    '#title' => t('Influxis API URL'),
    '#size' => 60,
    '#maxlength' => 128,
    '#default_value' => variable_get('influxis_baseurl', ''),
    '#required' => TRUE,
  );

  $form['influxis_videourl'] = array(
    '#type' => 'textfield',
    '#title' => t('Influxis Video URL'),
    '#size' => 60,
    '#maxlength' => 128,
    '#default_value' => variable_get('influxis_videourl', ''),
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Save',
  );

  return $form;
}

/*
 * Returns encoding admin settings
 */
function influxis_encoding_settings() {
  $fms = _influxis_();
  if ($fms == NULL) return;
  $form = array();
  $form['encoding'] = array(
    '#type' => 'vertical_tabs', 
  );

  $form['basic'] = array(
    '#type' => 'fieldset',
    '#title' => t('Basic encoding settings'),
    '#collapsible' => TRUE,
    '#description' => t('Enter basic encoding settings here'),
    '#group' => 'encoding'
  );

  $form['H264'] = array(
    '#type' => 'fieldset',
    '#title' => t('H264 encoding settings'),
    '#collapsible' => TRUE,
    '#description' => t('Enter H264 encoding settings here'),
    '#group' => 'encoding'
  );

  $form['VPX'] = array(
    '#type' => 'fieldset',
    '#title' => t('VPX encoding settings'),
    '#collapsible' => TRUE,
    '#description' => t('Enter VPX encoding settings here'),
    '#group' => 'encoding'
  );


  ////////////////////BASE ENCODING////////////////////////

  $form['basic']['influxis_state_data'] = array(
    '#type' => 'textfield',
    '#title' => t('State Data'),
    '#size' => 5,
    '#maxlength' => 5,
    '#required' => FALSE,
    '#default_value' => variable_get('influxis_state_data'),
    '#group' => 'encoding'
  );

  $form['basic']['influxis_container'] = array(
    '#type' => 'select',
    '#title' => t('Container'),
    '#options' => $fms->options('influxis_container'),
    '#default_value' => variable_get('influxis_container'),
    '#required' => TRUE,
    '#group' => 'encoding'
  );

  $form['basic']['influxis_video'] = array(
    '#type' => 'select',
    '#title' => t('Video'),
    '#options' => $fms->options('influxis_video'),
    '#default_value' => variable_get('influxis_video'),
    '#required' => TRUE,
    '#group' => 'encoding'
  );

  $form['basic']['influxis_audio'] = array(
    '#type' => 'select',
    '#title' => t('Audio'),
    '#options' => $fms->options('influxis_audio'),
    '#default_value' => variable_get('influxis_audio'),
    '#required' => TRUE,
    '#group' => 'encoding'
  );

  $form['basic']['influxis_video_bitrate'] = array(
    '#type' => 'textfield',
    '#title' => t('Video Bitrate'),
    '#size' => 60,
    '#maxlength' => 128,
    '#default_value' => variable_get('influxis_video_bitrate'),
    '#required' => TRUE,
    '#group' => 'encoding'
  );

  $form['basic']['influxis_audio_bitrate'] = array(
    '#type' => 'textfield',
    '#title' => t('Audio Bitrate'),
    '#size' => 60,
    '#maxlength' => 128,
    '#default_value' => variable_get('influxis_audio_bitrate'),
    '#required' => TRUE,
    '#group' => 'encoding'
  );

  $form['basic']['influxis_min_video_bitrate'] = array(
    '#type' => 'textfield',
    '#title' => t('Min Video Bitrate'),
    '#size' => 60,
    '#maxlength' => 128,
    '#default_value' => variable_get('influxis_min_video_bitrate'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['basic']['influxis_min_audio_bitrate'] = array(
    '#type' => 'textfield',
    '#title' => t('Min Audio Bitrate'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_min_audio_bitrate'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['basic']['influxis_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_width'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['basic']['influxis_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Height'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_height'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['basic']['influxis_frame_rate'] = array(
    '#type' => 'textfield',
    '#title' => t('Frame Rate'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_frame_rate'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['basic']['influxis_frame_rate_numerator'] = array(
    '#type' => 'textfield',
    '#title' => t('Frame Rate Numerator'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_frame_rate_numerator'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['basic']['influxis_frame_rate_denominator'] = array(
    '#type' => 'textfield',
    '#title' => t('Frame Rate Denominator'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_frame_rate_denominator'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['basic']['influxis_audio_channels'] = array(
    '#type' => 'textfield',
    '#title' => t('Audio Channels'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_audio_channels'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['basic']['influxis_audio_sample_rate'] = array(
    '#type' => 'textfield',
    '#title' => t('Audio Sample Rate'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_audio_sample_rate'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['basic']['influxis_container_mixer'] = array(
    '#type' => 'select',
    '#title' => t('Container Mixer'),
    '#options' => $fms->options('influxis_container_mixer'),
    '#default_value' => variable_get('influxis_container_mixer'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['basic']['influxis_video_encoder'] = array(
    '#type' => 'select',
    '#title' => t('Video Encoder'),
    '#options' => $fms->options('influxis_video_encoder'),
    '#default_value' => variable_get('influxis_video_encoder'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['basic']['influxis_audio_encoder'] = array(
    '#type' => 'select',
    '#title' => t('Audio Encoder'),
    '#options' => $fms->options('influxis_audio_encoder'),
    '#default_value' => variable_get('influxis_audio_encoder'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );


  ////////////////////H264 ENCODING////////////////////////

  $form['H264']['influxis_use_h264_settings'] = array(
    '#type' => 'checkbox',
    '#title' => t('Click here to enable the additional H264 settings.'),
    '#default_value' => variable_get('influxis_use_h264_settings'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_h264_profile'] = array(
    '#type' => 'select',
    '#title' => t('Profile'),
    '#options' => $fms->options('influxis_h264_profile'),
    '#default_value' => variable_get('influxis_h264_profile'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_level'] = array(
    '#type' => 'select',
    '#title' => t('Level'),
    '#options' => $fms->options('influxis_level'),
    '#default_value' => variable_get('influxis_level'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_min_key_frame_interval'] = array(
    '#type' => 'textfield',
    '#title' => t('Min Key Frame Interval'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_min_key_frame_interval'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_max_key_frame_interval'] = array(
    '#type' => 'textfield',
    '#title' => t('Max Key Frame Interval'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_max_key_frame_interval'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_max_b_frames'] = array(
    '#type' => 'textfield',
    '#title' => t('Max B Frames'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_max_b_frames'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_decoded_picture_buffer'] = array(
    '#type' => 'textfield',
    '#title' => t('Decoded Picture Buffer'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_decoded_picture_buffer'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_no_deblock'] = array(
    '#type' => 'checkbox',
    '#title' => t('No Deblock'),
    '#default_value' => variable_get('influxis_no_deblock'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_deblock'] = array(
    '#type' => 'textfield',
    '#title' => t('Deblock'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_deblock'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_open_gop'] = array(
    '#type' => 'checkbox',
    '#title' => t('Open Gop'),
    '#default_value' => variable_get('influxis_open_gop'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_preset'] = array(
    '#type' => 'select',
    '#title' => t('Preset'),
    '#options' => $fms->options('influxis_preset'),
    '#default_value' => variable_get('influxis_preset'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_tune'] = array(
    '#type' => 'select',
    '#title' => t('Tune'),
    '#options' => $fms->options('influxis_tune'),
    '#default_value' => variable_get('influxis_tune'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_intra_refresh'] = array(
    '#type' => 'checkbox',
    '#title' => t('Intra Refresh'),
    '#default_value' => variable_get('influxis_intra_refresh'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_b_adapt'] = array(
    '#type' => 'select',
    '#title' => t('BAdapt'),
    '#options' => $fms->options('influxis_b_adapt'),
    '#default_value' => variable_get('influxis_b_adapt'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_b_bias'] = array(
    '#type' => 'textfield',
    '#title' => t('Decoded Picture BBias'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_b_bias'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_b_pyramid'] = array(
    '#type' => 'select',
    '#title' => t('BPyramid'),
    '#options' => $fms->options('influxis_b_pyramid'),
    '#default_value' => variable_get('influxis_b_pyramid'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_constant_rate_factor'] = array(
    '#type' => 'textfield',
    '#title' => t('Constant Rate Ractor'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_constant_rate_factor'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_constant_rate_factor_max'] = array(
    '#type' => 'textfield',
    '#title' => t('Constant Rate Ractor Max'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_constant_rate_factor_max'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_vbv_max_rate'] = array(
    '#type' => 'textfield',
    '#title' => t('VBV Max Rate'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_vbv_max_rate'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_vbv_buf_size'] = array(
    '#type' => 'textfield',
    '#title' => t('VBV Buf Size'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_vbv_buf_size'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_vbv_init'] = array(
    '#type' => 'textfield',
    '#title' => t('VBV Init'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_vbv_init'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_adaptive_quatization'] = array(
    '#type' => 'select',
    '#title' => t('Adaptive Quatization'),
    '#options' => $fms->options('influxis_adaptive_quatization'),
    '#default_value' => variable_get('influxis_adaptive_quatization'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_adaptive_quatization_strength'] = array(
    '#type' => 'textfield',
    '#title' => t('Adaptive Quatization Stength'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_adaptive_quatization_strength'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_partitions'] = array(
    '#type' => 'textfield',
    '#title' => t('Paritions seperated by comma (ex: i4x4", "i8x8", "p8x8", "b8x8")'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_partitions'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_direct'] = array(
    '#type' => 'select',
    '#title' => t('Direct'),
    '#options' => $fms->options('influxis_direct'),
    '#default_value' => variable_get('influxis_direct'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_no_weight_b'] = array(
    '#type' => 'checkbox',
    '#title' => t('No Wight B'),
    '#default_value' => variable_get('influxis_no_weight_b'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_weight_p'] = array(
    '#type' => 'select',
    '#title' => t('Weight P'),
    '#options' => $fms->options('influxis_weight_p'),
    '#default_value' => variable_get('influxis_weight_p'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_motion_estimation'] = array(
    '#type' => 'select',
    '#title' => t('Motion Estimation'),
    '#options' => $fms->options('influxis_motion_estimation'),
    '#default_value' => variable_get('influxis_motion_estimation'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_motion_estimation_range'] = array(
    '#type' => 'textfield',
    '#title' => t('Motion Estimaton Range'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_motion_estimation_range'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_sub_motion_estimation'] = array(
    '#type' => 'select',
    '#title' => t('Sub Motion Estimation'),
    '#options' => $fms->options('influxis_sub_motion_estimation'),
    '#default_value' => variable_get('influxis_sub_motion_estimation'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_psy_rdo_strength'] = array(
    '#type' => 'textfield',
    '#title' => t('Psy RDO Strength'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_psy_rdo_strength'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_no_psy_rdo'] = array(
    '#type' => 'checkbox',
    '#title' => t('No Psy RDO'),
    '#default_value' => variable_get('influxis_no_psy_rdo'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_trellis'] = array(
    '#type' => 'select',
    '#title' => t('Trellis'),
    '#options' => $fms->options('influxis_trellis'),
    '#default_value' => variable_get('influxis_trellis'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_no_fast_p_skip'] = array(
    '#type' => 'checkbox',
    '#title' => t('No Fast P Skip'),
    '#default_value' => variable_get('influxis_no_fast_p_skip'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_no_dct_decimate'] = array(
    '#type' => 'checkbox',
    '#title' => t('No DCT Decimate'),
    '#default_value' => variable_get('influxis_no_dct_decimate'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['H264']['influxis_noise_reduction'] = array(
    '#type' => 'textfield',
    '#title' => t('Noise Reduction'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_noise_reduction'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );


  ////////////////////VPX ENCODING////////////////////////


  $form['VPX']['influxis_use_vp8_settings'] = array(
    '#type' => 'checkbox',
    '#title' => t('Click here to enable the additional VP8 settings.'),
    '#default_value' => variable_get('influxis_use_vp8_settings'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['VPX']['influxis_min_key_frame_interval'] = array(
    '#type' => 'textfield',
    '#title' => t('Min Key Frame Interval'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_min_key_frame_interval'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['VPX']['influxis_max_key_frame_interval'] = array(
    '#type' => 'textfield',
    '#title' => t('Max Key Frame Interval'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_max_key_frame_interval'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['VPX']['influxis_constant_rate_factor'] = array(
    '#type' => 'textfield',
    '#title' => t('Constant Rate Factor'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_constant_rate_factor'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['VPX']['influxis_quality_min'] = array(
    '#type' => 'textfield',
    '#title' => t('Quality Min'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_quality_min'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['VPX']['influxis_quality_max'] = array(
    '#type' => 'textfield',
    '#title' => t('Quality Max'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_quality_max'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['VPX']['influxis_cpu_used'] = array(
    '#type' => 'textfield',
    '#title' => t('CPU Used'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_cpu_used'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['VPX']['influxis_rc_look_ahead'] = array(
    '#type' => 'textfield',
    '#title' => t('RC Look Ahead'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_rc_look_ahead'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['VPX']['influxis_deadline'] = array(
    '#type' => 'select',
    '#title' => t('Deadline'),
    '#options' => $fms->options('influxis_deadline'),
    '#default_value' => variable_get('influxis_deadline'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['VPX']['influxis_error_resilient'] = array(
    '#type' => 'select',
    '#title' => t('Error Resilient'),
    '#options' => $fms->options('influxis_error_resilient'),
    '#default_value' => variable_get('influxis_error_resilient'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['VPX']['influxis_max_intra_rate'] = array(
    '#type' => 'textfield',
    '#title' => t('Max Intra Rate'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_max_intra_rate'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['VPX']['influxis_vpx_profile'] = array(
    '#type' => 'textfield',
    '#title' => t('Profile'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_vpx_profile'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['VPX']['influxis_sharpness'] = array(
    '#type' => 'textfield',
    '#title' => t('Sharpness'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_sharpness'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['VPX']['influxis_noise_reduction'] = array(
    '#type' => 'textfield',
    '#title' => t('Noise Reduction'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_noise_reduction'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  $form['VPX']['influxis_buf_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Buf Size'),
    '#size' => 10,
    '#maxlength' => 5,
    '#default_value' => variable_get('influxis_buf_size'),
    '#required' => FALSE,
    '#group' => 'encoding'
  );

  return system_settings_form($form);
}


/*
 * validates settings
 */
function influxis_account_settings_validate(&$form, &$form_state) {
  $base_url = $form_state['values']['influxis_baseurl'];
  $video_url = $form_state['values']['influxis_videourl'];
  if (!valid_url($base_url, TRUE)) {
    form_error($form['influxis_baseurl'], t("Please enter correct URL"));
  }
  if (!valid_url($video_url, TRUE)) {
    form_error($form['influxis_videourl'], t("Please enter correct URL"));
  }
}

/*
 * Submits form
 * Password is encrypted before saving to DB
 */
function influxis_account_settings_submit(&$form, &$form_state) {
  variable_set('influxis_username', $form_state['values']['influxis_username']);
  variable_set('influxis_baseurl', $form_state['values']['influxis_baseurl']);
  variable_set('influxis_videourl', $form_state['values']['influxis_videourl']);

  if ($form_state['values']['influxis_password'] != '') {
    $encrypt = new Encrypt();
    $protected = $encrypt->protect($form_state['values']['influxis_password']);
    variable_set('influxis_password', $protected);
    //$raw = $encrypt->reveal($protected);
  }
  drupal_set_message(t("Account Settings Saved"));
}


/*
 * Form that displays status of influxis videos and triggers video status change
 */
function influxis_status_form($form = array(), $form_state) {
  $rows = array('#tree' => TRUE);
  $fms = _influxis_();
  $all_complete = TRUE;
  //get all videos
  $results = _influxis_update_videos();
  $hide_complete = variable_get('influxis_hide_completed');

  foreach ($results as $record) {
    $fid = $record->fid;
    $status = $record->status;
    $nid = $record->nid;
    $file = file_load($fid);

    if ($file == NULL) {
      $file_name = "File has been deleted or replaced";
    } 
    else {
      $file_name = $file->filename;
    }

    $node=node_load($nid);

    //define node display in form
    if ($node == NULL) {
      $node_type = "";
      $node_link = array(
        '#markup' => "Node Deleted",
      );
    } 
    else {
      $node_type = $node->type;
      $node_link = array('data' => array(
        '#type' => 'link', 
        '#title' => filter_xss($node->title), 
        '#href' => 'node/' . $nid, 
      ));
    }

    if ($status != 3) $all_complete = FALSE;
    if ($status == 3 && $hide_complete) continue;
    if ($status == 5 && $hide_complete) continue;

    $rows[] = array(
      'c1' => array(
        '#markup' => $node_type,
      ),
      'c2' => array(
        '#markup' => $nid
      ),
      'c3' => $node_link,
      'c4' => array(
        '#markup' => $fid,
      ),
      'c5' => array(
        '#markup' => $file_name
      ),
      'c6' => array(
        '#markup' => $fms->getProcess($status)
      ),
    );
  }

  if ($all_complete) {
    $form['influxis_message'] = array(
      '#markup' => t("There are no videos currently being processed"), 
    );
  }

  if (!($hide_complete && $all_complete)) {
    $form['influxis_table'] = array(
      '#theme' => 'influxis_form_table',
      '#header' => array(t('Node Type'), t('Nid'), t('Node Title'), t('Fid'), t('File'), t('Status')),
      'rows' => $rows,
    );
  }

  $form['basic']['influxis_hide_completed'] = array(
    '#type' => 'checkbox',
    '#title' => t('Click here to hide completed videos'),
    '#default_value' => variable_get('influxis_hide_completed', 1),
    '#required' => FALSE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update')
  );
  return $form;
}

/*
 * Refresh video list when submited
 */
function influxis_status_form_submit($form, $form_state) {
  variable_set('influxis_hide_completed', $form_state['values']['influxis_hide_completed']);
  _influxis_update_videos();
}

/*
 * hook_theme()
*/
function influxis_theme() {
  return array(
    'influxis_form_table' => array(
    'render element' => 'form'
    )
  );
}

/*
 * Called by influxis_theme to render admin form
*/
function theme_influxis_form_table(&$variables) {
  // Get the useful values.
  $form = $variables['form'];
  $rows = $form['rows'];
  $header = $form['#header'];

  // Setup the structure to be rendered and returned.
  $content = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => array(),
  );

  // Traverse each row.  @see element_chidren().
  foreach (element_children($rows) as $row_index) {
    $row = array();
    // Traverse each column in the row.  @see element_children().
    foreach (element_children($rows[$row_index]) as $col_index) {
      // Render the column form element.
      $row[] = drupal_render($rows[$row_index][$col_index]);
    }
    // Add the row to the table.
    $content['#rows'][] = $row;
  }
  // Redner the table and return.
  return drupal_render($content);
}