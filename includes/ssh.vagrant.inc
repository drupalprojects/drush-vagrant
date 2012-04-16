<?php

/**
 * Login to a VM via SSH.
 */
function drush_vagrant_ssh() {

  _drush_vagrant_check_user_context();
  $user_settings = drush_get_context('user');

  $args = func_get_args();
  if (!($workspace = $args[0])) {
    drush_set_error(dt('ERROR: vagrant-ssh requires a !workspace_label as an argument', array('!workspace_label' => WORKSPACE_LABEL)));
    drush_print(dt('You can initialize a new !workspace_label by running \'drush vagrant-init [<!workspace_label>]\'', array('!workspace_label' => WORKSPACE_LABEL)));
    drush_print(dt('Currently available !workspace_labels include:', array('!workspace_label' => WORKSPACE_LABEL)));
    drush_shell_exec_interactive('drush vagrant-status');
    exit(1);
  }
  $vm = $args[1];
  $workspace_path = WORKSPACE_ROOT . $workspace;

  _drush_vagrant_check_workspace($workspace);

  if ($vm) {
    drush_shell_exec("cd %s && vagrant ssh-config %s", $workspace_path, $vm);
  }
  else {
    drush_shell_exec("cd %s && vagrant ssh-config", $workspace_path);
  }
  $ssh_config = drush_shell_exec_output();
  $ssh_hostname  = substr($ssh_config[1], 11);
  $ssh_port = substr($ssh_config[3], 7);

  drush_shell_exec_interactive('cd %s && vagrant up', WORKSPACE_ROOT . '/' . $workspace);
  drush_shell_exec_interactive('ssh %s -l %s -i %s -p %s', $ssh_hostname, $user_settings['vagrant_username'], $user_settings['vagrant_ssh_keysprivate'], $ssh_port);

}
