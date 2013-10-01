<?php
/**
 * @file
 * GovDelivery Taxonomy Module Settings UI
 */

/**
 * Builds taxonomy settings admin form.
 *
 * @return array
 *   Admin form array.
 */
function govdelivery_taxonomy_admin() {
  $form = array();
  $form['govdelivery_taxonomy_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Enter the web services user'),
    '#default_value' => variable_get('govdelivery_taxonomy_username', ''),
    '#maxlength' => 255,
    '#required' => TRUE,
  );
  $form['govdelivery_taxonomy_password'] = array(
    '#type' => 'password',
    '#title' => t('Enter the web services user password'),
    '#default_value' => variable_get('govdelivery_taxonomy_password', ''),
    '#maxlength' => 25,
    '#required' => TRUE,
  );
  $form['govdelivery_taxonomy_clientcode'] = array(
    '#type' => 'textfield',
    '#title' => t('GovDelivery client code'),
    '#default_value' => variable_get('govdelivery_taxonomy_clientcode', ''),
    '#maxlength' => 20,
    '#required' => TRUE,
  );
  $form['govdelivery_taxonomy_server'] = array(
    '#type' => 'textfield',
    '#title' => t('GovDelivery API Server (without http)'),
    '#default_value' => variable_get('govdelivery_taxonomy_server', ''),
    '#maxlength' => 100,
    '#required' => TRUE,
  );
  $form['govdelivery_taxonomy_drupalserver'] = array(
    '#type' => 'textfield',
    '#title' => t('Drupal Server base path (without http)'),
    '#default_value' => variable_get('govdelivery_taxonomy_drupalserver', ''),
    '#maxlength' => 100,
    '#required' => TRUE,
  );
  $form['govdelivery_taxonomy_category'] = array(
    '#type' => 'textfield',
    '#title' => t('GovDelivery taxonomy category code'),
    '#default_value' => variable_get('govdelivery_taxonomy_category', ''),
    '#maxlength' => 25,
    '#required' => TRUE,
  );
  return system_settings_form($form);
}
