<?php

/**
 * @file
 *   Aegir-up family of drush commands for managing Vagrant-based VMs.
 */

define("WORKSPACE_LABEL", "project");
define("BLUEPRINT_LABEL", "template");
define("VM_LABEL", "VM");
define("AEGIR_UP_ROOT", dirname(__FILE__).'/');
define("WORKSPACE_ROOT", AEGIR_UP_ROOT . WORKSPACE_LABEL . "s/");
define("BLUEPRINT_DIR", AEGIR_UP_ROOT . "lib/" . BLUEPRINT_LABEL . "s/");


/**
 * Implementation of hook_drush_help().
 */
function aegirup_drush_help($section) {
  switch ($section) {
    case 'meta:aegirup:title':
      return dt('Aegir-up commands');
    case 'meta:aegirup:summary':
      return dt('Manage Vagrant-based networks of !VMs.', array('!VM' => VM_LABEL));
  }
}

/**
 * Implementation of hook_drush_command().
 */
function aegirup_drush_command() {
  $items = array();

  $items['aegir-up'] = array(
    'description' => "Print the status of your Aegir-up system.",
    'arguments' => array(
      WORKSPACE_LABEL => dt('An (optional) !workspace to present status information for.', array('!workspace'=> WORKSPACE_LABEL)),
      VM_LABEL => dt('An (optional) !VM to present status information for.', array('!VM' => VM_LABEL)),
    ),
    'examples' => array(
      'drush au Workspace1 hm' => dt('Print the status of the hm !VM in the Workspace1 !workspace.', array('!VM' => VM_LABEL, '!workspace'=> WORKSPACE_LABEL)),
    ),
    'aliases' => array('au'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['aegir-up-init'] = array(
    'description' => dt('Initialize a new Aegir-up !workspace.', array('!workspace'=> WORKSPACE_LABEL)),
    'arguments' => array(
      WORKSPACE_LABEL => dt('The !workspace to initialize. Defaults to \'Aegir\'.', array('!workspace'=> WORKSPACE_LABEL)),
    ),
    'callback' => 'drush_aegirup_aegir_up_initialize',
    'options' => array(
      BLUEPRINT_LABEL => array(
        'description' => dt('Specify a !blueprint to use. Defaults to \'aegir\'.', array('!blueprint' => BLUEPRINT_LABEL)),
        'example-value' => 'aegir-dev',
      ),
    ),
    'examples' => array(
      'drush aui Workspace1 --' . BLUEPRINT_LABEL .'=dev' => dt('Initialize a new Aegir-up !workspace using the \'aegir-dev\' !blueprint.', array('!blueprint' => BLUEPRINT_LABEL, '!workspace'=> WORKSPACE_LABEL)),
    ),
    'aliases' => array('aui'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['aegir-up-delete'] = array(
    'description' => dt('Delete a Aegir-up !workspace.', array('!workspace'=> WORKSPACE_LABEL)),
    'arguments' => array(
      WORKSPACE_LABEL => dt('The !workspace to delete.', array('!workspace'=> WORKSPACE_LABEL)),
    ),
    'examples' => array(
      'drush aud Workspace1' => dt('Delete the Workspace1 !workspace.', array('!workspace'=> WORKSPACE_LABEL)),
    ),
    'aliases' => array('aud'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['aegir-up-ssh'] = array(
    'description' => dt('Log into a Aegir-up !VM via SSH.', array('!VM'=> VM_LABEL)),
    'arguments' => array(
      WORKSPACE_LABEL => dt('The !workspace where the !VM to log into exists.', array('!VM' => VM_LABEL, '!workspace'=> WORKSPACE_LABEL)),
      VM_LABEL => dt('The !VM to log into. Optional, if there is only one !VM in the !workspace.', array('!VM' => VM_LABEL, '!workspace'=> WORKSPACE_LABEL)),
    ),
    'examples' => array(
      'drush aus Workspace1 hm' => dt('Log into the hm !VM in the Workspace1 !workspace.', array('!VM' => VM_LABEL, '!workspace'=> WORKSPACE_LABEL)),
    ),
    'aliases' => array('aus'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['aegir-up-clone'] = array(
    'description' => dt('Clone and initialize a !workspace.', array('!workspace'=> WORKSPACE_LABEL)),
    'arguments' => array(
      'git repo' => dt('The Git repo of the !workspace definition.', array('!workspace'=> WORKSPACE_LABEL)),
      WORKSPACE_LABEL => dt('The name of the new !workspace.', array('!workspace'=> WORKSPACE_LABEL)),
    ),
    'examples' => array(
      'drush auc http://git.drupal.org/project/openatria.git openatria' => dt('Clone the git repo into a new !workspace directory (openatria).', array('!workspace'=> WORKSPACE_LABEL)),
    ),
    'aliases' => array('auc'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['aegir-up-user'] = array(
    'description' => "Initialize or update user-specific settings for Aegir-up.",
    'aliases' => array('auu'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  // Drush topic
  $items['aegir-up-docs'] = array(
    'description' => 'Aegir-up documentation.',
    'hidden' => TRUE,
    'topic' => TRUE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_print_file',
    'callback arguments' => array(AEGIR_UP_ROOT . 'aegir-up-topic.txt'),
  );

  return $items;
}

/**
 * Print a status of workspaces and VMs.
 */
function drush_aegirup_aegir_up() {
  $args = func_get_args();
  $workspace = $args[0];
  $vm = $args[1];
  drush_print($workspace);
  drush_print($vm);
}

/**
 * Initialize a new workspace.
 */
function drush_aegirup_aegir_up_initialize() {
  $args = func_get_args();
  $workspace = $args[0];
  $template = '';
  drush_print($workspace);
  if ($template = drush_get_option('template')) {
    $template = '-t ' . $template;
  }
  drush_print($template);
  drush_shell_exec_interactive(AEGIR_UP_ROOT . "aegir-up init %s %s", $template, $workspace);
}

/**
 * Delete an existing workspace.
 */
function drush_aegirup_aegir_up_delete($workspace) {
  drush_print($workspace);
  drush_shell_exec_interactive(AEGIR_UP_ROOT . "aegir-up delete %s", $workspace);
}

/**
 * Login to a VM via SSH.
 */
function drush_aegirup_aegir_up_ssh() {
  $args = func_get_args();
  $workspace = $args[0];
  $vm = $args[1];
  drush_print($workspace);
  drush_print($vm);
  drush_shell_exec_interactive(AEGIR_UP_ROOT . "aegir-up ssh %s %s", $workspace, $vm);
}

/**
 * Clone a git repo, and initilize a new workspace within it.
 */
function drush_aegirup_aegir_up_clone() {
  $args = func_get_args();
  $git_repo = $args[0];
  $workspace = $args[1];
  drush_print($git_repo);
  drush_print($workspace);
  drush_shell_exec_interactive(AEGIR_UP_ROOT . "aegir-up clone %s %s", $git_repo, $workspace);
}

/**
 * Create or update user-specific settings.
 */
function drush_aegirup_aegir_up_user() {
  drush_shell_exec_interactive(AEGIR_UP_ROOT . "aegir-up user");
}

