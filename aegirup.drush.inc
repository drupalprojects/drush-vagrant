<?php

/**
 * @file
 *   Aegir-up Example drush command.
 */

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
//    'options' => array(
//      'spreads' => array(
//        'description' => 'Comma delimited list of spreads.',
//        'example-value' => 'mayonnaise,mustard',
//      ),
//    ),
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

  // Drush topic
  $items['aegir-up-docs'] = array(
    'description' => 'Aegir-up documentation.',
    'hidden' => TRUE,
    'topic' => TRUE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_print_file',
    'callback arguments' => array(dirname(__FILE__) . '/aegir-up-topic.txt'),
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
function drush_aegirup_aegir_up_initialize($workspace) {
  drush_print($workspace);
  drush_shell_exec("./aegir-up init -y %s", $workspace);
}


