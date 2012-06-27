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
function vagrant_default_build() {
  vagrant_default_build_steps(vagrant_default_build_vars());
}

function vagrant_default_build_vars() {
  $build = array();
  $build['project_path'] = drush_get_option('project-path');
  $build['project_name'] = drush_get_option('project-name');
  $build['blueprint'] = drush_get_option('blueprint');
  $build['blueprint_info'] = drush_vagrant_blueprint_info();
  $build['git_repo'] = drush_get_option('git-repo');

  return $build;
}

function vagrant_default_build_steps($build) {
  vagrant_default_build_project_dir($build);
  vagrant_default_build_project_setup($build);
  vagrant_default_build_config_dir($build);
  vagrant_default_build_record_blueprint($build);
  vagrant_default_build_user_data($build);
  vagrant_default_build_user_dotfiles($build);
  vagrant_default_build_git_init($build);
}

function vagrant_default_build_project_dir($build) {
  // Create the project directory from the blueprint, or the git repo
  if ($build['git_repo']) {
    drush_vagrant_exec_and_log(array(
      'command' => drush_shell_exec_interactive('git clone %s %s', $build['git_repo'], $build['project_path']),
      'success' => dt("Cloned the git repo at !git_repo to !project_path.", array('!git_repo' => $build['git_repo'], '!project_path' => $build['project_path'])),
      'failure' => dt("The following errors occurred when running `git clone !git_repo !project_path`.", array('!git_repo' => $build['git_repo'], '!project_path' => $build['project_path'])),
    ));
  }
  else {
    drush_vagrant_exec_and_log(array(
      'command' => drush_copy_dir($build['blueprint_info'][$build['blueprint']]['full_path'], $build['project_path']),
      'success' => dt("Copied the `!blueprint` blueprint directory to !project_path.", array('!blueprint' => $build['blueprint'], '!project_path' => $build['project_path'])),
      'failure' => dt("The following errors occurred when running `drush_copy_dir()` in `vagrant_default_build_project_dir()`."),
    ));
  }
}

/*
  // Check for support of 64-bit client OSes
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

function vagrant_default_build_project_setup($build) {
  // Set up the new project
  $target = $build['project_path'] . '/Vagrantfile';
  if (!file_exists($target)) {
    drush_vagrant_exec_and_log(array(
      'command' => symlink(DRUSH_VAGRANT_ROOT . '/lib/Vagrantfile', $target),
      'success' => dt("Created symlink to Vagrantfile."),
      'failure' => dt("Errors occurred when running `symlink()` in `!function`.", array('!function' => __FUNCTION__)),
    ));
  }
  $target = $build['project_path'] . '/.gitignore';
  if (!file_exists($target)) {
    drush_vagrant_exec_and_log(array(
      'command' => symlink(DRUSH_VAGRANT_ROOT . '/lib/gitignore', $target),
      'success' => dt("Created symlink to .gitignore."),
      'failure' => dt("Errors occurred when running `drush_copy_dir()` in `vagrant_default_build_project_setup()`."),
    ));
  }
}

function vagrant_default_build_config_dir(&$build) {
  $build['config_path'] = $build['project_path'] . '/.config';
  drush_vagrant_exec_and_log(array(
    'command' => drush_mkdir($build['config_path']),
    'success' => dt("Created the `.config` directory in !project_path.", array('!project_path' => $build['project_path'])),
    'failure' => dt("Errors occurred when running `drush_mkdir()` in `vagrant_default_build_project_setup()`."),
  ));
}

function vagrant_default_build_record_blueprint($build) {
  if (!isset($build['git_repo'])) {
    // Record the blueprint that was used
    $blueprint_vars['extension'] = $build['blueprint_info'][$build['blueprint']]['extension'];
    $blueprint_vars['blueprint'] = $build['blueprint'];
    $blueprint_file = _drush_vagrant_render_template($blueprint_vars, "blueprint.tpl.php");
    drush_vagrant_exec_and_log(array(
      'command' => file_put_contents($build['config_path'] . '/blueprint.inc', $blueprint_file),
      'success' => dt("Wrote `blueprint.inc` to !config_path.", array('!config_path' => $build['config_path'])),
      'failure' => dt("Errors occurred when running `file_put_contents()` in `!function`.", array('!function' => __FUNCTION__)),
    ));
  }
}

function vagrant_default_build_user_data($build) {
  $user_settings = drush_get_context('user');

  $config_vars = array();
  $config_vars["global_path"] =  DRUSH_VAGRANT_ROOT . '/lib/global.rb';
  $config_vars["project_name"] = $build['project_name'];
  $config_vars["subnet"] = drush_get_option('next_subnet', '10');
  $config_vars["username"] = $user_settings['vagrant_username'];
  $config_vars["git_name"] = $user_settings['vagrant_git_name'];
  $config_vars["git_email"] = $user_settings['vagrant_git_email'];
  $config_vars["uid"] = $user_settings['vagrant_uid'];
  $config_vars["gid"] = $user_settings['vagrant_gid'];
  $config_file = _drush_vagrant_render_template($config_vars, $template = "config.tpl.php");
  file_put_contents($build['config_path'] . '/config.rb', $config_file);
}

function vagrant_default_build_user_dotfiles($build) {
  $user_settings = drush_get_context('user');

  // @todo: check for existence of these vars
  $dotfiles_dir = $build['config_path'] . '/files';
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

function vagrant_default_build_git_init($build) {
  if (!file_exists($build['project_path'] . '/.git')) {
    drush_shell_exec_interactive("cd %s && git init && git add . && git commit -m'Initial commit.'", $build['project_path']);
  }
}


/**
 * Implemention of COMMANDFILE_EXTENSION_update().
 */
function vagrant_default_update() {

}
