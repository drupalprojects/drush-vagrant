<?php
/**
 * drush-vagrant helps you add your Vagrant VMs as drush remote aliases.
 */

/**
 * Implementation of hook_drush_help().
 */
function vagrant_drush_help($section) {
  switch ($section) {
    case 'meta:vagrant:title':
      return dt("Vagrant integration");
    case 'meta:vagrant:summary':
      return dt("Add Vagrant VMs as drush remotes.");
  }
}

/**
 * Implementation of hook_drush_command().
 */
function vagrant_drush_command() {
  $items['vagrant'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
  );
  $items['vagrant-alias-config'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'description' => 'Convert the output of `vagrant ssh_config` into a Drush remote alias.',
  );
  return $items;
}

/**
 * Command callback for `drush vagrant`.
 */
function drush_vagrant() {
  $args = func_get_args();
  $command = array_shift($args);
  if (empty($command)) {
    $command = 'alias-config';
  }

  $commands = drush_get_commands();
  $command = $commands['vagrant-'. $command];

  drush_dispatch($command, $args);
}

/**
 * Command callback for `drush vagrant-alias-config`.
 */
function drush_vagrant_alias_config() {
  $args = func_get_args();

  drush_print("Attempting to read the Vagrant SSH configuration.");
  $status = drush_shell_exec("vagrant ssh_config");
  $output = drush_shell_exec_output();
  if (!$status) {
    drush_log(dt("The following errors occurred when running `vagrant ssh_config`. Are you in the directory with the Vagrantfile?"), "error");
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
  $command = drush_get_command();
  $template_path = $command['path'] . '/templates/' . $template;

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

