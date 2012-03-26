<?php

/**
 * Implementation of hook_drush_load().
 */
function aegir_up_drush_load() {
  // Check if Vagrant is installed
  $status = drush_shell_exec('which vagrant');
  if ($status == false) {
    drush_print(dt("*** Vagrant must be installed for Aegir-up to run. *** \nInstall Vagrant (http://vagrantup.com/), or check the results of 'which vagrant'.\n\n"));
  }
  else {
    return true;
  }
}
