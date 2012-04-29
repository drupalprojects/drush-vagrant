<?php

// Include all the commands
$includes = scandir(dirname(__FILE__));
foreach ($includes as $key => $file) {
  if (@strpos($file, '.vagrant.inc', strlen($file) - 12) !== FALSE) {
    include_once($file);
  }
}

/**
 * Generate help text
 */
function _drush_vagrant_help_text() {
  $output[] = dt("Run a command on your Vagrant projects or VMs.\n");
  $output[] = dt("Usage: !usage\n", array('!usage' => 'drush [<@alias>] vagrant <sub-command> [<arguments>] [<options>]'));
  
  $output[] = dt("Available native Vagrant commands:");
  drush_shell_exec('vagrant');
  $sub_commands = drush_shell_exec_output();
  $sub_commands = array_splice($sub_commands, 6, -2);
  $output[] = _drush_vagrant_help_table($sub_commands);
  $blueprints = drush_vagrant_blueprint_info();
  foreach ($blueprints as $blueprint => $info) {
    $extensions[] = $info['extension'];
  }
  $drush_commands = drush_get_context('DRUSH_COMMANDS');
  $commands = array();
  foreach ($drush_commands as $command => $info) {
    if (in_array($info['commandfile'], $extensions)
        && $info['topic'] != 1
        && $info['hidden'] != 1
        && $info['is_alias'] != 1
        && $command != 'vagrant') {
      $commands[] = '     ' . substr($command, strlen('vagrant-'));
    }
  }
  if (count($commands)) {
    $output[] = dt("Commands provided by Drush Vagrant:");
    $output[] = _drush_vagrant_help_table($commands, 2);
  }

  $output[] = dt("For additional help on native Vagrant sub-commands try: 'vagrant <sub-command> --help'");
  $output[] = dt("For additional help on Drush Vagrant sub-commands try 'drush vagrant-<sub-command> --help'");

  $output = implode("\n", $output);
  return $output;
}

function _drush_vagrant_help_table($list, $cols = 3) {
  sort($list, SORT_STRING);
  $rows = ceil(count($list)/$cols);
  $i = 0;
  for ($col = 0; $col < $cols; $col++) {
    for ($row = 0; $row < $rows; $row++) {
      $table[$row][$col] = $list[$i];
      $i++;
    }
  }
  $tmp_file = drush_save_data_to_temp_file('');
  $handle = fopen($tmp_file, 'w+b');
  drush_print_table($table, FALSE, NULL, $tmp_file);
  return fread($handle, 500);
}

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
