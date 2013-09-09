<?php
/**
 * @file
 * Drush functions for the GovDelivery Integration module.
 */

/**
 * Implements hook_drush_command().
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
 */
function govdelivery_drush_command() {
  $items = array();

  // The key in the $items array is the name of the command.
  $items['govdelivery-send'] = array(
    // The name of the function implementing your command.
    'callback' => 'drush_govdelivery_send_queued_messages',
    // A short description of your command.
    'description' => "Send the messages in the govdelivery queue",
    'arguments' => array(
      'type' => 'The type of the queue that should be sent.',
    ),
    'aliases' => array('govds'),
  );

  // The key in the $items array is the name of the command.
  $items['govdelivery-test'] = array(
    // The name of the function implementing your command.
    'callback' => 'drush_govdelivery_send_test_message',
    // A short description of your command.
    'description' => "Send a test message using the GovDelivery service test functionality.",
    'arguments' => array(
      'addresses' => 'Email to send a test message to.',
    ),
  );

  // The key in the $items array is the name of the command.
  $items['govdelivery-subscribe'] = array(
    // The name of the function implementing your command.
    'callback' => 'govdelivery_drush_subscribe',
    // A short description of your command.
    'description' => "Sends subscriptions from the subscription queue.",
    'arguments' => array(
      'code' => 'List code for subscription',
      'addresses' => 'Email to subscribe',
    ),
  );

  return $items;
}

/**
 * Implements hook_drush_help().
 *
 * This function is called whenever a drush user calls
 * 'drush help <name-of-your-command>'
 */
function govdelivery_drush_help($section) {
  $help = NULL;
  switch ($section) {
    case 'drush:govdelivery send':
      $help = dt("Send messages or subscriptions from the GovDelivery queues.");
    case 'drush:govdelivery test':
      $help = dt("Sends a test message using govdelivery (this may be queued or sent immediately depending on the settings.)");
    case 'drush:govdelivery subscribe':
      $help = dt("Subscribes a user to a list managed by govdelivery (this may be queued or sent immediately depending on the settings.)");
  }
  return $help;
}

/**
 * Types of queue.
 *
 * @return array
 *   Queue types.
 */
function drush_govdelivery_queue_types() {
  $types = array(
    'messages' => array('govdelivery_message_queue', 'govdelivery_send_message'),
    'subscribers' => array('govdelivery_subscription_queue', 'govdelivery_queued_subscribe'),
  );
  return $types;
}

/**
 * Send messages from the govdelivery queue.
 *
 * @param string $type
 *   The number of messages to send from the queue.
 */
function drush_govdelivery_send_queued_messages($type = NULL) {
  $types = drush_govdelievery_queue_types();
  if ($type) {
    $messages_sent = govdelivery_process_queue($types[$type]['0'], $types[$type]['1']);
  }
  else {
    $choice = drush_choice($types, 'Which queue would you like to process', '!key');
    $messages_sent = govdelivery_process_queue($types[$choice]['0'], $types[$choice]['1']);
  }
  if (($messages_sent) || ($messages_sent === 0)) {
    drush_log("Message queue has been processed, " . $messages_sent . " messages sent.", 'completed');
  }
  else {
    drush_log("Message queue could not be processed.  Check watchdog for errors.", 'failed', TRUE);
  }
}

/**
 * Send a test message.
 *
 * @param string $address
 *   Email address.
 *
 * @return bool
 *   If successful.
 */
function drush_govdelivery_send_test_message($address) {
  if (!isset($address)) {
    drush_log("Cannot send a test message without an address to send to", 'failed', TRUE);
    return FALSE;
  }

    $success = govdelivery_send_test_message($address);
  if ($success) {
    drush_log("Test message has been queued.  It will be sent the next time the queue is processed.", 'completed');
  }
  else {
    drush_log("Sending a test message failed", 'failed', TRUE);
  }

  return $success;
}

/**
 * Subscribe to list.
 *
 * @param int $code
 *   List code.
 * @param string $address
 *   Email address.
 *
 * @return bool
 *   If successful.
 */
function govdelivery_drush_subscribe($code, $address) {
  $success = govdelivery_subscribe($code, $address);
  if ($success) {
    drush_log("User has been subscribed to the list", 'completed');
  }
  else {
    drush_log("User could not be subscribed", 'failed', TRUE);
  }
  return $success;
}
