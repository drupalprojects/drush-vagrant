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

