<?php
// $Id$

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
  $items['govdelivery-send'] = array(
    // the name of the function implementing your command.
    'callback' => 'drush_govdelivery_send_queued_messages',
    // a short description of your command
    'description' => "Send the messages in the govdelivery queue",
    'arguments' => array(
      'messages' => 'The number of messages to send in this run.  If the argument is not provided all messages will be sent.',
    ),
  );

  // the key in the $items array is the name of the command.
  $items['govdelivery-test'] = array(
    // the name of the function implementing your command.
    'callback' => 'drush_govdelivery_send_test_message',
    // a short description of your command
    'description' => "Send a test message using the GovDelivery service test functionality.",
    'arguments' => array(
      'addresses' => 'Email to send a test message to.',
    ),
  );

  // the key in the $items array is the name of the command.
  $items['govdelivery-subscribe'] = array(
    // the name of the function implementing your command.
    'callback' => 'govdelivery_drush_subscribe',
    // a short description of your command
    'description' => "Send a test message using the GovDelivery service test functionality.",
    'arguments' => array(
      'code' => 'List code for subscription',
      'addresses' => 'Email to subscribe',
    ),
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
    case 'drush:govdelivery send':
      return dt("Sends messages from the govdelivery queue.");
    case 'drush:govdelivery test':
      return dt("Sends a test message using govdelivery (this will be queued and sent on the next queue process)");
    case 'drush:govdelivery subscribe':
      return dt("Subscribes a user to a list managed by govdelivery");
  }
}

/**
 * Send messages from the govdelivery queue.
 *
 * @param
 *   The number of messages to send from the queue.
 */
function drush_govdelivery_send_queued_messages($messages = NULL) {
  $messages_sent = govdelivery_process_queue($messages, TRUE);
  if ( ( $messages_sent ) || ( $messages_sent === 0 ) ) {
    drush_log("Message queue has been processed, " . $messages_sent . " messages sent.", 'completed');
  }
  else {
    drush_log("Message queue could not be processed.  Check watchdog for errors.", 'failed', TRUE);
  }
}

function drush_govdelivery_send_test_message($address) {
  if ( !isset($address) ) {
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

function govdelivery_drush_subscribe($code, $address) {
  $success = govdelivery_subscribe($code, $address);
  if ( $success ) {
    drush_log("User has been subscribed to the list", 'completed');
  }
  else {
    drush_log("User could not be subscribed", 'failed', TRUE);
  }
  return $success ;
}
