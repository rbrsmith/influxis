<?php
/**
 * @file
 * Add influxis status for views to check status of video
 */
class influxis_hander_filter_influxis_status extends views_handler_filter_in_operator {
	/**
	 * override get_value_options() to return video status
	 */
  function get_value_options() {
    if (!isset($this->value_options)) {
     $fms = _influxis_();
      $this->value_options = $fms->getProcess();
    }
  }
}