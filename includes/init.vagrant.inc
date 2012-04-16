<?php

/**
 * Implementation of drush_hook_commandhook_validate().
 */
function drush_vagrant_initialize_validate($workspace) {

  if (!isset($workspace)) {
    drush_set_error(dt("ERROR: Please provide a name for your new !workspace_label.",
      array('!workspace_label' => WORKSPACE_LABEL,)));
    exit(1);
  }

  _drush_vagrant_check_user_context();

  // Check that the workspace name is unique and well-formed
  _drush_vagrant_validate_workspace($workspace);


  // Check that we're using a valid blueprint
  if ($blueprint = drush_get_option(BLUEPRINT_LABEL)) {
    if (!is_dir(BLUEPRINT_DIR . '/' . $blueprint)) {
      drush_set_error(dt("ERROR: Could not find the '!blueprint' !blueprint_label.",
        array('!blueprint'       => $blueprint,
              '!blueprint_label' => BLUEPRINT_LABEL,)));
    }
  }
}

/**
 * Initialize a new workspace.
 */
function drush_vagrant_initialize($workspace) {

  $user_settings = drush_get_context('user');
  if (!$user_settings) {
    $home = drush_shell_exec_output();
    include($home[0] . '/.drushrc.php');
    foreach ($options as $option => $value) {
      $user_settings[$option] = $value;
    }
  }

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

  if (!drush_confirm(dt('Do you want to proceed with initializing the !workspace_label?', array('!workspace_label' => WORKSPACE_LABEL)))) {
    return drush_user_abort();
  }
  // Find the next available subnet
  $subnet = _drush_vagrant_next_available_subnet();

  // Create the workspace directory from the blueprint
  $workspace_path = WORKSPACE_ROOT . '/' . $workspace . '/';
  if ($git_repo) {
    drush_shell_exec_interactive('git clone %s %s', $git_repo, $workspace_path);
  }
  else {
    drush_copy_dir(BLUEPRINT_DIR . '/' . $blueprint, $workspace_path);
  }

  //Check for support of 64-bit client OSes
  drush_shell_exec('egrep \'(vmx|svm)\' /proc/cpuinfo');
  $output = drush_shell_exec_output();
  if (!$output) {
    // Switch to i386 base box
    $settings_file = file_get_contents($workspace_path . 'settings.rb');
    $settings_file = str_replace('debian-LAMP-20', 'debian-LAMP-i386-20', $settings_file);
    $settings_file = str_replace('debian-LAMP-current.box', 'debian-LAMP-i386-current.box', $settings_file);
    file_put_contents($workspace_path . 'settings.rb', $settings_file);
  }


  // Set up the new workspace
  symlink(BLUEPRINT_DIR . '/' . 'Vagrantfile', $workspace_path . 'Vagrantfile');
  symlink(BLUEPRINT_DIR . '/' . 'gitignore', $workspace_path . '.gitignore');
  drush_copy_dir(BLUEPRINT_DIR . '/' . '.config', $workspace_path . '.config');
  // Make project-specific changes
  $config_file = file_get_contents($workspace_path . '.config/config.rb');
  $config_file = str_replace('"/path/to/global.rb"', '"' . DRUSH_VAGRANT_ROOT . '/lib/blueprints/global.rb"', $config_file);
  $config_file = str_replace('"subnet"', '"' . $subnet . '"', $config_file);
  $config_file = str_replace('"workspace_name"', '"' . $workspace . '"', $config_file);
  $config_file = str_replace('"vagrant_modules_path"', '"' . DRUSH_VAGRANT_ROOT . '/lib/modules"', $config_file);
  file_put_contents($workspace_path . '.config/config.rb', $config_file);

  //TODO: check for existence of these vars
  $dotfiles_dir = $workspace_path . '.config/files';
  mkdir($dotfiles_dir);
  foreach ($user_settings as $key => $dotfile_source) {
    if (strpos($key, 'dotfiles')) {
      $path = explode('/', $dotfile_source);
      $dotfile = $path[count($path) - 1];
      copy($dotfile_source, $dotfiles_dir . '/' . $dotfile);
    }
  }
  if (isset($user_settings['vagrant_ssh_keyspublic'])) {
    copy($user_settings['vagrant_ssh_keyspublic'], $dotfiles_dir . '/authorized_keys');
  }
  $config_file = file_get_contents($workspace_path . '.config/config.rb');
  $config_file = str_replace('"username"', '"' . $user_settings['vagrant_username'] . '"', $config_file);
  if (isset($user_settings['vagrant_git_name'])) {
    $config_file = str_replace('"Firstname Lastname"', '"' . $user_settings['vagrant_git_name'] . '"', $config_file);
  }
  if (isset($user_settings['vagrant_git_email'])) {
    $config_file = str_replace('"username@example.com"', '"' . $user_settings['vagrant_git_email'] . '"', $config_file);
  }
  $config_file = str_replace('"uid"', '"' . $user_settings['vagrant_uid'] . '"', $config_file);
  $config_file = str_replace('"gid"', '"' . $user_settings['vagrant_gid'] . '"', $config_file);
  file_put_contents($workspace_path . '.config/config.rb', $config_file);

}

/**
 * Post-initialize configuration
 */
function drush_vagrant_post_vagrant_initialize($workspace) {
  $no_hosts = drush_get_option('no-hosts', FALSE);
  $init_only = drush_get_option('init-only', FALSE);
  $subnet =  _drush_vagrant_get_subnet($workspace);

  // Add domain to hosts file
  if ($no_hosts == FALSE) {
    drush_print(dt('Enter your password to add an entry for \'!workspace\' to your hosts file, or press CTRL-c to leave it as is.', array( '!workspace' => $workspace)));
    $hosts_entry = sprintf('192.168.%d.10    "%s".aegir.local', $subnet, $workspace);
    drush_shell_exec_interactive('echo "' . $hosts_entry . '" | sudo tee -a /etc/hosts');
  }

  $workspace_path = WORKSPACE_ROOT . '/' . $workspace;

  drush_shell_exec_interactive("cd %s && git init && git add . && git commit -m'Initial commit.'", $workspace_path);

  if ($init_only == FALSE) {
    drush_shell_exec_interactive("cd %s && vagrant up", $workspace_path);
  }

  drush_print(dt('Your !workspace was successfully initialized.', array('!workspace' => WORKSPACE_LABEL)));
  drush_print(dt('The root for your !workspace is !workspace_path', array('!workspace' => WORKSPACE_LABEL, '!workspace_path' => $workspace_path)));
  if ($no_hosts == FALSE) {
    drush_print(dt('The subnet for your !workspace has been set to 192.168.!subnet.0, and an initial entry in your /etc/hosts file written.', array('!workspace' => WORKSPACE_LABEL, '!subnet' => $subnet)));
  }
  drush_print(dt('You can now: * Alter Aegir-up\'s behaviour by editing !workspace_path/settings.rb.', array('!workspace_path' => $workspace_path)));
  drush_print(dt('             * Redefine the !VM(s) by editing the Puppet manifest(s) in !workspace_path/manifests.', array('!VM' => VM_LABEL, '!workspace_path' => $workspace_path)));
  drush_print(dt('             * Add additional Puppet modules by copying them to !workspace_path/modules.', array('!workspace_path' => $workspace_path)));

}

/**
 * Ensure that a workspace name is unique and well-formed
 */
function _drush_vagrant_validate_workspace($workspace) {
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
function _drush_vagrant_next_available_subnet() {

  $initial_subnet = 10;
  $workspaces = _drush_vagrant_list_workspaces();

  $subnets = array();
  foreach ($workspaces as $workspace) {
    $subnets[$workspace] = _drush_vagrant_get_subnet($workspace);
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
