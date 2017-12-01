<?php

/**
 * @file
 * Hangman page callbacks.
 */

function hangman_landing_form($form, &$form_state) {
  $form['message'] = array(
    '#prefix' => '<h1>',
    '#markup' => t('Welcome to the Hangman game.'),
    '#suffix' => '</h1>',
  );

  $form['first_name'] = array(
    '#title' => t('First name'),
    '#type' => 'textfield',
    '#required' => TRUE,
  );

  $form['last_name'] = array(
    '#title' => t('Last Name'),
    '#type' => 'textfield',
    '#required' => TRUE,
  );

  $form['email'] = array(
    '#title' => t('Email'),
    '#type' => 'textfield',
    '#required' => TRUE,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Start game'),
  );

  return $form;
}
