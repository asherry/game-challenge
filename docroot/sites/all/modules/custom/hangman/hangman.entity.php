<?php
// ========================================== SUB MODULE ========
if ($bundle = Bundle::loadByMachineName('game_hangman')) {
  $bundle->delete();
}

// Create bundle for entity.
$entity_type_bundle = new Bundle();
$entity_type_bundle->name = 'hangman';
$entity_type_bundle->config = array(
  'max_guesses' => 5,
);
$entity_type_bundle->entity_type = 'game';
$entity_type_bundle->addField('text', array(
  'field' => array(
    'field_name' => 'guesses',
    'cardinality' => -1,
    'settings' => array(
      'max_length' => 1,
    ),
  ),
  'instance' => array(
    'label' => 'Guesses',
  ),
));
$entity_type_bundle->addField('list_text', array(
  'field' => array(
    'field_name' => 'word',
    'settings' => array(
      'allowed_values' => array(
        '3dhubs' => '3DHubs',
        'marvin' => 'Marvin',
        'print' => 'Print',
        'filament' => 'Filament',
        'order' => 'Order',
        'layer' => 'Layer',
      ),
    ),
  ),
  'instance' => array(
    'label' => 'Word',
  ),
));
// current word - bundle word list
// guesses, multi-value text - bundle maximum guesses
// score - needs its own behaviors
$entity_type_bundle->save();


//$entity_type = EntityType::loadByName('game');
//$entity_type->save();

