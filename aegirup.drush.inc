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

  // Drush topic
  $items['aegir-up-docs'] = array(
    'description' => 'Aegir-up README.',
    'hidden' => TRUE,
    'topic' => TRUE,
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'callback' => 'drush_print_file',
    'callback arguments' => array(AEGIR_UP_ROOT . 'README.md'),
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

/**
 * Implementation of drush_hook_commandhook_validate().
 */
function drush_aegirup_aegir_up_initialize_validate($workspace) {

  if (!isset($workspace)) {
    drush_set_error(dt("ERROR: Please provide a name for your new !workspace_label.",
      array('!workspace_label' => WORKSPACE_LABEL,)));
    exit(1);
  }

  _aegir_up_check_user_context();

  // Check that the workspace name is unique and well-formed
  _aegir_up_validate_workspace($workspace);


  // Check that we're using a valid blueprint
  if ($blueprint = drush_get_option(BLUEPRINT_LABEL)) {
    if (!is_dir(BLUEPRINT_DIR . $blueprint)) {
      drush_set_error(dt("ERROR: Could not find the '!blueprint' !blueprint_label.",
        array('!blueprint'       => $blueprint,
              '!blueprint_label' => BLUEPRINT_LABEL,)));
    }
  }
}

/**
 * Initialize a new workspace.
 */
function drush_aegirup_aegir_up_initialize($workspace) {

  $user_settings = drush_get_context('user');
  if (!$user_settings) {
    $home = drush_shell_exec_output();
    _debug('$home', $home);
    include($home[0] . '/.drushrc.php');
    _debug('$options', $options);
    foreach ($options as $option => $value) {
      $user_settings[$option] = $value;
    }
  }
  _debug('$user_settings', $user_settings);

  if (!($blueprint = drush_get_option(BLUEPRINT_LABEL))) {
    $blueprint = 'dev-aegir';
  }

  $git_repo = drush_get_option('git-repo');

  drush_print(dt("This script will create a new !workspace_label at !workspace_root!workspace.",
    array('!workspace_root'  => WORKSPACE_ROOT,
          '!workspace'       => $workspace,
          '!workspace_label' => WORKSPACE_LABEL,)));

  if ($git_repo) {
    drush_print(dt("It will clone the Git repository at: !git-repo", array('!git-repo' => $git_repo,)));
  }
  else {
    drush_print(dt("It will use the '!blueprint' !blueprint_label.",
      array('!blueprint' => $blueprint,
            '!blueprint_label' => BLUEPRINT_LABEL,
      ))
    );
  }
  _debug('Subnet: ', _aegir_up_next_available_subnet());

  if (!drush_confirm(dt('Do you want to proceed with initializing the !workspace_label?', array('!workspace_label' => WORKSPACE_LABEL)))) {
    return drush_user_abort();
  }
  // Find the next available subnet
  $subnet = _aegir_up_next_available_subnet();

  // Create the workspace directory from the blueprint
  $workspace_path = WORKSPACE_ROOT . $workspace . '/';
  if ($git_repo) {
    drush_shell_exec_interactive('git clone %s %s', $git_repo, $workspace_path);
  }
  else {
    drush_copy_dir(BLUEPRINT_DIR . $blueprint, $workspace_path);
  }

  // Set up the new workspace
  symlink(BLUEPRINT_DIR . 'Vagrantfile', $workspace_path . 'Vagrantfile');
  symlink(BLUEPRINT_DIR . 'gitignore', $workspace_path . '.gitignore');
  drush_copy_dir(BLUEPRINT_DIR . '.config', $workspace_path . '.config');
  // Make project-specific changes
  $config_file = file_get_contents($workspace_path . '.config/config.rb');
  $config_file = str_replace('"10"', '"' . $subnet . '"', $config_file);
  $config_file = str_replace('"hm"', '"' . $workspace . '"', $config_file);
  $config_file = str_replace('"aegirup_modules_path"', '"' . AEGIR_UP_ROOT . 'lib/modules"', $config_file);
  file_put_contents($workspace_path . '.config/config.rb', $config_file);

  //TODO: check for existence of these vars
  $dotfiles_dir = $workspace_path . '.config/files';
  _debug('$dotfiles_dir', $dotfiles_dir);
  mkdir($dotfiles_dir);
  foreach ($user_settings as $key => $dotfile_source) {
    if (strpos($key, 'dotfiles')) {
      $path = explode('/', $dotfile_source);
      $dotfile = $path[count($path) - 1];
      _debug('$dotfile', $dotfile);
      _debug('', $dotfile_source);
      copy($dotfile_source, $dotfiles_dir . '/' . $dotfile);
    }
  }
  if (isset($user_settings['aegirup_ssh_keyspublic'])) {
    copy($user_settings['aegirup_ssh_keyspublic'], $dotfiles_dir . '/authorized_keys');
  }
  $config_file = file_get_contents($workspace_path . '.config/config.rb');
  $config_file = str_replace('"username"', '"' . $user_settings['aegirup_username'] . '"', $config_file);
  if (isset($user_settings['aegirup_git_name'])) {
    $config_file = str_replace('"Firstname Lastname"', '"' . $user_settings['aegirup_git_name'] . '"', $config_file);
  }
  if (isset($user_settings['aegirup_git_email'])) {
    $config_file = str_replace('"username@example.com"', '"' . $user_settings['aegirup_git_email'] . '"', $config_file);
  }
  $config_file = str_replace('"uid"', '"' . $user_settings['aegirup_uid'] . '"', $config_file);
  $config_file = str_replace('"gid"', '"' . $user_settings['aegirup_gid'] . '"', $config_file);
  file_put_contents($workspace_path . '.config/config.rb', $config_file);

}

/**
 * Post-initialize configuration
 */
function drush_aegirup_post_aegir_up_initialize($workspace) {
  $no_hosts = drush_get_option('no-hosts', FALSE);
  $init_only = drush_get_option('init-only', FALSE);
//  $no_browser = drush_get_option('no-browser', $init_only);

  // Add domain to hosts file
  if ($no_hosts == FALSE) {
    drush_print(dt('Enter your password to add an entry for \'!workspace\' to your hosts file, or press CTRL-c to leave it as is.', array( '!workspace' => $workspace)));
    $hosts_entry = sprintf('192.168.%d.10    "%s".aegir.local',_aegir_up_get_subnet($workspace), $workspace);
    drush_shell_exec_interactive('echo "' . $hosts_entry . '" | sudo tee -a /etc/hosts');
  }

  $workspace_path = WORKSPACE_ROOT . $workspace;

  if ($init_only == FALSE) {
    drush_shell_exec_interactive("cd %s && vagrant up", $workspace_path);
  }

  drush_shell_exec_interactive("cd %s && git init && git add . && git commit -m'Initial commit.'", $workspace_path);

  drush_print(dt('Your !workspace was successfully initialized.', array('!workspace' => WORKSPACE_LABEL)));
  drush_print(dt('The root for your !workspace is !workspace_path', array('!workspace' => WORKSPACE_LABEL, '!workspace_path' => $workspace_path)));
  if ($no_hosts == FALSE) {
    drush_print(dt('The subnet for your !workspace has been set to 192.168.!subnet.0, and an initial entry in your /etc/hosts file written.', array('!workspace' => WORKSPACE_LABEL, '!subnet' => $subnet)));
  }
  drush_print(dt('You can now: * Alter Aegir-up\'s behaviour by editing !workspace_path/settings.rb.', array('!workspace_path' => $workspace_path)));
  drush_print(dt('             * Redefine the !VM(s) by editing the Puppet manifest(s) in !workspace_path/manifests.', array('!VM' => VM_LABEL, '!workspace_path' => $workspace_path)));
  drush_print(dt('             * Add additional Puppet modules by copying them to !workspace_path/modules.', array('!workspace_path' => $workspace_path)));

  // * Build platforms in the front-end based on makefiles added to .../makefiles."
  // * Have platforms built outside the VM by mounting /var/aegir under NFS."

//  if ($no_browser == FALSE) {}
}

/**
 * Delete an existing workspace.
 */
function drush_aegirup_aegir_up_delete($workspace) {
  _debug('$workspace', $workspace);

  _aegir_up_check_workspace($workspace);

  $subnet = _aegir_up_get_subnet($workspace);
  _debug('$subnet', $subnet);

  drush_shell_exec('grep 192.168.%s.10 /etc/hosts', $subnet);
  $remove_lines = drush_shell_exec_output();
  _debug('$remove_lines', $remove_lines);

  drush_print(dt('!!! WARNING: This operation cannot be undone. !!!'));
  drush_print(dt('This operation will delete the \'!workspace\' !workspace_label, and destroy all of its !VM_label(s)',
    array('!workspace'       => $workspace,
          '!workspace_label' => WORKSPACE_LABEL,
          '!VM_label'        => VM_LABEL,)));

  foreach ($remove_lines as $line) {
    $remove_line_block .= '   ' . $line . "\n";
  }

  drush_print(dt("It will also remove the following line(s) from your /etc/hosts file:\n!remove_line",
    array('!remove_line' => $remove_line_block,)));

  if (drush_confirm(dt('Are you certain that you want to proceed?'))) {
    foreach ($remove_lines as $remove_line) {
      drush_shell_exec_interactive('sudo sed /%s/d -i /etc/hosts', $remove_line);
    }
    drush_shell_exec('cd %s && vagrant destroy --force', WORKSPACE_ROOT . '/' . $workspace);
    if (!is_file(WORKSPACE_ROOT . '/' . $workspace . '/.vagrant')) {
      drush_shell_exec('rm -rf %s', WORKSPACE_ROOT . '/' . $workspace);
    }
    else {
      drush_set_error(dt('Failure during \'vagrant destroy\''), 'error');
    }
  }
  else {
    drush_user_abort();
  }
}

/**
 * Login to a VM via SSH.
 */
function drush_aegirup_aegir_up_ssh() {

  _aegir_up_check_user_context();
  $user_settings = drush_get_context('user');
  _debug('$user_settings', $user_settings);

  $args = func_get_args();
  if (!($workspace = $args[0])) {
    drush_set_error(dt('ERROR: aegir-up-ssh requires a !workspace_label as an argument', array('!workspace_label' => WORKSPACE_LABEL)));
    drush_print(dt('You can initialize a new !workspace_label by running \'drush aegir-up-initialize [<!workspace_label>]\'', array('!workspace_label' => WORKSPACE_LABEL)));
    drush_print(dt('Currently available !workspace_labels include:', array('!workspace_label' => WORKSPACE_LABEL)));
    drush_shell_exec_interactive('drush aegir-up');
    exit(1);
  }
  _debug('$workspace', $workspace);
  $vm = $args[1];
  _debug('$vm', $vm);
  $workspace_path = WORKSPACE_ROOT . $workspace;
  _debug('$workspace_path', $workspace_path);

  _aegir_up_check_workspace($workspace);

  if ($vm) {
    drush_shell_exec("cd %s && vagrant ssh-config %s", $workspace_path, $vm);
  }
  else {
    drush_shell_exec("cd %s && vagrant ssh-config", $workspace_path);
  }
  $ssh_config = drush_shell_exec_output();
  _debug('$ssh_config', $ssh_config);

  $ssh_hostname  = substr($ssh_config[1], 11);
  _debug('$ssh_hostname', $ssh_hostname);
  $ssh_port = substr($ssh_config[3], 7);
  _debug('$ssh_port', $ssh_port);

  drush_shell_exec_interactive('cd %s && vagrant up', WORKSPACE_ROOT . '/' . $workspace);

  drush_shell_exec_interactive('ssh %s -l %s -i %s -p %s', $ssh_hostname, $user_settings['aegirup_username'], $user_settings['aegirup_ssh_keysprivate'], $ssh_port);

}

/**
 * Create or update user-specific settings.
 */
function drush_aegirup_aegir_up_user() {

  $user_settings = array();
  $data = array();

  // Environment variables
  // TODO: allow other env vars to be passed through an option or hook?
  $data['aegirup_username'] = array( 'command' => 'whoami', 'label' => 'username');
  $data['aegirup_home'] = array( 'command' => 'echo $HOME', 'label' => 'home folder');
  $data['aegirup_uid'] = array( 'command' => 'id -u', 'label' => 'user id');
  $data['aegirup_gid'] = array( 'command' => 'id -g', 'label' => 'group id');
  $data['aegirup_workspace_root'] = array( 'command' => 'echo $HOME/aegir-up/', 'label' => 'Workspace folder');
  $data['aegirup_git_name'] = array( 'command' => 'git config --global --get user.name', 'label' => 'Git username');
  $data['aegirup_git_email'] = array( 'command' => 'git config --global --get user.email', 'label' => 'Git email');
  // Get env var defaults
  foreach ($data as $key => $datum) {
    drush_shell_exec($datum['command']);
    $default = drush_shell_exec_output();
    $data[$key]['default'] = $default[0];
  }

  // Files
  // TODO: allow other files & filesets to be passed through an option or hook?
  $file_sets = array('dotfiles', 'ssh_keys' );
  $dotfiles = array('.profile', '.bashrc', '.bash_aliases', '.vimrc', );
  $ssh_keys = array('public' => '.ssh/id_rsa.pub', 'private' => '.ssh/id_rsa');
  // Get file defaults
  foreach ($file_sets as $set) {
    foreach ($$set as $key => $file) {
      if (is_numeric($key)) {
        $name = $file;
      }
      else {
        $name = $key;
      }
      if (file_exists($data['aegirup_home']['default'] . '/' . $file)) {
        $data['aegirup_' . $set . $name] = array('default' => $data['aegirup_home']['default'] . '/' . $file, 'label' => $name . '(' . $set .')');
      }
    }
  }

  // find the longest label so we can align values
  $i = 0;
  foreach ($data as $datum) {
    if (strlen($datum['label']) > $i) {
      $i = strlen($datum['label']);
    }
  }

  // Build our confirmation prompt
  $defaults = "The following settings will be added to your ~/.drushrc.php:\n";
  foreach ($data as $key => $datum) {
    $defaults .= '  ' . $datum['label'] . ' ' . str_repeat('.', $i + 1 - strlen($datum['label'])) . ' ' . $datum['default'] . "\n";
  }
  $defaults .= 'Are these settings correct?';
  $correct = drush_confirm($defaults);

  // Build our user context
  if ($correct) {
    foreach ($data as $key => $datum) {
      $user_settings[$key] = $datum['default'];
    }
  }
  // Or prompt for everything
  else {
    $prompt = 'What !label would you like to use? ';
    foreach ($data as $key => $datum) {
      $user_settings[$key] = drush_prompt(dt($prompt, array( '!label' => $datum['label'])), $datum['default']);
    }
  }

  if (!is_dir($user_settings['aegirup_workspace_root'])) {
    mkdir($user_settings['aegirup_workspace_root']);
  }

  _debug("WORKSPACE_ROOT", WORKSPACE_ROOT);

  drush_set_context('user', $user_settings);
  drush_save_config('user');
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
 * Ensure that a workspace name is unique and well-formed
 */
function _aegir_up_validate_workspace($workspace) {
  if (is_dir(WORKSPACE_ROOT . $workspace)) {
    drush_set_error(dt('ERROR: There is already a !workspace_label called !workspace.',
      array('!workspace_label' => WORKSPACE_LABEL,
            '!workspace'       => $workspace,)));
  }
  $regex = '/^([a-z0-9][a-z0-9.-]*[a-z0-9])$/';
  if (preg_match($regex, $workspace) < 1) {
    drush_set_error(dt('ERROR: the name of your !workspace_label (!workspace) should only contains lower-case letters and numbers. It can also contain hyphens (-) and dots (.), so long as they don\'t come at the beginning or end of the name.',
      array('!workspace_label' => WORKSPACE_LABEL,
            '!workspace'       => $workspace,)));
  }
}

/**
 * Return the next available subnet
 */
function _aegir_up_next_available_subnet() {

  $initial_subnet = 10;
  $workspaces = _aegir_up_list_workspaces();

  _debug('Workspaces: ' . count($workspaces), $workspaces);

  $subnets = array();
  foreach ($workspaces as $workspace) {
    $subnets[$workspace] = _aegir_up_get_subnet($workspace);
  }

  // If there aren't any projects yet, use the default
  if (!(count($subnets) > 0)) {
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
    drush_log('Function: ' . debug_backtrace(false)[1]['function'] . '()', 'debug');
    drush_log($label . ': ' . print_r($data, true), 'debug');
  }
}
