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
    'description' => dt('Run a Vagrant command.'),
    'options' => array(
      'project-path' => array(
        'description' => dt('The relative or absolute path to the project on which to run the command.'),
      ),
      'vm-name' => array(
        'description' => dt('The name of the Vagrant virtual machine on which to run the command'),
      ),
      'params' => array(
        'description' => dt('Additional parameters to pass to Vagrant'),
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
    'description' => dt('List current Vagrant projects.'),
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
    'arguments' => array(
      'project' => dt('The name of the new Vagrant project to initialize.'),
    ),
    'options' => array(
      'blueprint' => array(
        'description' => dt('Specify the blueprint to use.')),
      'git-repo' => array(
        'description' => dt('Clone a Git repo, instead of using a blueprint.')),
      'hosts' => array(
        'description' => dt('Add an entry to /etc/hosts.')),
      'up' => array(
        'description' => dt('Start up the Vagrant project VM(s) immediately.')),
    ),
    'examples' => array(
      'drush aui project1 --blueprint=aegir' => dt('Initialize a new Vagrant project using the \'aegir\' blueprint.'),
    ),
    'aliases' => array('vb'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['vagrant-hosts'] = array(
    'description' => dt('Add an entry to /etc/hosts'),
    'options' => array(
      'ip-addr' => array(
        'description' => dt('The IP address of the server/site.')),
      'domains' => array(
        'description' => dt('Comma-delimited list of domains.')),
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
      'project' => dt('The project where the VM to log into exists.'),
      'VM' => dt('The VM to log into. Optional, if there is only one VM in the project.'),
    ),
    'examples' => array(
      'drush vagrant-ssh project1 vm1' => dt('Log into the \'vm1\' VM in the \'project1\' project.'),
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
  $vagrant_commands = array(
    'init',
    'status',
    'basebox',
    'box',
    'destroy',
    'gem',
    'halt',
    'package',
    'provision',
    'reload',
    'resume',
    'ssh',
    'ssh-config',
    'status',
    'suspend',
    'up',
    );
  $command = array_shift($args);
  if (empty($command)) {
    $command = 'alias-config';
  }
  elseif (in_array($command, $vagrant_commands)) {
    $vg_command = '';
    if ($project_path = drush_get_option('project-path')) {
      if ($command == 'init' && !is_dir($project_path)) {
        // TODO: create alias(es) for the project/VMs
        drush_mkdir($project_path);
      }
      $vg_command .= sprintf('cd %s && ', $project_path); 
    }
    $vg_command .= sprintf('vagrant %s', $command);
    if ($vm_name = drush_get_option('vm-name')) {
      $vg_command .= sprintf(' %s', $vm_name);
    }
    if ($params = drush_get_option('params')) {
      $vg_command .= sprintf(' %s', $params);
    }
    drush_shell_exec_interactive($vg_command);
  }
  else{
  $commands = drush_get_commands();
  $command = $commands['vagrant-'. $command];

  drush_dispatch($command, $args);
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
 * Poll all Drush extensions implementing hook_vagrant_blueprints().
 *
 * @blueprint
 *   Optional argument, used to return info for only one blueprint.
 *
 * @return
 *   Returns an array of blueprints where the key is the name, and the value is
 *   an array of information relevant to the blueprint.
 *
 * See also: docs/drush-vagrant.api.php
 */
function drush_vagrant_blueprint_info($blueprint = null) {
  $extensions = drush_vagrant_extension_info();
  $blueprints =  drush_command_invoke_all('vagrant_blueprints');
  foreach ($extensions as $name => $info) {
    foreach ($info['blueprints'] as $key => $value) {
      $blueprints[$value]['extension'] = $name;
      $blueprints[$value]['full_path'] = $info['full_path'] . '/' . $blueprints[$value]['path'];
    }
  }
  if ($blueprint) {
    return $blueprints[$blueprint];
  }
  return $blueprints;
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


