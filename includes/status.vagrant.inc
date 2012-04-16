<?php

/**
 * Print a status of projects and VMs.
 */
function drush_vagrant_status() {
  $args = func_get_args();
  $project = $args[0];
  $vm = $args[1];

  $projects = _drush_vagrant_list_projects();

  foreach ($projects as $entry) {
    drush_print($entry);
  }
}
