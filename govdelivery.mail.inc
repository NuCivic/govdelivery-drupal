<?php

/**
 * @file
 * Implementation of drupal_mail_wrapper for the GovDelivery Integration module.
 */

class GovDeliveryMailSystem implements MailSystemInterface {

  public function format(array $message) {

    return $message;
  }

  public function mail(array $message) {
    watchdog('ben', 'inside mail');
    $key = md5(print_r($message, TRUE) . microtime() . strval(rand()));
    govdelivery_process_message($key, $message);
    return TRUE;
  }
}
// end GovDeliveryMailSystem
