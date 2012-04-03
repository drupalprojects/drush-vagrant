<?php

/**
 * Implementation of hook_drush_load().
 */
function aegirup_drush_load() {
  // Check if Vagrant is installed
  $status_vagrant = drush_shell_exec('which vagrant');
  // Check if NFS is installed
  $status_nfs = drush_shell_exec('man -f nfs');
  if ($status_vagrant == false) {
    drush_log(dt("*** You have installed Aegir-up, but Vagrant doesn't appear to be installed. *** \nPlease install Vagrant (http://vagrantup.com/) before continuing.\n\n"), 'warning');
  }
  elseif ($status_nfs == false) {
    drush_log(dt("*** You have installed Aegir-up, but NFS doesn't appear to be installed. *** \nPlease install NFS before continuing.\n\n"), 'warning');
  }
  else {
    return true;
  }
}
