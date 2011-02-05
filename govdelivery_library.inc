<?php
// $Id$

/**
 * @file
 * Implementation of drupal_mail_wrapper for the GovDelivery Integration module.
 */

function drupal_mail_wrapper($message, $cache = TRUE) {
  $key = md5(print_r($message, TRUE) . microtime() . strval(rand()));
  govdelivery_process_message($key, $message);
  return TRUE;
}
