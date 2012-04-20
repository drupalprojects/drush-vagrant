<?php

/**
 * Print a list of blueprints and the information they provide.
 */
function drush_vagrant_blueprints() {

  $blueprints = drush_vagrant_register_blueprints();
  foreach ($blueprints as $machine_name => $info) {
    drush_print('BLUEPRINT: ' . $machine_name);
    foreach ($info as $label => $value) {
      drush_print('   ' . $label . ': ' . $value);
    }
  }
}
