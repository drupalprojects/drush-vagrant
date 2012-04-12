<?php

/**
 * Delete an existing workspace.
 */
function drush_vagrant_delete($workspace) {

  _drush_vagrant_check_workspace($workspace);

  $subnet = _drush_vagrant_get_subnet($workspace);

  drush_shell_exec('grep 192.168.%s.10 /etc/hosts', $subnet);
  $remove_lines = drush_shell_exec_output();

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
      drush_set_error(dt('Failure during \'vagrant destroy\''));
    }
  }
  else {
    drush_user_abort();
  }
}
