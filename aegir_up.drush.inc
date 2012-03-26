<?php

/**
 * @file
 *   Aegir-up Example drush command.
 */

/**
 * Implementation of hook_drush_command().
 */
function aegir_up_drush_command() {
  $items = array();

  $items['aegir-up'] = array(
    'description' => "Run your Aegir-up system.",
    'arguments' => array(
      'sub-command' => 'The sub-command to have Aegir-up run. Defaults to \'...\'.',
      'workspace' => 'The workspace on which to act.',
      'vm' => 'The VM on which to act.',
    ),
    'options' => array(
      'spreads' => array(
        'description' => 'Comma delimited list of spreads.',
        'example-value' => 'mayonnaise,mustard',
      ),
    ),
    'examples' => array(
      'drush au init Workspace1 --template=dev' => 'Build a new Aegir-up Workspace using the \'dev\' template.',
    ),
    'aliases' => array('au'),
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
