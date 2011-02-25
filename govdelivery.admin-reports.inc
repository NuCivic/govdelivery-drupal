<?php

// Report of the GovDelivery Queues status.
// Subject lines are links to a details page.
function govdelivery_message_queue_report() {
  $queue = 'govdelivery_message_queue';
  // Setup table fields mapping to the data.
  $header = array(
    array('data' => 'Subject'),
    array('data' => 'Created', 'field' => 'day', 'sort' => 'DESC'),
    array('data' => 'Form'),
  );

  // Query the database for the data.
  $dataquery = "SELECT FROM_DAYS(TO_DAYS(FROM_UNIXTIME(created))) day, cid FROM {". $queue ."} GROUP BY day, cid " . tablesort_sql($header);
  $countquery = "SELECT COUNT(*) FROM {". $queue ."}";
  $result = pager_query($dataquery, 30, 0, $countquery);

  // Assign data to rows.
  $rows = array();
  while ($queuedmessage = db_fetch_object($result)) {
    // Get the message unserialized.
    $queuedmessage->data = govdelivery_queue_get($queue, $queuedmessage->cid);
    // Build the row.
    if ($queuedmessage->data != 0) {
      $subject_text = $queuedmessage->data['subject'] ? $queuedmessage->data['subject'] : t('(no subject)');
      $subject_link = l($subject_text, 'admin/reports/govdelivery_message_queue/details/' . $queuedmessage->cid);
      $rows[] = array($subject_link, $queuedmessage->day, check_plain($queuedmessage->data['id']));
    }
  }// End while.

  // Empty dataset message.
  if (empty($rows)) {
    $rows[] = array(array('data' => t('No messages currently in the queue.'), 'colspan' => 3));
  }

  $output  = theme('table', $header, $rows);
  $output .= theme('pager', NULL, 30, 0);

  return $output;
}// End govdelivery_message_queue_status.

// View a single message in the queue.
function govdelivery_details($cid) {
  $queue = 'govdelivery_message_queue';
  // Build a table to display the message details.
  if (!empty($cid)) {
    // Get the message unserialized.
    $queuedmessage = govdelivery_queue_get($queue, $cid);
    if ($queuedmessage != 0) {
      // Setup table fields mapping to the data.
      $header = array(
        array('data' => t('Field')),
        array('data' => t('Value')),
      );
      // Setup the rows.
      $rows = array(
        array('CID', check_plain($cid) ),
        array('ID', check_plain($queuedmessage['id']) ),
        array('To', check_plain($queuedmessage['to']) ),
        array('Errors-To', check_plain($queuedmessage['headers']['Errors-To']) ),
        array('Return-Path', check_plain($queuedmessage['headers']['Return-Path']) ),
        array('Sender', check_plain($queuedmessage['headers']['Sender']) ),
        array('Language', (empty($queuedmessage['language']) ? '' : implode(',', $queuedmessage['language'])) ),
        array('Parameters', check_plain($queuedmessage['params']) ),
        array('Subject', check_plain($queuedmessage['subject']) ),
        array('Body', check_plain($queuedmessage['body']) ),
      );
    }
  }

  // Empty dataset message.
  // If the data set is empty dont load the form.
  if (empty($rows)) {
    $rows[] = array(array('data' => t('Message no longer in the queue.'), 'colspan' => 2));
  }
  else {
    $output .= drupal_get_form('govdelivery_details_form', $cid);
  }
  $output .= theme('table', $header, $rows);

  return $output;
}

// A form to send a message immediately.
function govdelivery_details_form($form, $cid) {
  $form = array(
    'govdelivery_details' => array(
      '#type' => 'fieldset',
      '#title' => t('Message Actions'),
      '#tree' => TRUE,
      '#collapsible' => TRUE,
      'cid' => array('#type' => 'value', '#value' => $cid),
      'send' => array('#type' => 'submit', '#value' => t('Send Now')),
    ),
  );
  return $form;
}

function govdelivery_details_form_submit($form, &$postinfo) {
  $cid = $postinfo['values']['govdelivery_details']['cid'];
  // Send the message from the queue immediately.
  if (!empty($cid)) {
    $sent = govdelivery_process_message_queue(NULL, FALSE, $cid);
    drupal_set_message(t('Messages sent from the queue: @sent', array('@sent' => $sent)));
  }
  $postinfo['redirect'] = 'admin/reports/govdelivery_message_queue';
}
