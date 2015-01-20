<?php
/**
 * @file
 * File for adding in addition view filter.
 */

/**
 * Implements hook_views_api().
 */
function influxis_views_api() {
  return array(
    'api' => 3,
  );
}

/**
 * Implements hook_views_data().
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
      'filter' => array(
        'handler' => 'influxis_hander_filter_influxis_status',
      ),
    ),
  );
  return $data;
}
