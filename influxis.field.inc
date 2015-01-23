<?php

/**
 * @file
 * File used to group all field hooks together.
 *
 * Module creates an influxis video upload hook.
 */

/**
 * Implements hook_field_info().
 */
function influxis_field_info() {
  return array(
    'influxis' => array(
      'label' => t('Influxis'),
      'description' => t('This field stores the ID of a video file as an integer'),
      'default_widget' => 'influxis_upload',
      'default_formatter' => 'influxis_formatter_player',
      'settings' => array(
        'uri_scheme' => variable_get('file_default_scheme', 'public'),
      ),
      'instance_settings' => array(
        'file_extensions' => 'mp4 ogg avi mov wmv flv ogv webm',
        'file_directory' => 'influxis/original',
        'max_filesize' => '500',
      ),
    ),
  );

}

/**
 * Implements hook_field_widget_info().
 */
function influxis_field_widget_info() {
  return array(
    'influxis_upload' => array(
      'label' => t('Influxis Upload'),
      'field types' => array('influxis'),
      'settings' => array(
        'progress_indicator' => 'throbber',
      ),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_CUSTOM,
        'default value' => FIELD_BEHAVIOR_NONE,
      ),
    ),
  );
}

/**
 * Implements hook_field_widget_form().
 */
function influxis_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  // Error if this is not set.
  $field['settings']['display_field'] = 0;
  $elements = file_field_widget_form($form, $form_state, $field, $instance, $langcode, $items, $delta, $element);
  return $elements;
}

/**
 * Implements hook_field_settings_form().
 */
function influxis_field_settings_form($field, $instance, $has_data) {
  $scheme_options = array();
  foreach (file_get_stream_wrappers(STREAM_WRAPPERS_WRITE_VISIBLE) as $scheme => $stream_wrapper) {
    $scheme_options[$scheme] = $stream_wrapper['name'];
  }
  $defaults = field_info_field_settings($field['type']);
  $settings = array_merge($defaults, $field['settings']);

  $form['uri_scheme'] = array(
    '#type' => 'radios',
    '#title' => t('Upload destination of original file'),
    '#options' => $scheme_options,
    '#default_value' => $settings['uri_scheme'],
    '#description' => t('Select where the original video files should be stored. Private file storage has significantly more overhead than public files, but allows restricted access to files within this field.'),
  );
  return $form;
}

/**
 * Implements hook_field_instance_settings_form().
 */
function influxis_field_instance_settings_form(array $field, array $instance) {
  // Use the file field instance settings form as a basis.
  $form = file_field_instance_settings_form($field, $instance);
  return $form;
}


/**
 * Implements hook_field_is_empty().
 */
function influxis_field_is_empty($item, $field) {
  return file_field_is_empty($item, $field);
}

/**
 * Implements hook_field_formatter_info().
 */
function influxis_field_formatter_info() {
  // Define two formatter, the player and embed code.
  return array(
    'influxis_formatter_player' => array(
      'label' => t('Influxis Video'),
      'field types' => array('influxis'),
    ),
  );
}

/**
 * Implements hook_field_formatter_view().
 */
function influxis_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();
  foreach ($items as $delta => $item) {
    switch ($display['type']) {
      case 'influxis_formatter_player':
        $results = _influxis_get_records(array("fid", $item['fid'], "="));
        if ($results == NULL) {
          _influxis_error("Unable to locate file with influxis");
          return;
        }

        // Should only be one record via integrity constraint of db.
        $record = $results[0];

        // Update the video's status.
        _influxis_update_video($record);

        $fms = _influxis_();
        if ($fms == NULL) {
          return NULL;
        }
        // It's not completed.
        if ($record->status != 3) {
          $video = "<div class='influxis-status'>Video unavailable.  Status:  " . $fms->getProcess($record->status) . "</div>";
        }
        else {
          $config = array(
            'playlist' => array(
              'mp4' => $record->httpUrl,
            ),
            'ratio' => '3/4',
          );
          $video = theme('flowplayer5',
            array(
              'config' => $config,
              'id' => 'flowplayer5-influxis',
              'attributes' => array('style' => 'width:100%;height:350px;'),
            )
          );
        }

        $element[$delta] = array(
          '#type' => 'html_tag',
          '#tag' => 'p',
          '#value' => $video,
        );
        break;
    }
  }
  return $element;
}


/**
 * Implements hook_field_insert().
 */
function influxis_field_insert($entity_type, $entity, $field, $instance, $langcode, &$items) {
  $nid = $entity->nid;

  foreach ($items as $item) {
    $file = file_load($item['fid']);
    file_usage_add($file, 'influxis', 'node', $nid);
    _influxis_upload($item['fid'], $nid);
  }
}

/**
 * Implements hook_field_update().
 */
function influxis_field_update($entity_type, $entity, $field, $instance, $langcode, &$items) {
  $nid = $entity->nid;
  list($id, $vid, $bundle) = entity_extract_ids($entity_type, $entity);

  $original = entity_create_stub_entity($entity_type, array($id, $vid, $bundle));
  field_attach_load($entity_type, array($id => $original), FIELD_LOAD_CURRENT, array('field_id' => $field['id']));

  $new_fids = array();
  foreach ($items as $item) {
    array_push($new_fids, $item['fid']);
    // Check to see if video is already in db.
    $results = _influxis_get_records(array(
      array("fid", $item['fid'], '='),
      array("nid", $nid, '='),
    ));

    // Video has changed.
    if (count($results) == 0) {
      $file = file_load($item['fid']);
      file_usage_add($file, 'influxis', 'node', $nid);
      _influxis_upload($item['fid'], $nid);
    }
  }

  // Need to remove video if new video is uploaded.
  // Or if video remove button is clicked.
  if (!empty($original->{$field['field_name']})) {
    // Get removed fids.
    $original_fid = $original->{$field['field_name']}[LANGUAGE_NONE][0]['fid'];
    // Check to see if video has been removed.
    if (!in_array($original_fid, $new_fids)) {
      foreach ($original->{$field['field_name']}[$langcode] as $item) {
        // Remove fid.
        if ($item['fid'] == $original_fid) {
          $file = file_load($item['fid']);
          file_usage_delete($file, 'influxis');
          file_delete($file);
          _influxis_prepare_to_delete($file->fid);
          break;
        }
      }
    }
  }
}

/**
 * Implements hook_field_delete().
 */
function influxis_field_delete($entity_type, &$entity, $field, $instance, $langcode, &$items) {
  foreach ($items as $item) {
    $res = _influxis_get_records(array("fid", $item['fid'], "="));
    foreach ($res as $record) {
      _influxis_prepare_to_delete($record->fid);
    }
  }
}
