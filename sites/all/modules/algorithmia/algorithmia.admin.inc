<?php
/**
 * @file
 * Algorithmia administrative interface file.
 */

/**
 * Implements hook_admin_settings().
 */
function algorithmia_admin_settings_form($form_state) {
  $form['account'] = array(
    '#type' => 'fieldset',
    '#title' => t('General settings'),
  );

  $form['account']['algorithmia_api_key'] = array(
    '#title' => t('Algorithmia API Key'),
    '#type' => 'textfield',
    '#default_value' => variable_get('algorithmia_api_key'),
    '#size' => 30,
    '#maxlength' => 30,
    '#required' => TRUE,
    '#description' => t('Insert your Algorithmia API key. You may find this in
       your account settings on Algorithmia. If you do not have an account, sign up at algorithmia.com.'),
  );

  $form['algorithms'] = array(
    '#type' => 'fieldset',
    '#title' => t('Enable Algorithms'),
  );

  // Check existing value, enabled/disabled, true/false.
  $nudity_enabled = variable_get('algorithmia_nudity');
  $form['algorithms']['nudity'] = array(
    '#title' => t('Nudity Detection'),
    '#default_value' => $nudity_enabled,
    '#return_value' => 1,
    '#type' => 'checkbox',
    '#description' => t('This algorithm detects nudity in pictures. This will allow users who can administer files to check images for nudity in the file management UI at admin/content/file. https://algorithmia.com/algorithms/sfw/NudityDetection version  0.1.69'),
  );

  return system_settings_form($form);
}

/**
 * Implements _form_validate().
 */
function algorithmia_admin_settings_form_validate($form, &$form_state) {
  $response = _algorithmia_validate_api_key($form_state['values']['algorithmia_api_key']);
  if ($response->code == '200') {
    variable_set('algorithmia_api_key', $form_state['values']['algorithmia_api_key']);
    drupal_set_message(t('Your API key is valid. This API key will be used for all Algorithmia configuration on this site.'));
  }
  if ($response->code != '200') {
    drupal_set_message(t('Your API key is invalid. Please provide an API key, or the module will not operate.'));
  }

  // Nudity detection is enabled / disabled via admin UI.
  if ($form_state['values']['nudity'] == 1) {
    variable_set('algorithmia_nudity', TRUE);
  }
  elseif ($form_state['values']['nudity'] != 1) {
    variable_set('algorithmia_nudity', FALSE);
  }

}

/**
 * We validate the API key by posting to a simple algorithm.
 * 
 * @Param $key: Algorithmia API key.
 *
 * Algorithmia validates API key on every algorithm request. We use this to validate our form.
 */
function _algorithmia_validate_api_key($key) {
  $url = 'https://api.algorithmia.com/v1/algo/nlp/SentimentAnalysis/0.1.1';
  $options = array(
    'method' => 'POST',
    // Dummy data.
    'data' => '"HAPPY GOOD YES"',
    'timeout' => 30,
    'headers' => array(
      'content-type' => 'application/json',
      'Authorization' => 'Simple ' . $key,
    ),
  );
  $response = drupal_http_request($url, $options);
  return $response;
}

