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

function hangman_landing_form_submit(&$form, &$form_state) {
  $values = $form_state['values'];
  $player = entity_create('player', array(
    'type' => 'basic',
    'first_name' => $values['first_name'],
    'last_name' => $values['last_name'],
    'email' => $values['email'],
  ));
  $player->save();
  $hangman = Hangman::create(array('pid' => $player->id))->save();
  $form_state['redirect'] = 'hangman/game/' . $hangman->id();
}

/**
 * Page callback for hangman form api form.
 */
function hangman_form($form, &$form_state, $game) {
  $hangman = Hangman::loadByGame($game);
  $form['screen'] = array(
    '#markup' => $hangman->screen(),
  );
  $form['guess'] = array(
    '#title' => t('Guess'),
    '#type' => 'textfield',
    '#description' => t('You have @count @label remaining.', array(
      '@count' => $hangman->remaining(),
      '@label' => $hangman->remaining() > 1 ? 'guesses' : 'guess',
    )),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => 'Submit',
  );

  $form_state['game'] = $game;
  return $form;
}

function hangman_form_validate(&$form, &$form_state) {
  $hangman = Hangman::loadByGame($form_state['game']);
  $guesses = $hangman->guesses();
  if (in_array($form_state['values']['guess'], $guesses)) {
    form_set_error('guess', t('Silly, you already guessed that.'));
  }
}

/**
 * Form API submit callback.
 */
function hangman_form_submit(&$form, &$form_state) {
  $hangman = Hangman::loadByGame($form_state['game']);
  $hangman->guess($form_state['values']['guess']);
  $hangman->save();
  if (!$hangman->remaining()) {
    drupal_set_message(t('It\'s all over.'));
  }
  $form_state['rebuild'] = TRUE;
}
