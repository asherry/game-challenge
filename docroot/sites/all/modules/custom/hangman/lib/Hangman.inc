<?php

/**
 * @file
 * Hangman class.
 */

/**
 * Hangman game class.
 */
class Hangman {
  protected $game;

  protected $gameWrapper;

  // switch to protected.

  public $max;


  public static function loadByGame(Entity $game) {
    return new Hangman($game);
  }

  public static function load($gid) {
    if ($game = entity_load($gid)) {
      return new Hangman($game);
    }
    return FALSE;
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
