<?php

/**
 * Register this extension's blueprints with Drush Vagrant
 *
 * @return
 *   Return an array of blueprints, where the key is the blueprint name and the
 *   value is an extensible list of information relating to the blueprint.
 */
function hook_vagrant_blueprints() {
  $blueprints = array(
    'default' => array(
      'name' => 'Default',
      'description' => 'The default blueprint.',
      'path' => 'blueprints/default', // req'd
      ),
    'another' => array(
      'name' => 'Second',
      'description' => 'Another blueprint.',
      'path' => 'blueprints/another', // req'd
      ),
    );
  return $blueprints;
}
