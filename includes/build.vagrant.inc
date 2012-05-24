<?php

/**
 * @file
 * Build projects.
 */

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
      drush_log(dt("Could not find the '!blueprint' blueprint.", array('!blueprint' => $blueprint)), "error");
    }
  }
}

/**
 * Initialize a new project.
 */
function drush_vagrant_build() {

  $user_settings = drush_get_context('user');

  // Prompt for a project name, if one wasn't passed as an option
  if (!($project_name = drush_get_option('project-name'))) {
    drush_print(dt('The name of your project may be used in URLs, and so should only contain lower-case letters and numbers.'));
    drush_print(dt('It may also contain hyphens (-) and dots (.), so long as they do not come at the beginning or end of the name.'));
    // Only accept a valid project name.
    while ($error_msg !== TRUE) {
      $project_name = drush_prompt(dt('What would you like to call your project?'));
      $error_msg = _drush_vagrant_check_project_name($project_name);
      if ($error_msg !== TRUE) {
        drush_print($error_msg);
      }
    }
    drush_set_option('project-name', $project_name);
  }
  // Determine the project's path from its name
  $project_path = PROJECTS_PATH . $project_name;
  drush_set_option('project-path', $project_path);

  // Begin building our final confirmation prompt
  $actions = array();
  $actions[] = dt("You are about to:");
  $actions[] = dt("  * Create a new project called '!project_name' at '!project_path'.", array('!project_name' => $project_name, '!project_path' => $project_path));

  // Prompt for a Git repository, if one wasn't passed as an option
  if (!($git_repo = drush_get_option('git-repo'))) {
    drush_print(dt('You can build your project based on a Git repository, or a blueprint. Provide the URL of a Git repository, or leave blank to choose a template.'));
    // Only accept a valid git repo, or pass through to blueprint selection.
    do {
      $git_repo = drush_prompt(dt('Git URL'), 'none');
    } while ($git_repo != 'none' && !_drush_vagrant_check_git_repo($git_repo));
    if ($git_repo == 'none') {
      unset($git_repo);
    }
  }

  $blueprint_info = drush_vagrant_blueprint_info();
  // Prompt for choice of blueprint, if one wasn't passed as an option, and we aren't building from a Git repo.
  if (!($blueprint = drush_get_option('blueprint')) && !isset($git_repo)) {
    foreach ($blueprint_info as $name => $data) {
      $choice[$name] = $data['name'] . ': ' . $data['description'] ;
    }
    $prompt = dt('Which blueprint would you like to use?');
    $blueprint = drush_choice($choice, $prompt);
    if ($blueprint == '') {
      // User chose 'cancel'
      return drush_user_abort();
    }
    drush_set_option('blueprint', $blueprint);
  }

  $hosts = drush_get_option('hosts', FALSE);
  if ($hosts === FALSE) {
    $hosts = drush_confirm(dt('Would you like to generate entries in /etc/hosts for the VMs in your project?'));
    if ($hosts !== FALSE) {
      // Save the new option
      drush_set_option('hosts', TRUE);
      $actions[] = dt("  * Add entries for the VMs' FQDNs to /etc/hosts. (You will be prompted for your sudo password.)");
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
      $actions[] = dt("  * Generate Drush aliases for the project and VMs.");
      // --aliases assumes --up
      drush_set_option('up', TRUE);
    }
  }
  else {
    $actions[] = dt("  * Generate Drush aliases for the project and VMs.");
    // --aliases assumes --up
    drush_set_option('up', TRUE);
  }

  $up = drush_get_option('up', FALSE);
  if ($up === FALSE) {
    drush_confirm(dt('Would you like to launch the VMs immediately?'));
    if ($up !== FALSE) {
      // Save the new option
      drush_set_option('up', TRUE);
      $actions[] = dt("  * Launch the project's VMs immediately.");
    }
  }
  else {
    $actions[] = dt("  * Launch the project's VMs immediately.");
  }

  if (isset($git_repo)) {
    $actions[] = dt("  * Build the project from the Git repository at: !git-repo", array('!git-repo' => $git_repo));
  }
  else {
    $actions[] = dt("  * Build the project using the '!blueprint' blueprint from the '!extension' extension.",
      array('!blueprint' => $blueprint, '!extension' => $blueprint_info[$blueprint]['extension'])
    );
  }

  foreach ($actions as $action) {
    drush_print($action);
  }

  if (!drush_confirm(dt('Do you want to proceed with initializing the project?'))) {
    return drush_user_abort();
  }
  // Check subnets before we create the new project, to avoid a race condition
  drush_set_option('next_subnet', _drush_vagrant_next_available_subnet());

  call_user_func($blueprint_info[$blueprint]['build_callback']);
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

  // Launch the VMs
  if ($up !== FALSE) {
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
    $project_name = drush_vagrant_get_project_name($project_path);
    foreach ($vms as $vm) {
      $vm_alias = '@' . $project_name . '.' . $vm;
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

