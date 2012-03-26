<?php

/**
 * @file
 *   Aegir-up family of drush commands for managing Vagrant-based VMs.
 */

define("AEGIR_UP_ROOT", dirname(__FILE__).'/');
define("WORKSPACE_ROOT", "");


/**
 * Implementation of hook_drush_help().
 */
function aegirup_drush_help($section) {
  switch ($section) {
    case 'meta:aegirup:title':
      return dt('Aegir-up commands');
    case 'meta:aegirup:summary':
      return dt('Manage Vagrant-based networks of VMs.');
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
      'workspace' => 'An (optional) workspace to present status information for.',
      'vm' => 'An (optional) VM to present status information for.',
    ),
    'examples' => array(
      'drush au Workspace1 hm' => "Print the status of the hm VM in the Workspace1 workspace.",
    ),
    'aliases' => array('au'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['aegir-up-init'] = array(
    'description' => "Initialize a new Aegir-up workspace.",
    'arguments' => array(
      'workspace' => 'An (optional) workspace to present status information for.',
    ),
    'callback' => 'drush_aegirup_aegir_up_initialize',
    'options' => array(
      'template' => array(
        'description' => "Specify a template to use. Defaults to aegir.",
        'example-value' => 'dev',
      ),
    ),
    'examples' => array(
      'drush aui Workspace1 --template=dev' => "Initialize a new Aegir-up workspace using the dev template.",
    ),
    'aliases' => array('aui'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['aegir-up-delete'] = array(
    'description' => "Delete an Aegir-up workspace.",
    'arguments' => array(
      'workspace' => 'The workspace to delete.',
    ),
    'examples' => array(
      'drush aud Workspace1' => "Delete the Workspace1 workspace.",
    ),
    'aliases' => array('aud'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['aegir-up-ssh'] = array(
    'description' => "Log into an Aegir-up VM via SSH.",
    'arguments' => array(
      'workspace' => 'The workspace where the VM to log into exists.',
      'VM' => 'The VM to log into. Optional, if there is only one VM in the workspace.',
    ),
    'examples' => array(
      'drush aus Workspace1 hm' => "Log into the hm VM in the Workspace1 workspace.",
    ),
    'aliases' => array('aus'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['aegir-up-clone'] = array(
    'description' => "Clone and initialize a workspace.",
    'arguments' => array(
      'git repo' => 'The Git repo of the workspace definition.',
      'workspace' => 'The name of the new workspace.',
    ),
    'examples' => array(
      'drush auc http://git.drupal.org/project/openatria.git openatria' => "Clone the git repo into a new workspace directory (openatria).",
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
    'callback arguments' => array(AEGIR_UP_ROOT . '/aegir-up-topic.txt'),
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

