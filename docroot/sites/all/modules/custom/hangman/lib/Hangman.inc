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

  /**
   * Calculated game score.
   *
   * @var int
   */
  protected $score;

  /**
   * Game entity.
   *
   * @var Entity
   */
  protected $game;

  /**
   * Entity metadata wrapper.
   *
   * @var EntityDrupalWrapper.
   */
  protected $gameWrapper;

  /**
   * Max number of wrong guesses.
   *
   * @var int
   */
  protected $max;

  /**
   * Load a Hangman class by the game entity.
   *
   * @param Entity $game
   *   Fully loaded game object.
   *
   * @return Hangman
   *   New Hangman class.
   */
  public static function loadByGame(Entity $game) {
    return new Hangman($game);
  }

  /**
   * Load a Hangman class by game id.
   *
   * @param int $gid
   *   Game ID.
   *
   * @return Hangman
   *   New Hangman class.
   */
  public static function load($gid) {
    if ($game = entity_load($gid)) {
      return new Hangman($game);
    }
    return FALSE;
  }

  /**
   * Load game entities by player id.
   *
   * @param int $pid
   *   Player ID.
   * @param int $omit_gid
   *   Optional: a game ID to omit in the query.
   *
   * @return array
   *   Array of fully loaded game entities.
   */
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

  /**
   * Generate a new Hangman class.
   */
  public static function create($params = array()) {
    return new Hangman(self::createEntity($params));
  }

  /**
   * Create a new game entity of the hangman bundle.
   */
  public static function createEntity($params = array()) {
    $values = array_merge(array(
      'type' => 'hangman',
    ), $params);
    return entity_create('game', $values);
  }

  /**
   * PHP Constructor.
   */
  public function __construct(Entity $game) {
    $this->game = $game;
    $this->gameWrapper = entity_metadata_wrapper('game', $this->game);
    $this->word();
  }

  /**
   * Helper function to get game id.
   */
  public function id() {
    return $this->gameWrapper->getIdentifier();
  }

  /**
   * Save current game changes.
   */
  public function save() {
    $this->gameWrapper->save();
    return $this;
  }

  /**
   * Remaining amout of guesses.
   */
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

  /**
   * Add a new guess to the game.
   */
  public function guess($value) {
    $guesses = $this->gameWrapper->guesses->value();
    $guesses[] = $value;
    $this->gameWrapper->guesses->set($guesses);
  }

  /**
   * Get players current guesses.
   */
  public function guesses() {
    return $this->gameWrapper->guesses->value();
  }

  /**
   * Get the game's current word.
   */
  public function word() {
    $word = $this->gameWrapper->word->value();
    if (empty($word)) {
      $this->generateWord();
    }
    return $this->gameWrapper->word->value();
  }

  /**
   * Generate a display of the word with underscore.
   */
  public function screen() {
    $guesses = $this->guesses();
    $letters = $this->letters();
    foreach ($letters as &$letter) {
      if (!in_array($letter, $guesses)) {
        $letter = "_";
      }
    }
    return implode(' ', $letters);
  }

  /**
   * Have they guessed all the letters.
   */
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

  /**
   * Generate the game's current score.
   */
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

  /**
   * Get the saved bonus question response.
   */
  public function bonusQuestionResponse() {
    return $this->gameWrapper->bonus_question_response->value();
  }

  /**
   * Generate a new random word.
   */
  protected function generateWord() {
    $info = field_info_field('word');
    $words = $info['settings']['allowed_values'];
    $wordsArray = array_values(array_flip($words));
    $index = rand(0, count($words) - 1);
    $this->gameWrapper->word->set($wordsArray[$index]);
  }

  /**
   * Get an array of the letters of the current word.
   */
  protected function letters() {
    return str_split($this->word());
  }

  /**
   * Get max amount of guesses configured.
   */
  protected function max() {
    if (isset($this->max)) {
      return $this->max;
    }
    $this->max = bundle_load('game', 'hangman')->config['max_guesses'];
    return $this->max;
  }

}
