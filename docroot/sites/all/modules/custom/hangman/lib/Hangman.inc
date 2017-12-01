<?php

/**
 * @file
 * Hangman class.
 */

/**
 * Hangman game class.
 */
class Hangman {

  const POINTS_CORRECT_ANSWER = 20;
  const POINTS_WIN = 250;
  const POINTS_PERFECT_GAME = 500;
  const POINTS_ALMOST_PERFECT_GAME = 250;
  const POINTS_BONUS_QUESTION = 1000000;
  const BONUS_QUESTION_CORRECT_RESPONSE = 'Alan Sherry';

  protected $score;

  protected $game;

  protected $gameWrapper;

  protected $max;

  public static function loadByGame(Entity $game) {
    return new Hangman($game);
  }

  public static function load($gid) {
    if ($game = entity_load($gid)) {
      return new Hangman($game);
    }
    return FALSE;
  }

  public static function loadByPlayerId($pid, $omit_gid = NULL) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'game');
    $query->entityCondition('bundle', 'hangman');
    $query->propertyCondition('pid', $pid);
    if ($omit_gid) {
      $query->propertyCondition('id', $omit_gid, '!=');
    }
    $result = $query->execute();
    return !empty($result['game']) ? entity_load('game', array_keys($result['game'])) : array(); 
  }

  public static function create($params = array()) {
    return new Hangman(self::createEntity($params));
  }

  public static function createEntity($params = array()) {
    $values = array_merge(array(
      'type' => 'hangman',
    ), $params);
    return entity_create('game', $values);
  }

  public function __construct(Entity $game) {
    $this->game = $game;
    $this->gameWrapper = entity_metadata_wrapper('game', $this->game);
    $this->word();
  }

  public function id() {
    return $this->gameWrapper->getIdentifier();
  }

  public function save() {
    $this->gameWrapper->save();
    return $this;
  }

  public function remaining() {
    $guesses = $this->gameWrapper->guesses->value();
    $wrong = array();
    foreach ($guesses as $letter) {
      if (!in_array($letter, $this->letters())) {
        $wrong[] = $letter;
      }
    }
    return $this->max() - count($wrong);
  }


  public function guess($value) {
    $guesses = $this->gameWrapper->guesses->value();
    $guesses[] = $value;
    $this->gameWrapper->guesses->set($guesses);
  }

  public function guesses() {
    return $this->gameWrapper->guesses->value();
  }

  public function word() {
    $word = $this->gameWrapper->word->value();
    if (empty($word)) {
      $this->generateWord();
    }
    return $this->gameWrapper->word->value();
  }

  public function screen() {
    $guesses = $this->guesses();
    $letters = $this->letters();
    foreach ($letters as &$letter) {
      if (!in_array($letter, $guesses)) {
        $letter = "_";
      }
    }
    return join(' ', $letters);
  }

  public function completed() {
    $letters = $this->letters();
    foreach ($this->guesses() as $guess) {
      $search = array_search($guess, $letters);
      if ($search !== FALSE) {
        unset($letters[$search]);
      }
    }
    return empty($letters);
  }

  public function score() {
    $this->score = 0;
    if ($this->completed()) {
      $this->score += self::POINTS_WIN;
    }
    $letters = $this->letters();
    $incorrect_count = 0;
    foreach ($this->guesses() as $guess) {
      if (in_array($guess, $letters)) {
        $this->score += self::POINTS_CORRECT_ANSWER;
      }
      else {
        $incorrect_count++;
      }
    }
    if ($incorrect_count == 0) {
      $this->score += self::POINTS_PERFECT_GAME;
    }
    if ($incorrect_count > 0 && $incorrect_count <= 2) {
      $this->score += self::POINTS_ALMOST_PERFECT_GAME;
    }
    if ($this->bonusQuestionResponse() == self::BONUS_QUESTION_CORRECT_RESPONSE) {
      $this->score += self::POINTS_BONUS_QUESTION;
    }
    return $this->score;
  }

  public function bonusQuestionResponse() {
    return $this->gameWrapper->bonus_question_response->value();
  }

  protected function generateWord() {
    $info = field_info_field('word');
    $words = $info['settings']['allowed_values'];
    $wordsArray = array_values(array_flip($words));
    $index = rand(0, count($words) - 1);
    $this->gameWrapper->word->set($wordsArray[$index]);
  }

  protected function letters() {
    return str_split($this->word());
  }

  protected function max() {
    if (isset($this->max)) {
      return $this->max;
    }
    $this->max = bundle_load('game', 'hangman')->config['max_guesses'];
    return $this->max;
  }
}
