<?php

/**
 * Print a status of workspaces and VMs.
 */
function drush_vagrant_status() {
  $args = func_get_args();
  $workspace = $args[0];
  $vm = $args[1];

  $workspaces = _drush_vagrant_list_workspaces();

  foreach ($workspaces as $entry) {
    drush_print($entry);
  }
}
