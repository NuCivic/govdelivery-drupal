<?php

function govdelivery_settings() {
  $govdelivery_odm_settings = variable_get('govdelivery_odm_settings', array());
  $govdelivery_subscription_settings = variable_get('govdelivery_subscription_settings', array());

  $account_fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Mail Accounts'),
  );

  if (isset($govdelivery_odm_settings) && isset($govdelivery_odm_settings['accounts']) && is_array($govdelivery_odm_settings['accounts'])) {
    foreach ($govdelivery_odm_settings['accounts'] as $username => $account_settings) {
      $account_fieldset[$username . '%###%'  . 'fromname'] = array(
          '#type' => "textfield",
          '#title' => t('From Name'),
          '#default_value' => $account_settings['fromname'],
        );
      $account_fieldset[$username . '%###%' . 'username'] = array(
          '#type' => "textfield",
          '#title' => t('Username'),
          '#default_value' => $username,
        );
      $account_fieldset[$username . '%###%' . 'password'] = array(
        '#type' => "textfield",
        '#title' => t('Password'),
        '#default_value' => $account_settings['password'],
      );
    }
  }

  $account_fieldset['fromname'] = array(
    '#type' => "textfield",
    '#title' => t('From Name'),
  );
  $account_fieldset['username'] = array(
    '#type' => "textfield",
    '#title' => t('Username'),
  );
  $account_fieldset['password'] = array(
    '#type' => "textfield",
    '#title' => t('Password'),
  );

  $subscription_fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Subscription API'),
    '#tree' => TRUE,
  );

  $subscription_fieldset['service_url'] = array(
    '#type' => "textfield",
    '#title' => t('URL for Subscription Service'),
    '#default_value' => url($govdelivery_subscription_settings['service_url']),
  );

  $subscription_fieldset['cron_subscriptions'] = array(
    '#type' => 'radios',
    '#default_value' => (isset($govdelivery_subscription_settings['cron_subscriptions']) ? $govdelivery_subscription_settings['cron_subscriptions'] : 'Enabled'),
    '#title' => t('Subscriptions Cron (Will automatically resend the subscriptions queue on cron runs)'),
    '#options' => array('Disabled', 'Enabled'),
  );


  $form=array(
    'govdelivery_odm_settings' => array(
      '#type' => 'fieldset',
      '#title' => t('Mail Server Settings'),
      '#tree' => TRUE,
      'accounts' => $account_fieldset,
      'server' => array(
        '#type' => "textfield",
        '#title' => 'server',
        '#default_value' => $govdelivery_odm_settings['server'],
      ),
      'enabled' => array(
        '#type' => 'radios',
        '#default_value' => $govdelivery_odm_settings['enabled'],
        '#title' => t('Outbound mail rerouting'),
        '#options' => array('Disabled', 'Enabled'),
      ),
      'queue' => array(
        '#type' => 'radios',
        '#default_value' => $govdelivery_odm_settings['queue'],
        '#title' => t('Queue Messages (Messages that fail to send are queued regardless of this setting)'),
        '#options' => array('Disabled', 'Enabled'),
      ),
      'cron_odm' => array(
        '#type' => 'radios',
        '#default_value' => (isset($govdelivery_odm_settings['cron_odm']) ? $govdelivery_odm_settings['cron_odm'] : 'Enabled'),
        '#title' => t('Messages Cron (Will automatically resend the messages queue on cron runs)'),
        '#options' => array('Disabled', 'Enabled'),
      ),
    ),
    'govdelivery_subscription_settings' => $subscription_fieldset,
    'save' => array('#type' => 'submit', '#value' => 'Save Configuration'),
  );
  return $form;
//  return system_settings_form($form);
}

function govdelivery_settings_submit($form, $postinfo) {
  // change around the account storage to an associative array indexed by username
  $govdelivery_odm_settings = $postinfo['values']['govdelivery_odm_settings'];
  $govdelivery_subscription_settings = $postinfo['values']['govdelivery_subscription_settings'];
  $new_account = array();
  $account_settings = array();

  foreach ($govdelivery_odm_settings['accounts'] as $composite_key => $value) {
    // if we don't have a value we don't want to do anything
    if (isset($value) && !empty($value)) {
      $key_elements = explode('%###%', $composite_key);
      if (count($key_elements) == 2) {
        $username = $key_elements[0];
        $field = $key_elements[1];

        if (!isset($account_settings[$username]) && isset($username) && !empty($username)) {
          $account_settings[$username] = array();
        }
        if ($field != 'username') {
          $account_settings[$username][$field] = $value;
        }
      }
      else {
        if (isset($key_elements[0])) {
          $new_account[$key_elements[0]] = $value;
        }
      }
    }
  }

  if (isset($new_account['username'])) {
    $account_settings[$new_account['username']] = array(
      "password" => $new_account['password'],
      "fromname" => $new_account['fromname'],
    );
  }

  $govdelivery_odm_settings['accounts'] = $account_settings;

  variable_set('govdelivery_odm_settings', $govdelivery_odm_settings);
  variable_set('govdelivery_subscription_settings', $govdelivery_subscription_settings);

  if ($postinfo['values']['govdelivery_odm_settings']['enabled'] == 1) {
    $path = drupal_get_path('module', 'govdelivery');
    variable_set('smtp_library', $path . "/govdelivery_library.inc");
  }
  else{
    variable_del('smtp_library');
  }
}

// Send a test message to the specificed addresses.
function govdelivery_test_settings() {
  $form = array(
    'govdelivery_test_settings' => array(
      '#type' => 'fieldset',
      '#title' => t('GovDelivery Test Settings'),
      '#tree' => TRUE,
      'recipients' => array(
        '#type' => 'textfield',
        '#title' => t('Recipient\'s Email (Comma seperated)'),
      ),
      'test' => array(
      '#type' => 'submit',
      '#value' => t('Send test message'),
      ),
    ),
  );

  return $form;
}

function govdelivery_test_settings_submit($form, $postinfo) {
  $recipients = $postinfo['values']['govdelivery_test_settings']['recipients'];
  // Save settings and Send message.
  if (isset($recipients) && !empty($recipients)) {
    // Pass on the list to be sent.
    $status = govdelivery_send_test_message($recipients);
  }
  drupal_set_message(t('Your test message has been sent.'));
}

function govdelivery_send_test_message($send_to) {
  return drupal_mail('govdelivery', 'test_id', $send_to, array(), NULL, TRUE);
}

// Signup an email address for a specified list and pass any known questions and answers along.
function govdelivery_test_subscriptions() {
  $form = array(
    'govdelivery_test_subscriptions' => array(
      '#type' => 'fieldset',
      '#title' => t('GovDelivery Settings Test - Subscriptions'),
      '#tree' => TRUE,
      'subscriber' => array(
        '#type' => 'textfield',
        '#title' => t('Subscriber\'s Email'),
      ),
      'list_code' => array(
        '#type' => 'textfield',
        '#title' => t('Subscription List Code'),
      ),
      // Use Queue option
      'test' => array(
      '#type' => 'submit',
      '#value' => t('Test send message'),
      ),
    ),
  );

  return $form;
}

// Need validation that there is only one subscriber or change to handle multiple subscriptions.

function govdelivery_test_subscriptions_submit($form, $postinfo) {
  $subscriber = $postinfo['values']['govdelivery_test_subscriptions']['subscriber'];
  $list_code = $postinfo['values']['govdelivery_test_subscriptions']['list_code'];

  //if (isset($subscriber) && !empty($subscriber) && isset($list_code) && !empty($list_code)) {
    $status = govdelivery_subscribe($list_code, $subscriber, FALSE);
  //}

  drupal_set_message(t('%response', array('%response' => $status ? 'Subscription Added.' : 'Subscription Failed.')));
}

