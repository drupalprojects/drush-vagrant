<?php

/**
 * @file
 *
 * Hooks related to the Default blueprint.
 */

/**
 * Implemention of hook_vagrant_blueprints().
 */
function vagrant_vagrant_blueprints() {
  $blueprints = array(
    'default' => array(
      'name' => 'Default',
      'description' => 'The default blueprint.',
      'path' => 'blueprints/default',
      'build_callback' => 'vagrant_default_build',
      'update_callback' => 'vagrant_default_update',
      ),
    );
  return $blueprints;
}

/**
 * Implementation of drush_hook_pre_COMMAND().
 *
 * Set SSH username and identity file options
 */
function drush_vagrant_pre_vagrant_shell() {
  include_once(PROJECT_PATH . '/.config/blueprint.inc');
  if ($options['blueprint']['blueprint'] == 'default') {
    $user_settings = drush_get_context('user');
    drush_set_option('ssh-user', drush_get_option('ssh-user', $user_settings['vagrant_username']));
    drush_set_option('ssh-id', drush_get_option('ssh-id', $user_settings['vagrant_ssh_keysprivate']));
  }
}

/**
 * Implemention of COMMANDFILE_EXTENSION_build().
 */
function vagrant_default_build($build) {
  // Create the project directory from the blueprint, or the git repo
  if ($build['git_repo']) {
    drush_vagrant_exec_and_log(array(
      'command' => drush_shell_exec_interactive('git clone %s %s', $build['git_repo'], $build['project_path']),
      'success' => dt("Cloned the git repo at !git_repo to !project_path.", array('!git_repo' => $build['git_repo'], '!project_path' => $build['project_path'])),
      'failure' => dt("The following errors occurred when running `git clone !git_repo !project_path`.", array('!git_repo' => $build['git_repo'], '!project_path' => $build['project_path'])),
    ));
  }
  else {
    drush_copy_dir($build['blueprint_info'][$build['blueprint']]['full_path'], $build['project_path']);
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
  symlink(DRUSH_VAGRANT_ROOT . '/lib/Vagrantfile', $build['project_path'] . '/Vagrantfile');
  symlink(DRUSH_VAGRANT_ROOT . '/lib/gitignore', $build['project_path'] . '/.gitignore');
  $config_path = $build['project_path'] . '/.config';
  drush_mkdir($config_path);

  if (!isset($build['git_repo'])) {
    // Record the blueprint that was used
    $blueprint_vars['extension'] = $build['blueprint_info'][$build['blueprint']]['extension'];
    $blueprint_vars['blueprint'] = $build['blueprint'];
    $blueprint_file = _drush_vagrant_render_template($blueprint_vars, $template = "blueprint.tpl.php");
    file_put_contents($config_path . '/blueprint.inc', $blueprint_file);
  }

  $user_settings = drush_get_context('user');

  $config_vars = array();
  $config_vars["global_path"] =  DRUSH_VAGRANT_ROOT . '/lib/global.rb';
  $config_vars["project_name"] = $build['project_name'];
  $config_vars["vagrant_modules_path"] = DRUSH_VAGRANT_ROOT . '/lib/modules';
  $config_vars["subnet"] = drush_get_option('next_subnet', '10');
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
 * Implemention of COMMANDFILE_EXTENSION_update().
 */
function vagrant_default_update() {

}
