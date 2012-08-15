<?php

/**
 * @file
 *   Vagrant family of Drush commands for managing Vagrant-based VMs.
 */

define("DRUSH_VAGRANT_ROOT", dirname(__FILE__));

// Include all command and helper functions
include_once('includes/vagrant.inc');

// Include hooks for our default blueprint
include_once('vagrant.blueprints.inc');

/**
 * Implements hook_drush_help().
 */
function vagrant_drush_help($section) {
  switch ($section) {
    case 'meta:vagrant:title':
      return dt('Vagrant integration');
    case 'meta:vagrant:summary':
      return dt('Manage Vagrant-based networks of VMs.');
  }
}

/**
 * Implements hook_drush_init().
 */
function vagrant_drush_init() {
  $user_settings = drush_get_context('user');
  if (isset($user_settings['vagrant_project_root'])) {
    define("PROJECTS_PATH", $user_settings['vagrant_project_root'] . '/');
  }
  define("PROJECT_PATH", drush_get_option('project-path', getcwd()));
  define("VM_NAME", drush_get_option('vm-name', NULL));
  drush_set_context('vagrant_help_text', _drush_vagrant_help_text());
}

/**
 * Implements hook_drush_command().
 */
function vagrant_drush_command() {
  $items = array();

  $items['vagrant'] = array(
    'description' => drush_get_context('vagrant_help_text', dt('Run a command on your Vagrant projects or VMs.')),
    'options' => array(
      'project-path' => array(
        'description' => dt('The relative or absolute path to the project on which to run the command.'),
      ),
      'vm-name' => array(
        'description' => dt('The name of the Vagrant virtual machine on which to run the command.'),
      ),
    ),
    'strict-option-handling' => TRUE,
    'allow-additional-options' => TRUE,
    'aliases' => array('vg'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['vagrant-alias-config'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'description' => dt('Convert the output of `vagrant ssh-config` into a Drush remote alias.'),
  );
  $items['vagrant-aliases'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'description' => dt('Generate aliases for a project and its VMs.'),
    'options' => array(
      'delete' => dt('Delete aliases for a project.'),
      ),
    'strict-option-handling' => TRUE,  
    'allow-additional-options' => TRUE,
  );
  $items['vagrant-list'] = array(
    'description' => dt('List current Vagrant projects, VMs and statuses.'),
    'options' => array(
      'all' => dt('When in a project directory, list all projects instead of just the current one.'),
      'projects' => dt('Only list project names.'),
      'vms' => dt('Only list VM names.'),
      ),
    'strict-option-handling' => TRUE,  
    'aliases' => array('vls'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['vagrant-blueprints'] = array(
    'description' => dt('List all blueprints.'),
    'strict-option-handling' => TRUE,  
    'aliases' => array('vbl'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['vagrant-build'] = array(
    'description' => dt('Build a new Drush Vagrant project.'),
    'options' => array(
      'project-name' => array(
        'description' => dt('The short name of the project.')),
      'blueprint' => array(
        'description' => dt('Specify the blueprint to use.')),
      'git-repo' => array(
        'description' => dt('Clone a Git repo, instead of using a blueprint.')),
      'aliases' => array(
        'description' => dt('Generate aliases for the project and its VMs.')),
      'hosts' => array(
        'description' => dt('Add entries to /etc/hosts.')),
      'up' => array(
        'description' => dt('Start up the Vagrant project VM(s) immediately.')),
    ),
    'examples' => array(
      'drush vb --project-name=project1 --blueprint=aegir' => dt('Initialize a new Vagrant project using the "aegir" blueprint.'),
    ),
    'drush dependencies' => array('hosts'),
    'strict-option-handling' => TRUE,  
    'aliases' => array('vb'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['vagrant-delete'] = array(
    'description' => dt('Delete a Vagrant project.'),
    'arguments' => array(
      'project' => dt('The project to delete.'),
    ),
    'examples' => array(
      'drush vagrant-delete project1' => dt('Delete the "project1" project.'),
    ),
    'drush dependencies' => array('hosts'),
    'strict-option-handling' => TRUE,  
    'aliases' => array('vdl'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['vagrant-shell'] = array(
    'description' => dt('Log into a Drush Vagrant VM via SSH.'),
    'arguments' => array(
      'remote-command' => dt('(OPTIONAL) Instead of logging into a VM, run this command in the VM. The command should be quoted if there are parameters. Returns the output of the command.'),
    ),
    'options' => array(
      'ssh-user' => 'The username to login under. Defaults to the username in the user-specific config file (~/drushrc.php).',
      'ssh-id' => 'The identity file from which the private key is read. Defaults to the private key in the user-specific config file (~/drushrc.php).',
      'ssh-connection-string' => 'Instead of running SSH, return the a string of the command that would otherwise be run.',
      'default-user' => 'Use the default "vagrant" user, and ignore other username or identity files, whether specified in the user-specific config file, or on the command line.',
    ),
    'examples' => array(
      'drush vagrant shell' => dt('Log into the VM in a single VM project.'),
      'drush @example vsh "hostname -f"' => dt('Return the FQDN of the VM/project specified in the @example alias.'),
    ),
    'strict-option-handling' => TRUE,  
    'allow-additional-options' => TRUE,
    'aliases' => array('vsh'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['vagrant-user'] = array(
    'description' => dt('Initialize or update user-specific settings for Drush Vagrant.'),
    'strict-option-handling' => TRUE,  
    'aliases' => array('vuser'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );

  // Drush topics
  $items['docs-vagrant-readme'] = array(
    'description' => dt('Drush Vagrant README.'),
    'hidden' => TRUE,
    'topic' => TRUE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_print_file',
    'callback arguments' => array(DRUSH_VAGRANT_ROOT . '/README.md'),
  );
  $items['docs-vagrant-architecture'] = array(
    'description' => dt('Drush Vagrant architecture documentation.'),
    'hidden' => TRUE,
    'topic' => TRUE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_print_file',
    'callback arguments' => array(DRUSH_VAGRANT_ROOT . '/docs/architecture.md'),
  );
  $items['docs-vagrant-blueprints'] = array(
    'description' => dt('Drush Vagrant blueprints documentation.'),
    'hidden' => TRUE,
    'topic' => TRUE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_print_file',
    'callback arguments' => array(DRUSH_VAGRANT_ROOT . '/docs/blueprints.md'),
  );
  $items['docs-vagrant-projects'] = array(
    'description' => dt('Drush Vagrant projects documentation.'),
    'hidden' => TRUE,
    'topic' => TRUE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_print_file',
    'callback arguments' => array(DRUSH_VAGRANT_ROOT . '/docs/projects.md'),
  );
  $items['docs-vagrant-api'] = array(
    'description' => dt('Drush Vagrant API documentation.'),
    'hidden' => TRUE,
    'topic' => TRUE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_print_file',
    'callback arguments' => array(DRUSH_VAGRANT_ROOT . '/docs/drush-vagrant.api.php'),
  );

  return $items;
}

/**
 * Command callback for `drush vagrant`.
 */
function drush_vagrant() {
  // hook_drush_command() will pass additional command-line arguments to the
  // specified hook function, so we can recuperate them here.
  $args = func_get_args();
  // Since Drush will parse aliases and options itself, the first argument will
  // be a sub-command
  $command = array_shift($args);
  // To determine whether we should pass along sub-command to Vagrant, we first
  // check that there is no such command implemented in Drush.
  $commands = drush_get_commands();
  if (!isset($commands['vagrant-' . $command])) {
    // If there's no sub-command, provide some guidance (i.e., help text)
    if (empty($command)) {
      drush_shell_exec_interactive('drush vagrant --help');
    }
    // TODO: does this special-case even make sense anymore?
    elseif ($command == 'init' && !is_dir(PROJECT_PATH)) {
      drush_mkdir(PROJECT_PATH);
    }
    else {
      // Get the command as passed on the CLI
      $vg_command = drush_get_context('argv');
      if ($key = array_search('vg', $vg_command)) {
        $vg_command[$key] = 'vagrant';
      }
      $vg_command = implode(' ', array_splice($vg_command, array_search('vagrant', $vg_command)));
      $vg_command = strtr('cd !path && !command', array(
        '!path' => PROJECT_PATH,
        '!command' => $vg_command,
        )
      );

      drush_shell_exec_interactive($vg_command);
    }
  }
  else {
    // Re-dispatch the drush-vagrant sub-command using original CLI arguments
    drush_shell_exec_interactive('drush vagrant-' . implode(' ', drush_get_original_cli_args_and_options()));
  }
}

/**
 * Implements hook_drush_help_alter().
 */
function vagrant_drush_help_alter(&$command) {
  if ($command['command'] === 'vagrant') {
    $args = drush_get_arguments();
    if (isset($args[1])) {
      // Remove the 'drush vagrant' options and aliases for sub-commands
      $command['options'] = array();
      $command['aliases'] = array();
    }
  }
}

/**
 * Return a list of all extensions that implement blueprints
 */
function drush_vagrant_extension_info() {
  $all_extensions = drush_commandfile_list();
  $extensions = drush_command_implements('vagrant_blueprints');
  foreach ($extensions as $key => $value) {
    $blueprints = array_keys(call_user_func($value . '_vagrant_blueprints'));
    $extensions[$value]['blueprints'] = $blueprints;
    $extensions[$value]['full_path'] = dirname($all_extensions[$value]);
    unset($extensions[$key]);
  }

  return $extensions;
}
