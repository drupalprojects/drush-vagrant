<?php
/**
 * @file
 *   Vagrant family of Drush commands for managing Vagrant-based VMs.
 */

define("PROJECT_LABEL", "project");
define("BLUEPRINT_LABEL", "blueprint");
define("VM_LABEL", "VM");
define("DRUSH_VAGRANT_ROOT", dirname(__FILE__));
define("BLUEPRINT_DIR", DRUSH_VAGRANT_ROOT . "/lib/" . BLUEPRINT_LABEL . "s");

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
      return dt('Manage Vagrant-based networks of !VMs.', array('!VM' => VM_LABEL));
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
}

/**
 * Implementation of hook_drush_command().
 */
function vagrant_drush_command() {
  $items = array();

  $items['vagrant'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['vagrant-alias-config'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'description' => 'Convert the output of `vagrant ssh-config` into a Drush remote alias.',
  );
  $items['vagrant-status'] = array(
    'description' => dt('List current !project_labels.', array('!project_label' => PROJECT_LABEL)),
//    'arguments' => array(
//      PROJECT_LABEL => dt('An (optional) !project to present status information for.', array('!project'=> PROJECT_LABEL)),
//      VM_LABEL => dt('An (optional) !VM to present status information for.', array('!VM' => VM_LABEL)),
//    ),
//    'examples' => array(
//      'drush au Project1 hm' => dt('Print the status of the hm !VM in the Project1 !project.', array('!VM' => VM_LABEL, '!project'=> PROJECT_LABEL)),
//    ),
    'aliases' => array('vstat'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['vagrant-blueprints'] = array(
    'description' => dt('List all !blueprints.', array('!blueprint' => BLUEPRINT_LABEL)),
    'aliases' => array('vbl'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );

  $items['vagrant-initialize'] = array(
    'description' => dt('Initialize a new Drush Vagrant !project.', array('!project'=> PROJECT_LABEL)),
    'arguments' => array(
      PROJECT_LABEL => dt('The !project to initialize.', array('!project'=> PROJECT_LABEL)),
    ),
    'options' => array(
      BLUEPRINT_LABEL => array(
        'description' => dt('Specify a !blueprint to use. Defaults to \'default\'.', array('!blueprint' => BLUEPRINT_LABEL))),
      'git-repo' => array(
        'description' => dt('Clone a Git repo, instead of using a !blueprint.', array('!blueprint' => BLUEPRINT_LABEL))),
      'no-hosts' => array(
        'description' => dt('Don\'t edit /etc/hosts.')),
      'init-only' => array(
        'description' => dt('Don\'t start the !VM.', array('!VM' => VM_LABEL))),
    ),
    'examples' => array(
      'drush aui Project1 --' . BLUEPRINT_LABEL .'=aegir' => dt('Initialize a new Aegir-up !project using the \'aegir\' !blueprint.', array('!blueprint' => BLUEPRINT_LABEL, '!project'=> PROJECT_LABEL)),
    ),
    'aliases' => array('vinit', 'vagrant-init'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );

  $items['vagrant-delete'] = array(
    'description' => dt('Delete a Drush Vagrant !project.', array('!project'=> PROJECT_LABEL)),
    'arguments' => array(
      PROJECT_LABEL => dt('The !project to delete.', array('!project'=> PROJECT_LABEL)),
    ),
    'examples' => array(
      'drush vagrant-delete Project1' => dt('Delete the Project1 !project.', array('!project'=> PROJECT_LABEL)),
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['vagrant-ssh'] = array(
    'description' => dt('Log into a Drush Vagrant !VM via SSH.', array('!VM'=> VM_LABEL)),
    'arguments' => array(
      PROJECT_LABEL => dt('The !project where the !VM to log into exists.', array('!VM' => VM_LABEL, '!project'=> PROJECT_LABEL)),
      VM_LABEL => dt('The !VM to log into. Optional, if there is only one !VM in the !project.', array('!VM' => VM_LABEL, '!project'=> PROJECT_LABEL)),
    ),
    'examples' => array(
      'drush vagrant-ssh Project1 vm1' => dt('Log into the \'vm1\' !VM in the Project1 !project.', array('!VM' => VM_LABEL, '!project'=> PROJECT_LABEL)),
    ),
    'aliases' => array('vssh'),
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
  if (empty($command)) {
    $command = 'alias-config';
  }

  $commands = drush_get_commands();
  $command = $commands['vagrant-'. $command];

  drush_dispatch($command, $args);
}

/**
 * Return a list of all extensions that implement blueprints 
 */
function drush_vagrant_register_extensions() {
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
 * @return
 *   Returns an array of blueprints where the key is the name, and the value is
 *   an array of information relevant to the blueprint.
 *
 * See also: docs/drush-vagrant.api.php
 */
function drush_vagrant_register_blueprints() {
  $extensions = drush_vagrant_register_extensions();
  $blueprints =  drush_command_invoke_all('vagrant_blueprints');
  foreach ($extensions as $name => $info) {
    foreach ($info['blueprints'] as $key => $value) {
      $blueprints[$value]['extension'] = $name;
      $blueprints[$value]['full_path'] = $info['full_path'] . '/' . $blueprints[$value]['path'];
    }
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
      'path' => 'lib/blueprints/default',
      ),
    );
  return $blueprints;
}


