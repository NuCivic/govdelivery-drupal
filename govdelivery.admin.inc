<?php
/**
 * @file
 * GovDelivery TMS admin UI.
 */

/**
 * Constructs settings form.
 *
 * @return array
 *   Form data.
 */
function govdelivery_settings() {
  $govdelivery_tms_settings = variable_get('govdelivery_tms_settings', array());
  $govdelivery_subscription_settings = variable_get('govdelivery_subscription_settings', array());

  $account_fieldset = array(
    '#type' => 'fieldset',
    '#title' => t('Mail accounts'),
  );

  if (isset($govdelivery_tms_settings) && isset($govdelivery_tms_settings['accounts']) && is_array($govdelivery_tms_settings['accounts'])) {
    foreach ($govdelivery_tms_settings['accounts'] as $username => $account_settings) {
      $account_fieldset[$username . '%###%fromname'] = array(
        '#type' => "textfield",
        '#title' => t('From name'),
        '#default_value' => $account_settings['fromname'],
      );
      $account_fieldset[$username . '%###%username'] = array(
        '#type' => "textfield",
        '#title' => t('Username'),
        '#default_value' => $username,
      );
      $account_fieldset[$username . '%###%password'] = array(
        '#type' => "textfield",
        '#title' => t('Password'),
        '#default_value' => $account_settings['password'],
      );
    }
  }

  $account_fieldset['fromname'] = array(
    '#type' => "textfield",
    '#title' => t('From name'),
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
    '#title' => t('URL for subscription service'),
    '#default_value' => url($govdelivery_subscription_settings['service_url']),
  );
  $subscription_fieldset['cron_subscriptions'] = array(
    '#type' => 'radios',
    '#default_value' => (isset($govdelivery_subscription_settings['cron_subscriptions']) ? $govdelivery_subscription_settings['cron_subscriptions'] : 1),
    '#title' => t('Subscriptions cron'),
    '#options' => array('Disabled', 'Enabled'),
    '#description' => t('Will automatically resend the subscriptions queue on cron runs.'),
  );

  $form = array(
    'govdelivery_tms_settings' => array(
      '#type' => 'fieldset',
      '#title' => t('Mail server settings'),
      '#tree' => TRUE,
      'accounts' => $account_fieldset,
      'default_username' => array(
        '#type' => "textfield",
        '#title' => 'Default mail username',
        '#default_value' => (!empty($govdelivery_tms_settings['default_username']) ? check_url($govdelivery_tms_settings['default_username']) : ''),
        '#description' => t('The username of the default mail account to use.'),
      ),
      'server' => array(
        '#type' => "textfield",
        '#title' => 'Server',
        '#default_value' => $govdelivery_tms_settings['server'],
        '#description' => t('Enter the URL of your GovDelivery TMS instance. It must begin with "https." Example: https://yourserver.govdelivery.com'),
      ),
      'auth_token' => array(
        '#type' => "textfield",
        '#title' => t('Auth token'),
        '#default_value' => (!empty($govdelivery_tms_settings['auth_token']) ? $govdelivery_tms_settings['auth_token'] : ''),
        '#description' => t('Enter the authentication token needed to send emails through your GovDelivery TMS service. If you do not have an authentication token, please content your GovDelivery account representative.'),
      ),
      'enabled' => array(
        '#type' => 'radios',
        '#default_value' => (isset($govdelivery_tms_settings['enabled']) ? $govdelivery_tms_settings['enabled'] : 1),
        '#title' => t('Use TMS for Outbound Mail'),
        '#options' => array('Disabled', 'Enabled'),
        '#description' => t('If this option is enabled, emails from your Drupal site will be sent using GovDelivery\'s Transactional Message Service (TMS). If it is disabled, emails we be sent using Drupal\'s standard SMTP-based email system.'),
      ),
      'queue' => array(
        '#type' => 'radios',
        '#default_value' => (isset($govdelivery_tms_settings['queue']) ? $govdelivery_tms_settings['queue'] : 0),
        '#title' => t('Queue Mail for High Volume'),
        '#options' => array('Disabled', 'Enabled'),
        '#description' => t('If this option is enabled, messages will be placed in a queue for delivery rather than be sent immediately. (Messages that fail to send will be queued anyway for later delivery.)'),
      ),
      'cron_tms' => array(
        '#type' => 'radios',
        '#default_value' => (isset($govdelivery_tms_settings['cron_tms']) ? $govdelivery_tms_settings['cron_tms'] : 0),
        '#title' => t('Messages cron'),
        '#options' => array('Disabled', 'Enabled'),
        '#description' => t('Will automatically resend the messages queue on cron runs.'),
        '#options' => array('Disabled', 'Enabled'),
      ),
    ),
    // Commenting out the GovDelivery subscription settings for now.
    // TODO -- re-enable these and document the functionality properly.
    // 'govdelivery_subscription_settings' => $subscription_fieldset,
    'save' => array('#type' => 'submit', '#value' => 'Save configuration'),
  );
  return $form;
}

/**
 * Handles settings form submission.
 *
 * @param array $form
 *   Form data.
 * @param array $postinfo
 *   POST info.
 */
function govdelivery_settings_submit($form, $postinfo) {
  // Change the account storage to an associative array indexed by username.
  $govdelivery_tms_settings = $postinfo['values']['govdelivery_tms_settings'];
  $govdelivery_subscription_settings = $postinfo['values']['govdelivery_subscription_settings'];
  $new_account = array();
  $account_settings = array();

  foreach ($govdelivery_tms_settings['accounts'] as $composite_key => $value) {
    // If we don't have a value we don't want to do anything.
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

  $govdelivery_tms_settings['accounts'] = $account_settings;

  // Server.
  variable_set('govdelivery_tms_settings', $govdelivery_tms_settings);
  variable_set('govdelivery_subscription_settings', $govdelivery_subscription_settings);

  if ($postinfo['values']['govdelivery_tms_settings']['enabled'] == 1) {
    $path = drupal_get_path('module', 'govdelivery');
    variable_set('smtp_library', $path . "/govdelivery_library.inc");
  }
  else {
    variable_del('smtp_library');
  }
}

/**
 * Send a test message to the specificed addresses.
 *
 * @return array
 *   Form data.
 */
function govdelivery_test_settings() {
  $form = array(
    'govdelivery_test_settings' => array(
      '#type' => 'fieldset',
      '#title' => t('GovDelivery test settings'),
      '#tree' => TRUE,
      'recipients' => array(
        '#type' => 'textfield',
        '#title' => t("Recipient's e-mail (comma separated)"),
      ),
      'test' => array(
        '#type' => 'submit',
        '#value' => t('Send test message'),
      ),
    ),
  );

  return $form;
}

/**
 * Handle test settings form submission.
 */
function govdelivery_test_settings_submit($form, $postinfo) {
  $recipients = $postinfo['values']['govdelivery_test_settings']['recipients'];
  // Send message.
  if (isset($recipients) && !empty($recipients)) {
    // Pass on the list to be sent.
    $status = govdelivery_send_test_message($recipients);
  }
  drupal_set_message(t('Your test message has been sent.'));
}

/**
 * Send a test message.
 *
 * @param string $send_to
 *   Email address to send to.
 *
 * @return array
 *   Email data.
 */
function govdelivery_send_test_message($send_to) {
  return drupal_mail('govdelivery', 'test_id', $send_to, array(), NULL, TRUE);
}

/**
 * Signup an email address for a specified list.
 *
 * Pass any known questions and answers along.
 *
 * @return array
 *   Test subscriptions form.
 */
function govdelivery_test_subscriptions() {
  $form = array(
    'govdelivery_test_subscriptions' => array(
      '#type' => 'fieldset',
      '#title' => t('GovDelivery settings test - subscriptions'),
      '#tree' => TRUE,
      'subscriber' => array(
        '#type' => 'textfield',
        '#title' => t("Subscriber's e-mail"),
      ),
      'list_code' => array(
        '#type' => 'textfield',
        '#title' => t('Subscription list code'),
      ),
      // Use Queue option.
      'test' => array(
        '#type' => 'submit',
        '#value' => t('Test send message'),
      ),
    ),
  );
  return $form;
}

/**
 * Validate number of subscribers.
 *
 * Need validation that there is only one subscriber or change to handle
 * multiple subscriptions.
 */
function govdelivery_test_subscriptions_submit($form, $postinfo) {
  $subscriber = $postinfo['values']['govdelivery_test_subscriptions']['subscriber'];
  $list_code = $postinfo['values']['govdelivery_test_subscriptions']['list_code'];

  // Commented code:
  // if (isset($subscriber) && !empty($subscriber) &&
  // isset($list_code) && !empty($list_code)) {
  $status = govdelivery_subscribe($list_code, $subscriber, FALSE);
  // }
  drupal_set_message(t('%response', array('%response' => $status ? 'Subscription added.' : 'Subscription failed.')));
}
