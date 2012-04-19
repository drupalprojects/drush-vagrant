<?php

/**
 * Register this extension's blueprints with Drush Vagrant
 *
 * @return
 *   Return an array of blueprints, where the key is the blueprint name and the
 *   value is the (relative) path to the blueprint itself.
 */
function hook_vagrant_blueprints() {
  $blueprints = array(
    'blueprint' => 'path/to/blueprint',
    'other'     => 'path/to/other',
  );
  return $blueprints;
}
