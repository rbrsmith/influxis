<?php

/*
 * hook_views_api()
 */
function influxis_views_api() {
	return array(
			'api' => 3,
	);
}

/*
 * hook_views_data
 */
function influxis_views_data() {
	$data['influxis'] = array(
			'table' => array(
					'group' => t('Influxis'),
					'title' => 'Influxis',
					'join' => array(
							'node' => array(
									'left_field' => 'nid',
									'field' => 'nid',
							),
					),
			),
			'status' => array(
					'title' => t('Status'),
					'help' => t('Status of the video'),
		//			'field' => array(
		//					'handler' => 'views_handler_field_prerender_list',
		//					'click sortable' => TRUE,
		///			),
					'filter' => array(
							'handler' => 'influxis_hander_filter_influxis_status',
					),
					//			'sort' => array(
					//					'handler' => 'views_handler_sort',
					//			),
			),
	);
	return $data;
}





?>