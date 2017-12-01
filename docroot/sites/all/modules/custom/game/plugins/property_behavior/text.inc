<?php
/**
 * @file
 * The text behavior adds generic text behaviors for ECK.
 * 
 * 1) When then entity is view, we set the page title to be the 
 *    properties value.
 * 2) We set the property as the label of the entity.
 * 3) The property can be entered by the user with a text field.
 * 4) The property is displayed as an h1 tag.
 */

$plugin = array(
  'label' => "Text",
  'entity_view' => 'game_text_property_entity_view',
  'default_widget' => 'game_text_property_widget',
  'default_formatter' => 'game_text_property_formatter',
);

/**
 * How to input a title.
 */
function game_text_property_widget($property, $vars) {
  $entity = $vars['entity'];
  $title = _game_text_property_extract_value($entity, $property);
  return array(
    '#type' => 'textfield',
    '#title' => $vars['properties'][$property]['label'],
    '#maxlength' => 255,
    '#default_value' => $title,
    '#required' => TRUE,
  );
}

/**
 * Display the title as a simple div with classes.
 */
function game_text_property_formatter($property, $vars) {
  $entity = $vars['entity'];
  $text = _game_text_property_extract_value($entity, $property);
  $text = check_plain($text);
  $attributes = array(
    'id' => drupal_html_id("game_{$entity->id}_field_text_{$property}"),
    'class' => array(
      'game-field',
      'game-field-text',
      'field-' . drupal_html_class($property),
    ),
  );
  return array('#markup' => '<div ' . drupal_attributes($attributes) . ">{$text}</div>");
}

/**
 * Helper function that gets the title from an entity.
 *
 * @param object $entity
 *   An entity object.
 * @param string $property
 *   The name of the property that contains the title.
 *
 * @return string
 *   The title of the entity.
 */
function _game_text_property_extract_value($entity, $property) {
  $title = "";
  if (isset($entity->{$property})) {
    $title = $entity->{$property};
  }

  return $title;
}
