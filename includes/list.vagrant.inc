<?php

/**
 * List current projects, VMs and statuses.
 */
function drush_vagrant_list() {
  $project_path = drush_get_option('project-path', getcwd());

  $projects = drush_vagrant_projects_info();

  if (drush_vagrant_check_project($project_path) && !drush_get_option('all', FALSE)) {
    // We're in a project, so only list its VMs, unless we're told otherwise
    $project = drush_vagrant_get_project_name($project_path);
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
function drush_vagrant_projects_info(){
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
function drush_vagrant_vms_info($project_path){
  $vms = array();
  drush_shell_cd_and_exec($project_path, 'drush vagrant status');
  $vms = drush_shell_exec_output();
  $vms = array_slice($vms, 2, -4);
  foreach ($vms as $key => $vm) {
    $vm = explode(' ', $vm);
    unset($vms[$key]);
    if ($vm[0] != '') {
      $vms[array_shift($vm)] = array('status' => array_pop($vm));
    }
  }
  return $vms;
}

