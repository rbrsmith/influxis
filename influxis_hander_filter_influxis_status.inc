<?php
/**
 * @file
 * Add influxis status for views to check status of video.
 */

class InfluxisHanderFilterInfluxisStatus extends views_handler_filter_in_operator {
  /**
	  * Override get_value_options() to return video status.
	  *
	  * Status can be uploading, encoding, completed, ready to delete, deleted.
	  */
  public function get_value_options() {
    if (!isset($this->value_options)) {
      $fms = _influxis_();
      $this->value_options = $fms->getProcess();
    }
  }
}
