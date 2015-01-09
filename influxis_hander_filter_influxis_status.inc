<?php
	
class influxis_hander_filter_influxis_status extends views_handler_filter_in_operator {
	  function get_value_options() {
   		 if (!isset($this->value_options)) {
   		 	$fms = _influxis_();
   		 	$this->value_options = $fms->getProcess();
   		 	
   		 }
	  }

}
	
?>