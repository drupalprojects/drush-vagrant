<?php

/**
 * Login to a VM via SSH.
 */
function drush_vagrant_shell() {
  $user_settings = drush_get_context('user');

  if (VM_NAME) {
    drush_shell_exec("cd %s && vagrant up %s && vagrant ssh-config %s", PROJECT_PATH, VM_NAME, VM_NAME);
  }
  else {
    drush_shell_exec("cd %s && vagrant up && vagrant ssh-config", PROJECT_PATH);
  }
  $ssh_config = drush_shell_exec_output();
  $ssh_hostname  = substr($ssh_config[2], 11);
  $ssh_port = substr($ssh_config[4], 7);

  $command = sprintf('ssh %s -l %s -i %s -p %s -o \'NoHostAuthenticationForLocalhost yes\'', $ssh_hostname, $user_settings['vagrant_username'], $user_settings['vagrant_ssh_keysprivate'], $ssh_port);

  $args = drush_get_arguments();
  $last_arg = $args[count($args) -1 ];
  if ($args[count($args) - 1] != 'shell' && $args[count($args) - 1] != 'vsh') {
    $command .= ' "' . $last_arg . '"';
  }
  if (drush_get_option('ssh-connection-string', FALSE)) {
    drush_print($command);
  }
  else {
    drush_shell_exec_interactive($command);
  }

}

/**
 * Implementation of drush_COMMAND_validate().
 */
function drush_vagrant_shell_validate() {
  drush_vagrant_get_project_path();
}
