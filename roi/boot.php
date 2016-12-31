<?php

  // load config file
  include_once('../get_config.php');

  // get config
  $json_file_path = get_config($config_file_lines, 'USER_ROI_DATA');

  // create json file when received json data
  if(isset($_POST['json'])){
    $json = json_decode($_POST['json']);
    file_put_contents($json_file_path, json_encode($json));
    chmod($json_file_path, 0644);
  }

  echo @file_get_contents($json_file_path);
