<?php

/**
 * Create or update user-specific settings.
 */
function drush_vagrant_user() {

  $user_settings = array();
  $data = array();

  // Environment variables
  // TODO: allow other env vars to be passed through an option or hook?
  $data['vagrant_username'] = array( 'command' => 'whoami', 'label' => 'username');
  $data['vagrant_home'] = array( 'command' => 'echo $HOME', 'label' => 'home folder');
  $data['vagrant_uid'] = array( 'command' => 'id -u', 'label' => 'user id');
  $data['vagrant_gid'] = array( 'command' => 'id -g', 'label' => 'group id');
  $data['vagrant_workspace_root'] = array( 'command' => 'echo $HOME/aegir-up/', 'label' => 'Workspace folder');
  $data['vagrant_git_name'] = array( 'command' => 'git config --global --get user.name', 'label' => 'Git username');
  $data['vagrant_git_email'] = array( 'command' => 'git config --global --get user.email', 'label' => 'Git email');
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
      if (file_exists($data['vagrant_home']['default'] . '/' . $file)) {
        $data['vagrant_' . $set . $name] = array('default' => $data['vagrant_home']['default'] . '/' . $file, 'label' => $name . '(' . $set .')');
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

  if (!is_dir($user_settings['vagrant_workspace_root'])) {
    mkdir($user_settings['vagrant_workspace_root']);
  }

  _debug("WORKSPACE_ROOT", WORKSPACE_ROOT);

  drush_set_context('user', $user_settings);
  drush_save_config('user');
}