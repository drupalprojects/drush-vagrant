<?php

// Include all the commands
include_once('includes/alias.vagrant.inc');
include_once('includes/delete.vagrant.inc');
include_once('includes/init.vagrant.inc');
include_once('includes/ssh.vagrant.inc');
include_once('includes/status.vagrant.inc');
include_once('includes/user.vagrant.inc');

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
