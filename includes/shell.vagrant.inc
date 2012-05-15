<?php

/**
 * Login to a VM via SSH.
 */
function drush_vagrant_shell() {
  $user_settings = drush_get_context('user');

  $vm = drush_get_option('vm-name', FALSE);
  $project_path = drush_get_option('project-path', '.');
  
  drush_vagrant_check_project($project_path, TRUE);
  if ($vm) {
    drush_shell_exec(strtr("cd !project_path && vagrant up !vm_name && vagrant ssh-config !vm_name",
      array( '!project_path' => $project_path, '!vm_name' => $vm))
    );
  }
  else {
    drush_shell_exec("cd %s && vagrant up && vagrant ssh-config", $project_path);
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
