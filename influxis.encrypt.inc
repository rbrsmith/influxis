<?php

/*
 * @file
 * File used to encrypt influxis account password
 */
class Encrypt {

  private $key = "thuwre3A";

  public function protect($data) {
    $td = mcrypt_module_open('tripledes', '', 'ecb', '');
    $iv = mcrypt_create_iv(mcrypt_enc_get_iv_size($td), MCRYPT_RAND);
    mcrypt_generic_init($td, $this->key, $iv);
    $encrypted_data = mcrypt_generic($td, $data);
    mcrypt_generic_deinit($td);
    mcrypt_module_close($td);
    return $encrypted_data;
  }

  public function reveal($protected) {
    $td = mcrypt_module_open('tripledes', '', 'ecb', '');
    $iv = mcrypt_create_iv(mcrypt_enc_get_iv_size($td), MCRYPT_RAND);
    mcrypt_generic_init($td, $this->key, $iv);
    $raw = mdecrypt_generic($td, $protected);
    mcrypt_generic_deinit($td);
    mcrypt_module_close($td);
    return $raw;
  }

}