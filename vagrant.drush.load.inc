<?php

/**
 * @file
 * Check for dependencies and alter alias handling.
 */

/**
 * Implements hook_drush_load().
 */
function vagrant_drush_load() {
  // Check if Vagrant is installed
  $status = drush_shell_exec('which vagrant');
  if ($status == FALSE) {
    drush_log(dt('*** You have installed Drush Vagrant Integration, but Vagrant does not appear to be installed. *** \nPlease install Vagrant (http://vagrantup.com/) before continuing.\n\n'), 'warning');
  }
  else {
    /*
    $alias = drush_get_context('alias');
    // Ensure we're using a group alias
    if (isset($alias['#name']) && !isset($alias['type'])) {
      // Remove the project alias from the site-list
      if (array_shift($alias['site-list']) == $alias['#name'] . '.project') {
        drush_set_context('alias', $alias);
      }
    }
    */
    return TRUE;
  }
}
