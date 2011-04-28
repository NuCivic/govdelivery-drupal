<?php

/**
 * @file
 * Drush functions for the GovDelivery Integration module.
 */


/**
 * Implementation of hook_drush_command().
 *
 * In this hook, you specify which commands your
 * drush module makes available, what it does and
 * description.
 *
 * Notice how this structure closely resembles how
 * you define menu hooks.
 *
 * @See drush_parse_command()
 *   for a list of recognized keys.
 *
 * @return
 *   An associative array describing your command(s).
 */
function govdelivery_drush_command() {
  $items = array();

  // the key in the $items array is the name of the command.
  $items['govdelivery-process-queue'] = array(
    // the name of the function implementing your command.
    'callback' => 'drush_govdelivery_process_queue',
    // a short description of your command
    'description' => "Runs the queue process for either the messages or subscribers queues.",
    'arguments' => array(
      'type' => 'The type of the queue that should be sent. (messages or subscribers)',
    ),
    'aliases' => array('govds', 'govdr'),
  );

  // the key in the $items array is the name of the command.
  $items['govdelivery-test-message'] = array(
    // the name of the function implementing your command.
    'callback' => 'drush_govdelivery_test_message',
    // a short description of your command
    'description' => "Sends a test message using govdelivery (Make sure to run the queue if you think it isn't working.)",
    'arguments' => array(
      'addresses' => 'Email to send a test message to.',
    ),
    'aliases' => array('govdtm'),
  );

  // the key in the $items array is the name of the command.
  $items['govdelivery-test-subscribe'] = array(
    // the name of the function implementing your command.
    'callback' => 'drush_govdelivery_test_subscribe',
    // a short description of your command
    'description' => "Subscribes a given email to a given topic code.",
    'arguments' => array(
      'code' => 'Subscription Topic Code',
      'addresses' => 'Email to subscribe',
    ),
    'aliases' => array('govdts'),
  );

  return $items;
}

/**
 * Implementation of hook_drush_help().
 *
 * This function is called whenever a drush user calls
 * 'drush help <name-of-your-command>'
 *
 * @param
 *   A string with the help section (prepend with 'drush:')
 *
 * @return
 *   A string with the help text for your command.
 */
function govdelivery_drush_help($section) {
  switch ($section) {
    case 'drush:govdelivery run queue':
      return dt("Runs the queue process for either the messages or subscribers queues.");
    case 'drush:govdelivery test message':
      return dt("Sends a test message using govdelivery (Make sure to run the queue if you think it isn't working.)");
    case 'drush:govdelivery test subscribe':
      return dt("Subscribes a given email to a given list code.");
  }
}

/**
 * Private helper function to switch queues used by run queue.
 * @return array 
 */
function _drush_govdelievery_queue_types() {
  $types = array(
    'messages' => array('govdelivery_message_queue', 'govdelivery_send_message'),
    'subscribers' => array('govdelivery_subscription_queue', 'govdelivery_queued_subscribe'),
  );
  return $types;
}

/**
 * Send messages from the govdelivery queue.
 *
 * Hard Coded maximum of 1,000,000 queue items can be sent at once. 
 * 
 * Typical Cron Time Limits are ignored.
 * 
 * @param
 *   The number of messages to send from the queue.
 */
function drush_govdelivery_process_queue($type = NULL) {
  $types = drush_govdelievery_queue_types();
  if ($type) {
    $sent = govdelivery_process_queue($types[$type]['0'], $types[$type]['1'], 1000000, TRUE);
  }
  else {
    $choice = drush_choice($types, 'Which queue would you like to process', '!key');
    $sent = govdelivery_process_queue($types[$choice]['0'], $types[$choice]['1'], 1000000, TRUE);
  }
  if ( ( $sent ) || ( $sent === 0 ) ) {
    drush_log("Message queue has been processed, " . $sent . " messages sent.", 'completed');
  }
  else {
    drush_log("Message queue could not be processed.  Check watchdog for errors.", 'failed', TRUE);
  }
}

function drush_govdelivery_test_message($address) {
  if ( !isset($address) || empty($address) ) {
    drush_log("Cannot send a test message without an address to send to", 'failed', TRUE);
    return FALSE;
  }

  $success = govdelivery_send_test_message($address);
  if ( $success ) {
    drush_log("Test message has been queued.  It will be sent the next time the queue is processed.", 'completed');
  }
  else {
    drush_log("Sending a test message failed", 'failed', TRUE);
  }
  return $success ;
}

function drush_govdelivery_test_subscribe($code, $address) {
  $success = govdelivery_subscribe($code, $address);
  if ( $success ) {
    drush_log("User has been subscribed to the list", 'completed');
  }
  else {
    drush_log("User could not be subscribed", 'failed', TRUE);
  }
  return $success ;
}
