<?php

/**
 * Login to a VM via SSH.
 */
function drush_vagrant_ssh() {

  _drush_vagrant_check_user_context();
  $user_settings = drush_get_context('user');

  $args = func_get_args();
  if (!($project = $args[0])) {
    drush_set_error(dt('ERROR: vagrant-ssh requires a project as an argument'));
    drush_print(dt('You can initialize a new project by running \'drush vagrant-init <project_name>\''));
    drush_print(dt('Currently available projects include:'));
    drush_shell_exec_interactive('drush vagrant-status');
    exit(1);
  }
  $vm = $args[1];
  $project_path = PROJECTS_PATH . $project;

  _drush_vagrant_check_project($project);

  if ($vm) {
    drush_shell_exec("cd %s && vagrant ssh-config %s", $project_path, $vm);
  }
  else {
    drush_shell_exec("cd %s && vagrant ssh-config", $project_path);
  }
  $ssh_config = drush_shell_exec_output();
  $ssh_hostname  = substr($ssh_config[1], 11);
  $ssh_port = substr($ssh_config[3], 7);

  drush_shell_exec_interactive('cd %s && vagrant up', $project_path);
  drush_shell_exec_interactive('ssh %s -l %s -i %s -p %s', $ssh_hostname, $user_settings['vagrant_username'], $user_settings['vagrant_ssh_keysprivate'], $ssh_port);

}
