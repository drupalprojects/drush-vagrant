<?php
/**
 * drush-vagrant helps you add your Vagrant VMs as drush remote aliases.
 */

/**
 * Implementation of hook_drush_help().
 */
function vagrant_drush_help($section) {
  switch ($section) {
    case 'meta:vagrant:title':
      return dt("Vagrant integration");
    case 'meta:vagrant:summary':
      return dt("Add Vagrant VMs as drush remotes.");
  }
}

/**
 * Implementation of hook_drush_command().
 */
function vagrant_drush_command() {
  $items['vagrant'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['vagrant-alias-config'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'description' => 'Convert the output of `vagrant ssh-config` into a Drush remote alias.',
  );
  return $items;
}

/**
 * Command callback for `drush vagrant`.
 */
function drush_vagrant() {
  $args = func_get_args();
  $command = array_shift($args);
  if (empty($command)) {
    $command = 'alias-config';
  }

  $commands = drush_get_commands();
  $command = $commands['vagrant-'. $command];

  drush_dispatch($command, $args);
}
