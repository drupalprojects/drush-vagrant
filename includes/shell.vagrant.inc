<?php

/**
 * @file
 * Login to a VM via SSH.
 */


/**
 * Login to a VM via SSH.
 */
function drush_vagrant_shell() {
  $ssh_config = drush_vagrant_parse_ssh_config();
  $ssh_config['NoHostAuthenticationForLocalhost'] = 'yes';
  if (!drush_get_option('default-user')) {
    // Assume that we've included drush-vagrant::user, unless we've specified
    // that we want to use the default (vagrant) user, but allow overrides
    // using specific options. 
    $ssh_config['IdentityFile'] = drush_get_option('ssh-id', drush_get_option('vagrant_ssh_keyspublic', $ssh_config['IdentityFile']));
    $ssh_config['User'] = drush_get_option('ssh-user', drush_get_option('vagrant_username', $ssh_config['User']));
  }
  $hostname = $ssh_config['HostName'];
  unset($ssh_config['HostName']);
  unset($ssh_config['Host']);

  $command = sprintf('ssh %s %s', $hostname, drush_vagrant_render_ssh_options($ssh_config));

  $args = drush_get_original_cli_args_and_options();
  $command .= ' "' . implode(' ', $args) . '"';
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
