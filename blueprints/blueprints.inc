<?php

/**
 * @file
 *
 * Hooks related to the Default blueprint.
 */

/**
 * Implemention of hook_vagrant_blueprints().
 */
function vagrant_vagrant_blueprints() {
  $blueprints = array(
    'default' => array(
      'name' => 'Default',
      'description' => 'The default blueprint.',
      'path' => 'blueprints/default',
      ),
    );
  return $blueprints;
}

/**
 * Implementation of drush_hook_pre_COMMAND().
 *
 * Set SSH username and identity file options
 */
function drush_vagrant_pre_vagrant_shell() {
  include_once(PROJECT_PATH . '/.config/blueprint.inc');
  if ($options['blueprint']['blueprint'] == 'default') {
    $user_settings = drush_get_context('user');
    drush_set_option('ssh-user', drush_get_option('ssh-user', $user_settings['vagrant_username']));
    drush_set_option('ssh-id', drush_get_option('ssh-id', $user_settings['vagrant_ssh_keysprivate']));
  }
}
