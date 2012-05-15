<?php

/**
 * @file
 *   Vagrant family of Drush commands for managing Vagrant-based VMs.
 */

define("DRUSH_VAGRANT_ROOT", dirname(__FILE__));

// Include all command and helper functions
include_once('includes/vagrant.inc');

/**
 * Implementation of hook_drush_help().
 */
function vagrant_drush_help($section) {
  switch ($section) {
    case 'meta:vagrant:title':
      return dt("Vagrant integration");
    case 'meta:vagrant:summary':
      return dt('Manage Vagrant-based networks of VMs.');
  }
}

/**
 * Implementation of hook_drush_init().
 */
function vagrant_drush_init() {
  $user_settings = drush_get_context('user');
  if (isset($user_settings['vagrant_project_root'])) {
    define("PROJECTS_PATH", $user_settings['vagrant_project_root'] . '/');
  }
  define("PROJECT_PATH", drush_get_option('project-path', '.'));
  define("VM_NAME", drush_get_option('vm-name', NULL));
}

/**
 * Implementation of hook_drush_command().
 */
function vagrant_drush_command() {
  $items = array();

  $items['vagrant'] = array(
    'description' => _drush_vagrant_help_text(),
    'options' => array(
      'project-path' => array(
        'description' => dt('The relative or absolute path to the project on which to run the command.'),
      ),
      'vm-name' => array(
        'description' => dt('The name of the Vagrant virtual machine on which to run the command.'),
      ),
    ),
    'aliases' => array('vg'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['vagrant-alias-config'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'description' => 'Convert the output of `vagrant ssh-config` into a Drush remote alias.',
  );
  $items['vagrant-aliases'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'description' => 'Generate aliases for a project and its VMs.',
  );
  $items['vagrant-list'] = array(
    'description' => dt('List current Vagrant projects, VMs and statuses.'),
    'options' => array(
      'all' => dt('When in a project directory, list all projects instead of just the current one.'),
      'only-projects' => dt('Only list project names.'),
      'only-vms' => dt('Only list VM names.'),
      ),
    'aliases' => array('vls'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['vagrant-blueprints'] = array(
    'description' => dt('List all blueprints.'),
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
      'drush vb --project-name=project1 --blueprint=aegir' => dt('Initialize a new Vagrant project using the \'aegir\' blueprint.'),
    ),
    'aliases' => array('vb'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['vagrant-hosts'] = array(
    'description' => dt('Manage entries in /etc/hosts. (Requires sudo password)'),
    'options' => array(
      'ip-addr' => array(
        'description' => dt('The IP address of the server or site.')),
      'domains' => array(
        'description' => dt('A comma-delimited list of domains.')),
      'remove' => array(
        'description' => dt('Remove an IP address or domain(s).')),
      'purge' => array(
        'description' => dt('Remove all entries in /etc/hosts made by Drush Vagrant.')),
    ),
    'examples' => array(
      'drush @machine1 vh --domains=foo.com,bar.net' => dt('Add and entry to /etc/hosts, so that foo.com and bar.net both point to the IP address of the Vagrant VM aliased as \'machine1\'.'),
      'drush vh --ip-addr=192.168.22.13 --domains=bar.com --remove' => dt('Remove the domain \'bar.com\' from the entry in /etc/hosts for IP address 192.168.22.13.'),
      'drush vh --ip-addr=192.168.22.13 --remove' => dt('Remove the entire entry in /etc/hosts for IP address 192.168.22.13.'),
      ),
    'aliases' => array('vh'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['vagrant-delete'] = array(
    'description' => dt('Delete a Vagrant project.'),
    'arguments' => array(
      'project' => dt('The project to delete.'),
    ),
    'examples' => array(
      'drush vagrant-delete project1' => dt('Delete the \'project1\' project.'),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'aliases' => array('vdl'),
  );
  $items['vagrant-shell'] = array(
    'description' => dt('Log into a Drush Vagrant VM via SSH.'),
    'arguments' => array(
      'remote-command' => dt('(OPTIONAL) Instead of logging into a VM, run this command in the VM. The command should be quoted if there are parameters. Returns the output of the command.'),
    ),
    'options' => array(
      'ssh-connection-string' => 'Instead of running SSH, return the a string of the command that would otherwise be run.',
    ),
    'examples' => array(
      'drush vagrant shell' => dt('Log into the VM in a single VM project.'),
      'drush @example vsh "hostname -f"' => dt('Return the FQDN of the VM/project specified in the @example alias.'),
    ),
    'aliases' => array('vsh'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['vagrant-user'] = array(
    'description' => "Initialize or update user-specific settings for Drush Vagrant.",
    'aliases' => array('vuser'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );

  // Drush topics
  $items['docs-vagrant-readme'] = array(
    'description' => 'Drush Vagrant README.',
    'hidden' => TRUE,
    'topic' => TRUE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_print_file',
    'callback arguments' => array(DRUSH_VAGRANT_ROOT . '/README.md'),
  );
  $items['docs-vagrant-architecture'] = array(
    'description' => 'Drush Vagrant architecture documentation.',
    'hidden' => TRUE,
    'topic' => TRUE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_print_file',
    'callback arguments' => array(DRUSH_VAGRANT_ROOT . '/docs/architecture.md'),
  );
  $items['docs-vagrant-blueprints'] = array(
    'description' => 'Drush Vagrant blueprints documentation.',
    'hidden' => TRUE,
    'topic' => TRUE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_print_file',
    'callback arguments' => array(DRUSH_VAGRANT_ROOT . '/docs/blueprints.md'),
  );
  $items['docs-vagrant-projects'] = array(
    'description' => 'Drush Vagrant projects documentation.',
    'hidden' => TRUE,
    'topic' => TRUE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_print_file',
    'callback arguments' => array(DRUSH_VAGRANT_ROOT . '/docs/projects.md'),
  );
  $items['docs-vagrant-api'] = array(
    'description' => 'Drush Vagrant API documentation.',
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
  $args = func_get_args();
  $command = array_shift($args);
  $commands = drush_get_commands();
  if (!isset($commands['vagrant-'. $command])) {
    if (empty($command)) {
      drush_shell_exec_interactive('drush vagrant --help');
    }
    elseif ($command == 'init' && !is_dir(PROJECT_PATH)) {
      drush_mkdir(PROJECT_PATH);
    }
    else {
      $vg_command = drush_get_context('argv');
      $vg_command = implode(' ', array_splice($vg_command, 1));
      $vg_command = strtr('cd !path && !command', array(
        '!path' => PROJECT_PATH,
        '!command' => $vg_command,
        )
      );

      drush_shell_exec_interactive($vg_command);
    }
  }
  else {
    $command = $commands['vagrant-'. $command];
    drush_dispatch($command, $args);
  }
}

function vagrant_drush_help_alter(&$command) {
  $args = drush_get_arguments();
  if (array_shift($args) == 'vagrant') {
    if (count($args)) {
      $vg_commands = vagrant_drush_command();
      foreach ($vg_commands as $key => $value) {
        $sub_commands = str_replace('vagrant-', '', array_keys($vg_commands));
      }
      if (in_array($args[0], $sub_commands)) {
        $vg_command = 'drush vagrant-' . $args[0] . ' --help';
      }
      else {
        $vg_command = strtr("vagrant !sub-commands --help", array('!sub-commands' => implode(' ', $args)));
        $output[] = dt("Drush Vagrant Integration will pass through native Vagrant commands. However, it allows the use of Drush aliases to run commands on projects and VMs remotely. Here is the help output from Vagrant itself on the '!command' sub-command:\n", array('!command' => implode(' ', $args)));
      }
      drush_shell_exec($vg_command);
      drush_print_r(drush_get_context('argv'));
      $output[] = implode("\n", drush_shell_exec_output());
      drush_print(implode("\n", $output));
      unset($command['options']);
      unset($command['aliases']);
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

/**
 * Implemention of hook_vagrant_blueprints().
 */
function vagrant_vagrant_blueprints() {
  $blueprints = array(
    'default' => array(
      'name' => 'Default',
      'description' => 'The default blueprint.',
      'path' => 'blueprints/default',
      ),
    );
  return $blueprints;
}
