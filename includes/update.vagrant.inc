<?php

/**
 * @file
 * Update Git submodules provided by blueprints.
 */

use Symfony\Component\Yaml\Yaml;

/**
 * Update Git submodules provided by blueprints.
 */
function drush_vagrant_update_submodules() {

  if (file_exists(PROJECT_PATH . '/config/landscape.yml')) {
    if (file_exists(PROJECT_PATH . '/.git')) {

      $landscape = Yaml::parse(PROJECT_PATH . '/config/landscape.yml');
      $commands = drush_get_context('DRUSH_COMMANDS');
      $command_files = drush_commandfile_list();

      foreach ($landscape as $blueprint => $info) {
        $command = $info['command'];
        if (file_exists($command_files[$command])) {
          $command_file = $command_files[$command];
          $command_path = $commands[$command]['path'];

          // Query the extension for it's blueprints
          include_once $command_file;
          $blueprints = call_user_func($command . '_vagrant_blueprints');

          $blueprint_path = $command_path . '/' . $blueprints[$blueprint]['path'];
          $source = $blueprint_path . '/submodules.yml';
          $destination = PROJECT_PATH . '/config/submodules/' . $blueprint . '.yml';

          if (file_exists($source)) {
            if (copy($source, $destination)) {
              drush_log('Updated submodules manifest from ' . $blueprint, 'success');
            }
          }
        }
        else {
          drush_log('The extension for ' . $command . ' could not be found. Skipping update.', 'warning');
        }

        $submodules = Yaml::parse(PROJECT_PATH . '/config/submodules/' . $blueprint . '.yml');

        foreach ($submodules as $submodule => $submodule_info) {
          if (file_exists($submodule_info['location'] . '/.git')) {
            $write = false;
            if ($submodule_info['write_url'] != '') {
              // Try using a writeable url first
              drush_shell_cd_and_exec(PROJECT_PATH . '/' . $submodule_info['location'], 'git remote set-url' . $submodule_info['write_url']);
              if (drush_shell_cd_and_exec(PROJECT_PATH . '/' . $submodule_info['location'], 'git pull')) {
                $write = true;
                drush_log('Pulled in ' . $submodule_info['location'] . ' from writeable remote repo at ' . $submodule_info['write_url'], 'success');
              }
              else {
                drush_log('Failed to pull from writable remote repo at ' . $submodule_info['read_url'] . 'in ' . $submodule_info['location'] . '. Trying with read-only url.', 'notice');
              }
            }
            if ($submodule_info['read_url'] != '' && $write !== true) {
              drush_shell_cd_and_exec(PROJECT_PATH . '/' . $submodule_info['location'], 'git remote set-url' . $submodule_info['read_url']);
              if (drush_shell_cd_and_exec(PROJECT_PATH . '/' . $submodule_info['location'], 'git pull')) {
                drush_log('Pulled in ' . $submodule_info['location'] . ' from read-only remote repo at ' . $submodule_info['read_url'], 'success');
              }
              else {
                drush_log('Failed to pull from remote repo at ' . $submodule_info['read_url'] . 'in ' . $submodule_info['location'], 'failure');
              }
            }
          }
          else {
            if (!file_exists(PROJECT_PATH . '/' . $submodule_info['location'])) {
              // The submodule doesn't exist yet, so let's add it
              if (drush_shell_cd_and_exec(PROJECT_PATH, 'git submodule add ' . $submodule_info['write_url'] . ' ' . $submodule_info['location'])) {
                drush_log('Added submodule at ' . $submodule_info['location'] . ' from remote at ' . $submodule_info['write_url'], 'success');
              }
              else {
                drush_log('Failed adding submodule at ' . $submodule_info['location'] . ' from remote at ' . $submodule_info['write_url'] . '. Trying with read-only url.', 'notice');
                if (drush_shell_cd_and_exec(PROJECT_PATH, 'git submodule add ' . $submodule_info['read_url'] . ' ' . $submodule_info['location'])) {
                  drush_log('Added submodule at ' . $submodule_info['location'] . ' from remote at ' . $submodule_info['read_url'], 'success');
                }
                else {
                  drush_log('Failed adding submodule at ' . $submodule_info['location'] . ' from remote at ' . $submodule_info['read_url'], 'failure');
                }
              }
            }
          }
        }
      }
    }
    else {
      drush_log('This directory does not appear to be a Git repository. Try "git init".', 'error');
    }
  }
  else {
    drush_log('Could not find landscape file at config/landscape.yml', 'error');
  }
}
