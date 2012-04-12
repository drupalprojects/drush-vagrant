<?php

/**
 * @file
 *   Aegir-up family of drush commands for managing Vagrant-based VMs.
 */

define("WORKSPACE_LABEL", "workspace");
define("BLUEPRINT_LABEL", "blueprint");
define("VM_LABEL", "VM");
define("AEGIR_UP_ROOT", dirname(__FILE__).'/');
define("BLUEPRINT_DIR", AEGIR_UP_ROOT . "lib/" . BLUEPRINT_LABEL . "s/");

///////////////////////////////////////////////////////////////////////////////
//                        HOOK IMPLEMENTATIONS                               //
///////////////////////////////////////////////////////////////////////////////

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
 * Implementation of hook_drush_init().
 */
function aegirup_drush_init() {
  $user_settings = drush_get_context('user');
  if (isset($user_settings['aegirup_workspace_root'])) {
    define("WORKSPACE_ROOT", $user_settings['aegirup_workspace_root']);
  }
  _debug("WORKSPACE_ROOT", WORKSPACE_ROOT);
}

/**
 * Implementation of hook_drush_command().
 */
function aegirup_drush_command() {
  $items = array();

  $items['aegir-up'] = array(
    'description' => dt('List current !workspace_labels.', array('!workspace_label' => WORKSPACE_LABEL)),
//    'arguments' => array(
//      WORKSPACE_LABEL => dt('An (optional) !workspace to present status information for.', array('!workspace'=> WORKSPACE_LABEL)),
//      VM_LABEL => dt('An (optional) !VM to present status information for.', array('!VM' => VM_LABEL)),
//    ),
//    'examples' => array(
//      'drush au Workspace1 hm' => dt('Print the status of the hm !VM in the Workspace1 !workspace.', array('!VM' => VM_LABEL, '!workspace'=> WORKSPACE_LABEL)),
//    ),
    'aliases' => array('au'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['aegir-up-initialize'] = array(
    'description' => dt('Initialize a new Aegir-up !workspace.', array('!workspace'=> WORKSPACE_LABEL)),
    'arguments' => array(
      WORKSPACE_LABEL => dt('The !workspace to initialize.', array('!workspace'=> WORKSPACE_LABEL)),
    ),
    'options' => array(
      BLUEPRINT_LABEL => array(
        'description' => dt('Specify a !blueprint to use. Defaults to \'dev-aegir\'.', array('!blueprint' => BLUEPRINT_LABEL))),
      'git-repo' => array(
        'description' => dt('Clone a Git repo, instead of using a !blueprint.', array('!blueprint' => BLUEPRINT_LABEL))),
//      'no-browser' => array(
//        'description' => dt('Don\'t launch a browser')),
      'no-hosts' => array(
        'description' => dt('Don\'t edit /etc/hosts.')),
      'init-only' => array(
        'description' => dt('Don\'t start the !VM.', array('!VM' => VM_LABEL))),
    ),
    'examples' => array(
      'drush aui Workspace1 --' . BLUEPRINT_LABEL .'=dev-aegir' => dt('Initialize a new Aegir-up !workspace using the \'dev-aegir\' !blueprint.', array('!blueprint' => BLUEPRINT_LABEL, '!workspace'=> WORKSPACE_LABEL)),
    ),
    'aliases' => array('aui'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['aegir-up-delete'] = array(
    'description' => dt('Delete an Aegir-up !workspace.', array('!workspace'=> WORKSPACE_LABEL)),
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

  $items['aegir-up-user'] = array(
    'description' => "Initialize or update user-specific settings for Aegir-up.",
    'aliases' => array('auu'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  // Drush topics
  $items['aegir-up-readme'] = array(
    'description' => 'Aegir-up README.',
    'hidden' => TRUE,
    'topic' => TRUE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_print_file',
    'callback arguments' => array(AEGIR_UP_ROOT . 'README.md'),
  );
  $items['aegir-up-architecture'] = array(
    'description' => 'Aegir-up architecture documentation.',
    'hidden' => TRUE,
    'topic' => TRUE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_print_file',
    'callback arguments' => array(AEGIR_UP_ROOT . 'docs/architecture.md'),
  );
  $items['aegir-up-blueprints'] = array(
    'description' => 'Aegir-up blueprints documentation.',
    'hidden' => TRUE,
    'topic' => TRUE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_print_file',
    'callback arguments' => array(AEGIR_UP_ROOT . 'docs/blueprints.md'),
  );
   $items['aegir-up-workspaces'] = array(
    'description' => 'Aegir-up workspaces documentation.',
    'hidden' => TRUE,
    'topic' => TRUE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_print_file',
    'callback arguments' => array(AEGIR_UP_ROOT . 'docs/workspaces.md'),
  );

  return $items;
}

/**
 * Implementation of hook_drush_pm_download_destination_alter().
 *
 * For blueprints hosted on d.o, make 'drush dl' put them in the correct folder.
 */
function aegirup_drush_pm_download_destination_alter(&$project, $release) {
  $settings_rb = drush_scan_directory($project['full_project_path'], '/settings.rb/', array('.', '..'), $recurse_max_depth = 1);
  if (!empty($settings_rb)) {
    $project['project_install_location'] = BLUEPRINT_DIR;
  }
}


///////////////////////////////////////////////////////////////////////////////
//                              COMMAND FUNCTIONS                            //
///////////////////////////////////////////////////////////////////////////////

/**
 * Print a status of workspaces and VMs.
 */
function drush_aegirup_aegir_up() {
  $args = func_get_args();
  $workspace = $args[0];
  _debug('$workspace', $workspace);
  $vm = $args[1];
  _debug('$vm', $vm);

  $workspaces = _aegir_up_list_workspaces();
  _debug('$workspaces', $workspaces);

  foreach ($workspaces as $entry) {
    drush_print($entry);
  }
}


////////////////////////////////////////////////////////////////////////////////
//                            HELPER FUNCTIONS                                //
////////////////////////////////////////////////////////////////////////////////

/**
 * Ensure that there is a user context
 */
function _aegir_up_check_user_context() {
  $user_settings = drush_get_context('user');
  if (!isset($user_settings['aegirup_username'])) {
    drush_print('This appears to be your first time running Aegir-up or your user settings (~/.drushrc.php) have been deleted.');
    drush_print('Aegir-up requires those settings in order to function properly.');
    if (drush_confirm('Proceed?')) {
      drush_shell_exec_interactive('drush aegir-up-user');
      drush_shell_exec('echo $HOME');
      // WORKSPACE_ROOT won't have been defined in hook_drush_init() yet, so we
      // have to do it here for the first run of aegir-up-initialize
      $home = drush_shell_exec_output();
      _debug('$home', $home);
      include_once($home[0] . '/.drushrc.php');
      define("WORKSPACE_ROOT", $options['aegirup_workspace_root']);
      _debug("WORKSPACE_ROOT", WORKSPACE_ROOT);
    }
    else {
      drush_user_abort();
    }
  }
}

/**
 * Ensure that a given workspace exists
 */
function _aegir_up_check_workspace($workspace) {
  if (!is_dir(WORKSPACE_ROOT . $workspace)) {
    drush_set_error(dt('ERROR: Could not find a !workspace_label called !workspace.',
      array('!workspace_label' => WORKSPACE_LABEL,
            '!workspace'       => $workspace,)));
  }
}

/**
 * Return an array of current workspaces
 */
function _aegir_up_list_workspaces(){
  $workspaces = array();
  // See: http://php.net/manual/en/function.readdir.php
  if ($handle = opendir(WORKSPACE_ROOT)) {
    _debug('WORKSPACE_ROOT', WORKSPACE_ROOT);

    while (false !== ($file = readdir($handle))) {
      _debug('$file', $file);
      if (substr($file, 0, 1) != "." && is_dir(WORKSPACE_ROOT . $file)) {
        _debug('Directory', $file);
        $workspaces[] = $file;
      }
    }
    closedir($handle);
  }
  return $workspaces;
}

/**
 * Return the subnet of a given workspace
 */
function _aegir_up_get_subnet($workspace){
  $subnet = '';
  $match = '';
  $config_file = WORKSPACE_ROOT . $workspace . '/.config/config.rb';

  // Read each line of the config file to find the line that contains the subnet
  if (is_file($config_file) && $file = @fopen($config_file, "r")) {
    while (($line = fgets($file, 4096)) !== false) {
      if (strpos($line , 'Subnet')) {
        $match = $line;
        break;
      }
    }
    if ($match == '' && !feof($file)) {
      drush_set_error(dt('ERROR: unexpected fgets() fail in _aegir_up_get_subnet() reading !config_file.', array('!config_file' => $config_file)));
    }
    fclose($file);
  }
  else {
    drush_set_error(dt('ERROR: Could not open !config_file in _aegir_up_get_subnet().', array('!config_file' => $config_file)));
  }

  // The Subnet's value is double-quoted
  $i = explode('"' , $match);
  if (is_numeric($i[1])) {
    return $i[1];
  }
  else {
    drush_set_error(dt('ERROR: Could not parse !config_file in _aegir_up_get_subnet().', array('!config_file' => $config_file)));
  }

}

/**
 * Write variables to the Drush log for debugging purposes
 */
function _debug($label, $data = null) {
  if (drush_get_option('debug')) {
    $backtrace = debug_backtrace(false);
    drush_log('Function: ' . $backtrace[1]['function'] . '()', 'debug');
    drush_log($label . ': ' . print_r($data, true), 'debug');
  }
}
