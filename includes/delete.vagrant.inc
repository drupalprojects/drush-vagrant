<?php

/**
 * Validate the project to delete
 */
function drush_vagrant_delete_validate() {
  drush_vagrant_get_project_path();
}

/**
 * Delete an existing project.
 */
function drush_vagrant_delete() {

  $project_path = drush_vagrant_get_project_path();

  $project_alias = drush_get_context('alias');

  if (isset($project_alias['#name'])) {
    $project_alias = $project_alias['#name'];
  }
  else {
    $project_alias = '@' . drush_vagrant_get_project_name($project_path);
  }

  drush_print(dt('!!! WARNING: This operation cannot be undone. !!!'));
  drush_print(dt('This operation will delete the \'!project\' project, and destroy all of its VMs', array('!project' => drush_vagrant_get_project_name($project_path))));

  if (drush_confirm(dt('Are you certain that you want to proceed?'))) {
    drush_shell_cd_and_exec($project_path, 'drush vagrant list --only-vms');
    $vms = drush_shell_exec_output();
    foreach ($vms as $vm_name) {
      drush_shell_exec_interactive('drush %s.%s vagrant hosts --remove', $project_alias , $vm_name);
    }
    drush_shell_exec_interactive('cd %s && vagrant destroy --force', $project_path);
    if (!is_file($project_path . '/.vagrant')) {
      drush_shell_exec_interactive('cd %s && drush vagrant aliases --delete', $project_path);
      if ($project_path == getcwd()) {
        chdir(PROJECTS_PATH);
      }
      drush_shell_exec('rm -rf %s', $project_path);
    }
    else {
      drush_set_error(dt('Failure during \'vagrant destroy\''));
    }
  }
  else {
    drush_user_abort();
  }
}
