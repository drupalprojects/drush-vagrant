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
  if (!($blueprint = drush_get_option(BLUEPRINT_LABEL))) {
    $blueprint = 'default';
  }

  drush_print(dt("This script will create a new !workspace_label at !workspace_root!workspace.",
    array('!workspace_root'  => WORKSPACE_ROOT,
          '!workspace'       => $workspace,
          '!workspace_label' => WORKSPACE_LABEL,
    ))
  );
  drush_print(dt("It will use the '!blueprint' !blueprint_label.",
    array('!blueprint' => $blueprint,
          '!blueprint_label' => BLUEPRINT_LABEL,
    ))
  );

  // 
  $subnet = _aegir_up_next_available_subnet();


  drush_print($subnet);
  drush_print($blueprint);
  drush_print($workspace);
  drush_shell_exec_interactive(AEGIR_UP_ROOT . "aegir-up init -t %s %s", $blueprint, $workspace);
}

/**
 * Implementation of drush_hook_commandhook_validate().
 */
function drush_aegirup_aegir_up_initialize_validate() {
  $args = func_get_args();
  $workspace = $args[0];

  // Check that we're using a valid blueprint
  if ($blueprint = drush_get_option(BLUEPRINT_LABEL)) {
    if (!is_dir(BLUEPRINT_DIR . $blueprint)) {
      drush_set_error(dt("ERROR: Could not find the '!blueprint' !blueprint_label.",
        array('!blueprint'       => $blueprint,
              '!blueprint_label' => BLUEPRINT_LABEL,
        ))
      );
    }
  }

  // Check that the workspace name is unique and well-formed
  _aegir_up_validate_workspace($workspace);

}

/**
 * Ensure that a workspace name is unique and well-formed
 */
function _aegir_up_validate_workspace($workspace) {
  if (is_dir(WORKSPACE_ROOT . $workspace)) {
    drush_set_error(dt('ERROR: There is already a !workspace_label called !workspace.',
      array('!workspace_label' => WORKSPACE_LABEL,
            '!workspace'       => $workspace,
      ))
    );
  }
  $regex = '/^([a-z0-9][a-z0-9.-]*[a-z0-9])$/';
  if (preg_match($regex, $workspace) < 1) {
    drush_set_error(dt('ERROR: the name of your !workspace_label (!workspace) should only contains lower-case letters and numbers. It can also contain hyphens (-) and dots (.), so long as they don\'t come at the beginning or end of the name.',
      array('!workspace_label' => WORKSPACE_LABEL,
            '!workspace'       => $workspace,
      ))
    );
  }
}

/**
 * Return the next available subnet
 */
function _aegir_up_next_available_subnet() {

  $initial_subnet = 10;
  $workspaces = _aegir_up_list_workspaces();
  $subnets = array();
  foreach ($workspaces as $workspace) {
    $subnets[$workspace] = _aegir_up_get_subnet($workspace);
  }

  // If there aren't any projects yet, use the default  
  if (count($subnets) > 0) {
    return $initial_subnet;
  }

  // Find the first unused subnet
  $subnets = array_flip($subnets);
  for ( $key = $initial_subnet; $key <= 254; $key++) {
    if (!array_key_exists($key, $subnets)) {
      return $key;
    }
  }
  
  // If we've reached here, then no subnets are available
  drush_set_error(dt('ERROR: We\'ve run out of Subnets!'));
}

/**
 * Return an array of current workspaces
 */
function _aegir_up_list_workspaces(){
  $workspaces = array();
  // See: http://php.net/manual/en/function.readdir.php
  if ($handle = opendir(WORKSPACE_ROOT)) {
    while (false !== ($entry = readdir($handle))) {
      if ($entry != "." && $entry != ".." && is_dir($entry)) {
        $workspaces[] = $entry;
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
  if ($file = @fopen($config_file, "r")) {
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

