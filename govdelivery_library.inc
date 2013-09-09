<?php

/**
 * @file
 * Implementation of drupal_mail_wrapper for the GovDelivery Integration module.
 */

/**
 * Mail wrapper implementation.
 *
 * @param array $message
 *   Message to send.
 * @param bool $cache
 *   If to cache.
 *
 * @return bool
 *   True.
 */
function drupal_mail_wrapper($message, $cache = TRUE) {
  $key = md5(print_r($message, TRUE) . microtime() . strval(rand()));
  govdelivery_process_message($key, $message);
  return TRUE;
}
