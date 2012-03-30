<?php

/**
 * Implementation of hook_drush_load().
 */
function aegirup_drush_load() {
  // Check if Vagrant is installed
  $status = drush_shell_exec('which vagrant');
  if ($status == false) {
    drush_log(dt("*** You have installed Aegir-up, but Vagrant doesn't appear to be installed. *** \nPlease install Vagrant (http://vagrantup.com/) before continuing.\n\n"), 'warning');
  }
  else {
    return true;
  }
}
