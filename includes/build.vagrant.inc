<?php

/**
 * Implementation of drush_hook_commandhook_validate().
 */
function drush_vagrant_build_validate($project) {

  if (!isset($project)) {
    drush_set_error(dt("ERROR: Please provide a name for your new project."));
    exit(1);
  }

  _drush_vagrant_check_user_context();

  // Check that the project name is unique and well-formed
  _drush_vagrant_validate_project($project);

  // Check that we're using a valid blueprint
  if ($blueprint = drush_get_option('blueprint')) {
    if (!array_key_exists($blueprint, drush_vagrant_blueprint_info())) {
      drush_set_error(dt("ERROR: Could not find the '!blueprint' blueprint.", array('!blueprint' => $blueprint,)));
    }
  }
}

/**
 * Initialize a new project.
 */
function drush_vagrant_build($project) {

  $user_settings = drush_get_context('user');
  if (!$user_settings) {
    $home = drush_shell_exec_output();
    include($home[0] . '/.drushrc.php');
    foreach ($options as $option => $value) {
      $user_settings[$option] = $value;
    }
  }

  if (!($blueprint = drush_get_option('blueprint'))) {
    $blueprint = 'default';
  }

  $blueprint_info = drush_vagrant_blueprint_info($blueprint);

  $git_repo = drush_get_option('git-repo');

  drush_print(dt("This script will create a new project at \'!project_root!project\'.",
    array('!project_root'  => PROJECTS_PATH,
          '!project'       => $project,)));

  if ($git_repo) {
    drush_print(dt("It will clone the Git repository at: !git-repo", array('!git-repo' => $git_repo,)));
  }
  else {
    drush_print(dt("It will use the '!blueprint' blueprint from the '!extension' extension.",
      array('!blueprint' => $blueprint,
            '!extension' => $blueprint_info['extension'],
      ))
    );
  }

  if (!drush_confirm(dt('Do you want to proceed with initializing the projectl?'))) {
    return drush_user_abort();
  }
  // Find the next available subnet
  $subnet = _drush_vagrant_next_available_subnet();

  // Create the project directory from the blueprint
  $project_path = PROJECTS_PATH . $project ;
  if ($git_repo) {
    drush_shell_exec_interactive('git clone %s %s', $git_repo, $project_path);
  }
  else {
    drush_copy_dir($blueprint_info['full_path'], $project_path);
  }

  //Check for support of 64-bit client OSes
  drush_shell_exec('egrep \'(vmx|svm)\' /proc/cpuinfo');
  $output = drush_shell_exec_output();
  if (!$output) {
    // Switch to i386 base box
    $settings_file = file_get_contents($project_path . 'settings.rb');
    $settings_file = str_replace('debian-LAMP-20', 'debian-LAMP-i386-20', $settings_file);
    $settings_file = str_replace('debian-LAMP-current.box', 'debian-LAMP-i386-current.box', $settings_file);
    file_put_contents($project_path . 'settings.rb', $settings_file);
  }


  // Set up the new project
  symlink(DRUSH_VAGRANT_ROOT . '/lib/Vagrantfile', $project_path . '/Vagrantfile');
  symlink(DRUSH_VAGRANT_ROOT . '/lib/gitignore', $project_path . '/.gitignore');

  $variables = array();
 
  $variables["global_path"] =  DRUSH_VAGRANT_ROOT . '/lib/global.rb';
  $variables["project_name"] = $project;
  $variables["vagrant_modules_path"] = DRUSH_VAGRANT_ROOT . '/lib/modules';
  $variables["subnet"] = $subnet;
  $variables["username"] = $user_settings['vagrant_username'];
  $variables["git_name"] = $user_settings['vagrant_git_name'];
  $variables["git_email"] = $user_settings['vagrant_git_email'];
  $variables["uid"] = $user_settings['vagrant_uid'];
  $variables["gid"] = $user_settings['vagrant_gid'];
 
  $config_file = _drush_vagrant_render_template($variables, $template = "config.tpl.php");
 
  $config_path = $project_path . '/.config';
  drush_mkdir($config_path);
  file_put_contents($config_path . '/config.rb', $config_file);

  //TODO: check for existence of these vars
  $dotfiles_dir = $config_path . '/files';
  drush_mkdir($dotfiles_dir);
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

}

/**
 * Post-initialize configuration
 */
function drush_vagrant_post_vagrant_build($project) {
  $no_hosts = drush_get_option('no-hosts', FALSE);
  $init_only = drush_get_option('init-only', FALSE);
  $subnet =  _drush_vagrant_get_subnet($project);

  // Add domain to hosts file
  if ($no_hosts == FALSE) {
    drush_print(dt('Enter your password to add an entry for \'!project\' to your hosts file, or press CTRL-c to leave it as is.', array( '!project' => $project)));
    $hosts_entry = sprintf('192.168.%d.10    "%s".aegir.local', $subnet, $project);
    drush_shell_exec_interactive('echo "' . $hosts_entry . '" | sudo tee -a /etc/hosts');
  }

  $project_path = PROJECTS_PATH . $project;

  drush_shell_exec_interactive("cd %s && git init && git add . && git commit -m'Initial commit.'", $project_path);

  if ($init_only == FALSE) {
    drush_shell_exec_interactive("cd %s && vagrant up", $project_path);
  }

  drush_print(dt('Your project was successfully initialized.'));
  drush_print(dt('The root for your project is \'!project_path\'', array('!project_path' => $project_path)));
  if ($no_hosts == FALSE) {
    drush_print(dt('The subnet for your project has been set to 192.168.!subnet.0, and an initial entry in your /etc/hosts file written.', array('!subnet' => $subnet)));
  }
  drush_print(dt('You can now: * Alter your project\'s behaviour by editing !project_path/settings.rb.', array('!project_path' => $project_path)));
  drush_print(dt('             * Redefine the VM(s) by editing the Puppet manifest(s) in !project_path/manifests.', array('!project_path' => $project_path)));
  drush_print(dt('             * Add additional Puppet modules by copying them to !project_path/modules.', array('!project_path' => $project_path)));

}

/**
 * Ensure that a project name is unique and well-formed
 */
function _drush_vagrant_validate_project($project) {
  if (is_dir(PROJECTS_PATH . $project)) {
    drush_set_error(dt('ERROR: There is already a project called \'!project\'.', array('!project' => $project,)));
  }
  $regex = '/^([a-z0-9][a-z0-9.-]*[a-z0-9])$/';
  if (preg_match($regex, $project) < 1) {
    drush_set_error(dt('ERROR: the name of your project (!project) should only contain lower-case letters and numbers. It can also contain hyphens (-) and dots (.), so long as they don\'t come at the beginning or end of the name.',
      array('!project' => $project,)));
  }
}

/**
 * Return the next available subnet
 */
function _drush_vagrant_next_available_subnet() {

  $initial_subnet = 10;
  $projects = _drush_vagrant_list_projects();

  $subnets = array();
  foreach ($projects as $project) {
    $subnets[$project] = _drush_vagrant_get_subnet($project);
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
