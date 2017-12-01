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
function hangman_form($form, &$form_state, Entity $game) {
  $hangman = Hangman::loadByGame($game);
  $form['screen'] = array(
    '#markup' => $hangman->screen(),
  );
  $form['guess'] = array(
    '#title' => t('Guess'),
    '#type' => 'textfield',
    '#required' => TRUE,
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
  if (strlen($form_state['values']['guess']) > 1) {
    form_set_error('guess', t('Only one letter at a time please.'));
  }
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
  if ($hangman->remaining() == 0 || $hangman->completed()) {
    $form_state['redirect'] = "hangman/game/{$hangman->id()}/result";
  }
  else {
    $form_state['rebuild'] = TRUE;
  }
}

function hangman_results_form($form, &$form_state, Entity $game) {
  $hangman = Hangman::loadByGame($game);
  if ($hangman->remaining() > 0 && !$hangman->completed()) {
    drupal_set_message(t('This game is still in process.'), 'info');
    drupal_goto('hangman/game/' . $hangman->id());
  }

  $form['message'] = array(
    '#prefix' => '<h1>',
    '#markup' => $hangman->completed() ? t('You won! :-D') : t('Sorry, you lost. :-('),
    '#suffix' => '</h1>',
  );

  $form['score'] = array(
    '#prefix' => '<p>',
    '#markup' => t('Your score is <strong>@score</strong>.', array(
      '@score' => $hangman->score(),
    )),
    '#suffix' => '</p>',
  );

  $form['bonus_question'] = array(
    '#title' => t('Bonus question!'),
    '#type' => 'textfield',
    '#description' => t('Who is the best programmer in the world and should totally get a job with 3Dhubs?'),
    '#default_value' => Hangman::BONUS_QUESTION_CORRECT_RESPONSE,
    '#access' => empty($hangman->bonusQuestionResponse()),
  );

  $form['answer'] = array(
    '#type' => 'submit',
    '#value' => t('Answer bonus question'),
    '#access' => empty($hangman->bonusQuestionResponse()),
  );

  $form['play'] = array(
    '#type' => 'submit',
    '#value' => t('Play again?'),
  );

  $games = Hangman::loadByPlayerId($game->pid, $game->id);
  $form['games'] = array(
    '#prefix' => '<h4>' . t('Your scores:') . '</h4>',
    '#theme' => 'table',
    '#rows' => array(),
    '#header' => array('Game', 'Date', 'Score'),
    '#access' => !empty($games),
  );
  foreach ($games as $entity) {
    $col = array(
      $entity->id,
      format_date($entity->created, 'medium'),
      $entity->score,
    );
    $form['games']['#rows'][] = $col;
  }

  $form_state['game'] = $game;

  return $form;
}

function hangman_results_form_validate(&$form, &$form_state) {
  if ($form_state['values']['op'] == $form['answer']['#value']) {
    $response = $form_state['values']['bonus_question'];
    $default = $form['bonus_question']['#default_value'];
    if (strcasecmp($response, $default) !== 0) {
      form_set_error('bonus_question', t('Hmm, yeah, not sure you really got the whole point of this question.'));
    }
  }
}
function hangman_results_form_submit(&$form, &$form_state) {
  if ($form_state['values']['op'] == $form['answer']['#value']) {
    $game = $form_state['game'];
    $wrapper = entity_metadata_wrapper('game', $game);
    $wrapper->bonus_question_response->set($form_state['values']['bonus_question']);
    $wrapper->save();
    drupal_set_message(t('Great answer, thank you.'));
  }
  if ($form_state['values']['op'] == $form['play']['#value']) {
    $previous_game = $form_state['game'];
    $new_game = Hangman::create(array('pid' => $previous_game->pid))->save();
    $form_state['redirect'] = 'hangman/game/' . $new_game->id();
  }
}
