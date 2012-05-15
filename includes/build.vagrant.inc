<?php

/**
 * Implementation of drush_hook_commandhook_validate().
 */
function drush_vagrant_build_validate() {

  // Ensure we've gathered user info
  _drush_vagrant_check_user_context();

  // Check that the project name is unique and well-formed
  if ($project = drush_get_option('project-name')) {
    _drush_vagrant_check_project_name($project, TRUE);
  }

  // Check that the Git repo is accessible
  if ($git_repo = drush_get_option('git-repo')) {
    _drush_vagrant_check_git_repo($git_repo, TRUE);
  }

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
function drush_vagrant_build() {

  $user_settings = drush_get_context('user');

  // Prompt for a project name if one isn't already set.
  if (!($project_name = drush_get_option('project-name'))) {
    drush_print(dt('The name of your project may be used in URLs, and so should only contain lower-case letters and numbers. It can also contain hyphens (-) and dots (.), so long as they do not come at the beginning or end of the name.'));
    // Only accept a valid project name.
    while ($error_msg !== TRUE) {
      $project_name = drush_prompt(dt('What would you like to call your project?'));
      $error_msg = _drush_vagrant_check_project_name($project_name);
      if ($error_msg !== TRUE) {
        drush_print($error_msg);
      }
    }
  }

  // Prompt for a Git repository if one isn't already set.
  if (!($git_repo = drush_get_option('git-repo'))) {
    drush_print(dt('You can build your project based on a Git repository, or a blueprint. Provide the URL of a Git repository, or leave blank to choose a template.'));
    // Only accept a valid git repo, or pass through to blueprint selection.
    do {
      $git_repo = drush_prompt(dt('Git URL'), 'none');
    } while ($git_repo != 'none' && !_drush_vagrant_check_git_repo($git_repo));
  }
  
  if (!($blueprint = drush_get_option('blueprint')) && $git_repo == 'none') {
    unset($git_repo);
    $blueprint_info = drush_vagrant_blueprint_info();
    foreach ($blueprint_info as $name => $data) {
      $choice[$name] = $data['name'] . ': ' . $data['description'] ;
    }
    $prompt = dt('Which blueprint would you like to use?');
    $blueprint = drush_choice($choice, $prompt);
    // User chose 'cancel'
    if ($blueprint == '') {
      return drush_user_abort();
    }
  }

  $hosts = drush_get_option('hosts', FALSE);
  if ($hosts === FALSE) {
    $hosts = drush_confirm(dt('Would you like to generate entries in /etc/hosts for the VMs in your project?'));
    if ($hosts !== FALSE) {
      // Save the new option
      drush_set_option('hosts', TRUE);
      // --hosts assumes --aliases
      drush_set_option('aliases', TRUE);
    }
  }
  else {
    // --hosts assumes --aliases
    drush_set_option('aliases', TRUE);
  }
  
  $aliases = drush_get_option('aliases', FALSE);
  if ($aliases === FALSE) {
    $aliases = drush_confirm(dt('Would you like to generate Drush aliases for your project?'));
    if ($aliases !== FALSE) {
      // Save the new option
      drush_set_option('aliases', TRUE);
      // --aliases assumes --up
      drush_set_option('up', TRUE);
    }
  }
  else {
    // --aliases assumes --up
    drush_set_option('up', TRUE);
  }

  $up = drush_get_option('up', FALSE);
  if ($up === FALSE) {
    drush_confirm(dt('Would you like to launch the VMs immediately?'));
    if ($up !== FALSE) {
      // Save the new option
      drush_set_option('up', TRUE);
    }
  }

  $project_path = PROJECTS_PATH . $project_name;
  drush_set_option('project-path', $project_path);
  drush_print(dt("You are about to:\n * Create a new project at '!project_path'.",
    array('!project_path'  => $project_path))
  );

  if (isset($git_repo)) {
    drush_print(dt(" * Build the project from the Git repository at: !git-repo", array('!git-repo' => $git_repo,)));
  }
  else {
    drush_print(dt(" * Build the project using the '!blueprint' blueprint from the '!extension' extension.",
      array('!blueprint' => $blueprint, '!extension' => $blueprint_info[$blueprint]['extension']))
    );
  }
  if ($up) {
    drush_print(dt(" * Launch the project's VMs immediately."));
  }
  if ($aliases) {
    drush_print(dt(" * Generate Drush aliases for the project and VMs."));
  }
  if ($hosts) {
    drush_print(dt(" * Add entries for the VMs' FQDNs to /etc/hosts. (You will be prompted for your sudo password.)"));
  }

  if (!drush_confirm(dt('Do you want to proceed with initializing the project?'))) {
    return drush_user_abort();
  }

  // We need to check subnets before we create the new project, to avoid a race condition
  $subnet = _drush_vagrant_next_available_subnet();

  // Create the project directory from the blueprint, or the git repo
  if ($git_repo) {
    drush_shell_exec_interactive('git clone %s %s', $git_repo, $project_path);
  }
  else {
    drush_copy_dir($blueprint_info[$blueprint]['full_path'], $project_path);
  }
/*
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
*/
  // Set up the new project
  symlink(DRUSH_VAGRANT_ROOT . '/lib/Vagrantfile', $project_path . '/Vagrantfile');
  symlink(DRUSH_VAGRANT_ROOT . '/lib/gitignore', $project_path . '/.gitignore');
  $config_path = $project_path . '/.config';
  drush_mkdir($config_path);

  if (!isset($git_repo)) {
    // Record the blueprint that was used
    $blueprint_vars['extension'] = $blueprint_info[$blueprint]['extension'];
    $blueprint_vars['blueprint'] = $blueprint;
    $blueprint_file = _drush_vagrant_render_template($blueprint_vars, $template = "blueprint.tpl.php");
    file_put_contents($config_path . '/blueprint.inc', $blueprint_file);
  }

  $config_vars = array();
  $config_vars["global_path"] =  DRUSH_VAGRANT_ROOT . '/lib/global.rb';
  $config_vars["project_name"] = $project_name;
  $config_vars["vagrant_modules_path"] = DRUSH_VAGRANT_ROOT . '/lib/modules';
  $config_vars["subnet"] = $subnet;
  $config_vars["username"] = $user_settings['vagrant_username'];
  $config_vars["git_name"] = $user_settings['vagrant_git_name'];
  $config_vars["git_email"] = $user_settings['vagrant_git_email'];
  $config_vars["uid"] = $user_settings['vagrant_uid'];
  $config_vars["gid"] = $user_settings['vagrant_gid'];
  $config_file = _drush_vagrant_render_template($config_vars, $template = "config.tpl.php");
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

  drush_shell_exec_interactive("cd %s && git init && git add . && git commit -m'Initial commit.'", $project_path);
}

/**
 * Post-build configuration
 */
function drush_vagrant_post_vagrant_build() {
  $project_path = drush_get_option('project-path');
  $aliases = drush_get_option('aliases', FALSE);
  $hosts = drush_get_option('hosts', FALSE);
  $up = drush_get_option('up', FALSE);
  $subnet =  _drush_vagrant_get_subnet($project_path);

  // Launch the VMs (or skip, as aliases will do it for us)
  if ($up !== FALSE && $aliases === FALSE) {
    drush_shell_exec_interactive("cd %s && vagrant up", $project_path);
  }

  // Generate aliases for the new project
  if ($aliases !== FALSE) {
    drush_shell_exec_interactive("cd %s && drush vagrant aliases", $project_path);
  }

  // Add domain to hosts file
  if ($hosts !== FALSE) {
    drush_print(dt('Enter your password to add entries for the VMs in your project to your /etc/hosts file.'));
    drush_shell_cd_and_exec($project_path, 'drush vagrant list --only-vms');
    $vms = drush_shell_exec_output();
    foreach ($vms as $vm) {
      $vm_alias = '@' . drush_vagrant_get_project_name($project_path) . '.' . $vm;
      $domain = drush_sitealias_get_record($vm_alias);
      $domain = $domain['fqdn'];
      drush_shell_exec_interactive("drush %s vagrant hosts --domains=%s", $vm_alias, $domain);
    }
  }

  drush_print(dt('Your project was successfully initialized.'));
  drush_print(dt('The root for your project is \'!project_path\'', array('!project_path' => $project_path)));
  drush_print(dt('The subnet for your project has been set to 192.168.!subnet.0, and an initial entry in your /etc/hosts file written.', array('!subnet' => $subnet)));
  drush_print(dt('You can now: * Alter your project\'s behaviour by editing !project_path/settings.rb.', array('!project_path' => $project_path)));
  drush_print(dt('             * Redefine the VM(s) by editing the Puppet manifest(s) in !project_path/manifests.', array('!project_path' => $project_path)));
  drush_print(dt('             * Add additional Puppet modules by copying them to !project_path/modules.', array('!project_path' => $project_path)));

}

/**
 * Ensure that a project name is unique and well-formed
 */
function _drush_vagrant_check_project_name($project_name, $set_error = FALSE) {
  
  // Check whether a project with this name already exists.
  if (is_dir(PROJECTS_PATH . $project_name)) {
    $error_msg = dt('ERROR: There is already a project called \'!project\'.', array('!project' => $project_name));
    if ($set_error) {
      drush_set_error($error_msg);
    }
    else {
      return $error_msg;
    }
  }

  // Check that the project name can safely be used in a URL.
  $regex = '/^([a-z0-9][a-z0-9.-]*[a-z0-9])$/';
  if (preg_match($regex, $project_name) < 1) {
    $error_msg = dt('ERROR: the name of your project (!project) should only contain lower-case letters and numbers. It can also contain hyphens (-) and dots (.), so long as they don\'t come at the beginning or end of the name.',
      array('!project' => $project_name)
    );
    if ($set_error) {
      drush_set_error($error_msg);
    }
    else {
      return $error_msg;
    }
  }
  return TRUE;
}

/**
 * Ensure that a Git repository is accessible.
 */
function _drush_vagrant_check_git_repo($git_repo, $set_error = FALSE) {
  
  // Check that Git is installed.
  if ($set_error) {
    $status = drush_shell_exec('which git');
    if ($status == FALSE) {
      drush_set_error(dt("ERROR: You have specified a Git repository from which to build a project, but Git does not appear to be installed.\nPlease install Git before continuing.\n\n"));
    }
  }

  // Check that we can access the Git repo.
  $status = drush_shell_exec('git ls-remote ' . $git_repo);
  if ($status == FALSE) {
    $output = drush_shell_exec_output();
    $error_msg = dt("There was a problem accessing the Git repository. The output of 'git ls-remote' is:\n   ") . implode("\n   ", $output);
    if ($set_error) {
      drush_set_error(dt('ERROR:') . $error_msg);
    }
    else {
      drush_print($error_msg, 'warning');
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * Return the next available subnet
 */
function _drush_vagrant_next_available_subnet() {

  $initial_subnet = 10;
  drush_shell_exec('drush vagrant list --only-projects');
  $projects = drush_shell_exec_output();

  $subnets = array();
  foreach ($projects as $project) {
    $subnets[$project] = _drush_vagrant_get_subnet(PROJECTS_PATH . $project);
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

