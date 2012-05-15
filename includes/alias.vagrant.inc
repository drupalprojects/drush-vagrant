<?php

/**
 * Command callback for `drush vagrant-alias-config`.
 */
function drush_vagrant_alias_config() {
  $args = func_get_args();

  drush_print("Attempting to read the Vagrant SSH configuration.");
  $status = drush_shell_cd_and_exec(PROJECT_PATH, "vagrant ssh-config " . VM_NAME);
  $output = drush_shell_exec_output();
  if (!$status) {
    drush_log(dt("The following errors occurred when running `vagrant ssh-config`. Are you in the directory with the Vagrantfile?"), "error");
    if (!empty($output)) {
      foreach ($output as $line) {
        drush_print($line);
      }
    }
    exit(1);
  }
  else {
    drush_log(dt("No errors found in the Vagrant SSH configuration."), "ok");
  }

  $ssh_options = array();
  foreach ($output as $line) {
    $split_line = explode(" ", trim($line));
    $ssh_options[current($split_line)] = implode(" ", $split_line);
  }

  // These do not need to go into $alias['ssh-options'].
  unset($ssh_options["User"]);
  unset($ssh_options["Host"]);

  // Set up variables to send to the template.
  $variables = array();
  $variables["ssh_options"] = array_reduce($ssh_options, "_drush_vagrant_join_ssh_commands");

  drush_print(dt("Please enter the information about the Drupal site inside the VM."));
  $variables["alias"] = drush_prompt("Alias", "drush-vagrant");
  $variables["uri"] = drush_prompt("URI", "http://vagrantvm.dev/");
  $variables["docroot"] = drush_prompt("Docroot location", "/var/www/html/");

  drush_print(dt("Rendering the remote alias entry."));

  $output = _drush_vagrant_render_template($variables);
  if ($output === NULL) {
    drush_log(dt("Could not render the remote alias entry."), "error");
  }
  else {
    drush_log(dt("The remote alias entry is below. Please paste it into an aliases.drushrc.php file."), "ok");
    drush_print();
    drush_print($output);
  }
}

/**
 * Reduce the ssh options to one long line.
 */
function _drush_vagrant_join_ssh_commands($x, $acc) {
  $x .= "-o'{$acc}' ";
  return $x;
}

/**
 * Run all the variables through a template file.
 */
function _drush_vagrant_render_template($variables, $template = "alias.tpl.php") {
  $template_path = DRUSH_VAGRANT_ROOT . '/templates/' . $template;

  try {
    extract($variables);
    ob_start();
    include $template_path;
    $content = ob_get_clean();
    return $content;
  }
  catch (Exception $e) {
    return FALSE;
  }
}

/**
 * Generate aliases for a project and its VMs.
 */
function drush_vagrant_aliases() {
  $project_path = drush_get_context('project-path', getcwd());

  $drush_home = drush_server_home() . '/.drush/';
  if (!is_dir($drush_home)) {
    drush_mkdir($drush_home);
  }

  $alias_filename = $drush_home . drush_vagrant_get_project_name($project_path) . '.aliases.drushrc.php';

  if ($delete = drush_get_option('delete')) {
    if (is_file($alias_filename)) {
      if (!drush_confirm('Delete the aliases for this project?')) {
        return drush_user_abort();
      }
      $status = drush_shell_exec('rm %s', $alias_filename);
      return $status;
    }
    else {
      drush_log(dt('ERROR: There do not appear to be any aliases for this project.'), 'warning');
    }
  }
  else {

    if (is_file($alias_filename)) {
      drush_log(dt('WARNING: There appear to already be aliases for this project.'), 'warning');
      if (!drush_confirm('Regenerate the aliases for this project?')) {
        return drush_user_abort();
      }
    }

    drush_shell_exec_interactive('cd ' . $project_path . ' && drush vagrant up');

    $project_vars["project_path"] = $project_path;

    $vms_info = drush_vagrant_vms_info($project_path);
    foreach ($vms_info as $vm_name => $data) {
      $vm_vars = $project_vars;
      $vm_vars['vm_alias'] = $vm_name;
      drush_shell_cd_and_exec($project_path, 'drush vagrant shell --vm-name=' . $vm_name . ' "hostname -f"');
      $output = drush_shell_exec_output();
      $vm_vars['fqdn'] = $output[0];
      drush_shell_cd_and_exec($project_path, 'drush vagrant shell --vm-name=' . $vm_name . ' "hostname -I"');
      $output = drush_shell_exec_output();
      $vm_vars['ip_addr'] = array_pop(explode(' ', $output[0]));
      $vms[] = _drush_vagrant_render_template($vm_vars, 'vm_alias.tpl.php');
    }
    $project_vars["vms"] = $vms;

    $aliases = _drush_vagrant_render_template($project_vars, 'project_alias.tpl.php');

    $status = file_put_contents($alias_filename, _drush_vagrant_render_template($project_vars, 'project_alias.tpl.php'));
    drush_print($status);
  }
}
