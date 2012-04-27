<?php

/**
 * List current projects.
 */
function drush_vagrant_list() {
  $args = func_get_args();
  $project = $args[0];
  $vm = $args[1];

  $projects = _drush_vagrant_list_projects();

  foreach ($projects as $entry) {
    drush_print($entry);
  }
}
