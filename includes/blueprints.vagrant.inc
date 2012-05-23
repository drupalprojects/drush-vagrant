<?php

/**
 * Print a list of blueprints and the information they provide.
 */
function drush_vagrant_blueprints() {
  $blueprints = drush_vagrant_blueprint_info();
  foreach ($blueprints as $machine_name => $info) {
    drush_print('BLUEPRINT: ' . $machine_name);
    foreach ($info as $label => $value) {
      drush_print('   ' . $label . ': ' . $value);
    }
  }
}

/**
* Poll all Drush extensions implementing hook_vagrant_blueprints().
*
* @return
*   Returns an array of blueprints where the key is the name, and the value is
*   an array of information relevant to the blueprint.
*
* See also: docs/drush-vagrant.api.php
*/
function drush_vagrant_blueprint_info() {
  $extensions = drush_vagrant_extension_info();
  $blueprints =  drush_command_invoke_all('vagrant_blueprints');
  foreach ($extensions as $name => $info) {
    foreach ($info['blueprints'] as $key => $value) {
      $blueprints[$value]['extension'] = $name;
      $blueprints[$value]['full_path'] = $info['full_path'] . '/' . $blueprints[$value]['path'];
    }
  }
  // Ensure 'default' is the first blueprint
  $default = $blueprints['default'];
  unset($blueprints['default']);
  krsort($blueprints);
  $blueprints['default'] = $default;
  return array_reverse($blueprints);
}
