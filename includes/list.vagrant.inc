<?php

/**
 * @file
 * List projects and VMs.
 */

/**
 * List current projects, VMs and statuses.
 */
function drush_vagrant_list() {
  $projects = drush_vagrant_projects_info();

  if (drush_vagrant_check_project(PROJECT_PATH) && !drush_get_option('all', FALSE)) {
    // We're in a project, so only list its VMs, unless we're told otherwise
    $project = drush_vagrant_get_project_name(PROJECT_PATH);
    $projects = array( $project => $projects[$project]);
  }
  foreach ($projects as $project => $vms) {
    if (!drush_get_option('only-vms', FALSE)) {
      drush_print($project);
    }
    if (!drush_get_option('only-projects'))  {
      foreach ($vms as $vm_name => $vm) {
        if (drush_get_option('only-vms', FALSE)) {
          drush_print($vm_name);
        }
        else {
          drush_print('  ' . $vm_name . ' (' . $vm['status'] . ')');
        }
      }
    }
  }
}

/**
 * Return an array of current projects, their VMs and statuses
 */
function drush_vagrant_projects_info() {
  $projects = array();
  if ($dir = opendir(PROJECTS_PATH)) {
    while (false !== ($project = readdir($dir))) {
      $project_path = PROJECTS_PATH . $project;
      if (drush_vagrant_check_project($project_path)) {
        $projects[$project] = drush_vagrant_vms_info($project_path);
      }
    }
    closedir($dir);
  }
  return $projects;
}

/**
 * Return an array of VMs in a given project, and status.
 *
 * @project_path
 *   The path to look for the project
 */
function drush_vagrant_vms_info($project_path) {
  $vms = array();
  drush_shell_cd_and_exec($project_path, 'drush vagrant status');
  $output = drush_shell_exec_output();
  $blanks = array();
  foreach ($output as $key => $line) {
    if ($line == '') {
      $blanks[] = $key;
    }
  }
  $vms = array_slice($output, $blanks[0] + 1 , $blanks[1] - count($output));
  foreach ($vms as $key => $vm) {
    while(substr_count($vm,"  ") != 0){
      $vm = str_replace("  "," ",$vm);
    }
    $vm = explode(' ', $vm);
    unset($vms[$key]);
    if ($vm[0] != '') {
      $vms[array_shift($vm)] = array('status' => implode(' ', $vm));
    }
  }
  return $vms;
}

