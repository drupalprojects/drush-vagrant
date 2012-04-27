<?php

// Include all the commands
include_once('alias.vagrant.inc');
include_once('blueprints.vagrant.inc');
include_once('build.vagrant.inc');
include_once('delete.vagrant.inc');
include_once('hosts.vagrant.inc');
include_once('ssh.vagrant.inc');
include_once('status.vagrant.inc');
include_once('user.vagrant.inc');

/**
 * Ensure that there is a user context
 */
function _drush_vagrant_check_user_context() {
  $user_settings = drush_get_context('user');
  if (!isset($user_settings['vagrant_username'])) {
    drush_print('This appears to be your first time running Drush Vagrant or your user settings (~/.drushrc.php) have been deleted.');
    drush_print('Drush Vagrant Integration requires those settings in order to function properly.');
    if (drush_confirm('Proceed?')) {
      drush_shell_exec_interactive('drush vagrant-user');
      drush_shell_exec('echo $HOME');
      // PROJECTS_PATH won't have been defined in hook_drush_init() yet, so we
      // have to do it here for the first run of vagrant-initialize
      $home = drush_shell_exec_output();
      include_once($home[0] . '/.drushrc.php');
      define("PROJECTS_PATH", $options['vagrant_project_root' . '/']);
    }
    else {
      drush_user_abort();
    }
  }
}

/**
 * Ensure that a given project exists
 */
function _drush_vagrant_check_project($project) {
  if (!is_dir(PROJECTS_PATH . $project)) {
    drush_set_error(dt('ERROR: Could not find a project called \'!project\'.', array('!project' => $project,)));
  }
}

/**
 * Return an array of current projects
 */
function _drush_vagrant_list_projects(){
  $projects = array();
  if ($dir = opendir(PROJECTS_PATH)) {
    while (false !== ($project = readdir($dir))) {
      if (substr($project, 0, 1) != "." && is_dir(PROJECTS_PATH . $project)) {
        $projects[] = $project;
      }
    }
    closedir($dir);
  }
  return $projects;
}

/**
 * Return the subnet of a given project
 */
function _drush_vagrant_get_subnet($project){
  $subnet = '';
  $match = '';
  $config_file = PROJECTS_PATH . $project . '/.config/config.rb';

  // Read each line of the config file to find the line that contains the subnet
  if (is_file($config_file) && $file = @fopen($config_file, "r")) {
    while (($line = fgets($file, 4096)) !== false) {
      if (strpos($line , 'Subnet')) {
        $match = $line;
        break;
      }
    }
    if ($match == '' && !feof($file)) {
      drush_set_error(dt('ERROR: unexpected fgets() fail in _drush_vagrant_get_subnet() reading !config_file.', array('!config_file' => $config_file)));
    }
    fclose($file);
  }
  else {
    drush_set_error(dt('ERROR: Could not open !config_file in _drush_vagrant_get_subnet().', array('!config_file' => $config_file)));
  }

  // The Subnet's value is double-quoted
  $i = explode('"' , $match);
  if (is_numeric($i[1])) {
    return $i[1];
  }
  else {
    drush_set_error(dt('ERROR: Could not parse !config_file in _drush_vagrant_get_subnet().', array('!config_file' => $config_file)));
  }

}
