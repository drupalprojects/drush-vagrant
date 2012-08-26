<?php

/**
 * @file
 * Save some user-specific settings in a Drush context
 */

/**
 * Create or update user-specific settings.
 */
function drush_vagrant_user() {

  $user_settings = array();
  $data = array();

  // Environment variables
  // TODO: allow other env vars to be passed through an option or hook?
  $data['vagrant_username'] = array( 'command' => 'whoami', 'label' => 'username');
  if (!drush_is_windows()) {
    $data['vagrant_uid'] = array( 'command' => 'id -u', 'label' => 'user id');
    $data['vagrant_gid'] = array( 'command' => 'id -g', 'label' => 'group id');
  }
  // TODO: else {provide some default} ?
  drush_shell_exec('git --version');
  $result = drush_shell_exec_output();
  $result = explode(' ', $result[0]);
  if ($result[0] == 'git' && $result[1] == 'version') {
    $data['vagrant_git_name'] = array( 'command' => 'git config --global --get user.name', 'label' => 'Git username');
    $data['vagrant_git_email'] = array( 'command' => 'git config --global --get user.email', 'label' => 'Git email');
  }
  // Iterate over our list of commands, and get their output.
  foreach ($data as $key => $datum) {
    drush_shell_exec($datum['command']);
    $default = drush_shell_exec_output();
    $data[$key]['default'] = $default[0];
  }
  
  $data['vagrant_home'] = array('default' => drush_server_home(), 'label' => "User's home directory");
  $data['vagrant_project_root'] = array( 'default' => $data['vagrant_home']['default'] . '/vagrant/projects', 'label' => 'Projects directory');

  // Files
  // TODO: allow other files & filesets to be passed through an option or hook?
  $file_sets = array('dotfiles', 'ssh_keys' );
  $dotfiles = array('.profile', '.bashrc', '.bash_aliases', '.vimrc', );
  if (file_exists($data['vagrant_home']['default'] . '/.ssh/id_rsa')) {
    $ssh_keys = array('public' => '.ssh/id_rsa.pub', 'private' => '.ssh/id_rsa');
  }
  elseif (file_exists($data['vagrant_home']['default'] . '/.ssh/id_dsa')) {
    $ssh_keys = array('public' => '.ssh/id_dsa.pub', 'private' => '.ssh/id_dsa');
  } 

  // Get file defaults
  foreach ($file_sets as $set) {
    foreach ($$set as $key => $file) {
      if (is_numeric($key)) {
        $name = $file;
      }
      else {
        $name = $key;
      }
      if (file_exists($data['vagrant_home']['default'] . '/' . $file)) {
        $data['vagrant_' . $set . $name] = array('default' => $data['vagrant_home']['default'] . '/' . $file, 'label' => $name . '(' . $set . ')');
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

  if (!is_dir($user_settings['vagrant_project_root'])) {
    mkdir($user_settings['vagrant_project_root'], 0750, TRUE);
  }

  drush_set_context('user', $user_settings);
  drush_save_config('user');
}
