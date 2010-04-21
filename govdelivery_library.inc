<?php 
// $Id$ 
 
function drupal_mail_wrapper($message, $cache=true) {
  $key = md5(print_r($message,true) . microtime() . strval(rand()));
  govdelivery_process_message($key,$message);
  return true;
}
