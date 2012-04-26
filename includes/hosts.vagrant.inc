<?php

/**
 * Add an entry to /etc/hosts.
 */
function drush_vagrant_hosts() {
  // This command must be run as root
  if (posix_getuid() > 0) {
    $options_array = drush_redispatch_get_options();
    $options_string = '';
    foreach ($options_array as $option => $value) {
      $options_string .= ' --' . $option . '=' . $value;
    }
    drush_shell_exec_interactive("sudo -E drush vagrant-hosts" . $options_string);
  }
  else {
    $ip_addr = drush_get_option('ip-addr');
    $domains = drush_get_option_list('domains');

    $hosts_file = file('/etc/hosts', FILE_IGNORE_NEW_LINES);

    $ip_line = FALSE;

    foreach ($hosts_file as $count => $line) {
      if (strpos($line, $ip_addr) !== FALSE) {
        $ip_line = $count;
      }
    }
    if ($ip_line === FALSE) {
      $hosts_file[] = $ip_addr . "   ";
      $ip_line = count($hosts_file) - 1;
    }

    foreach ($domains as $domain) {
      $hosts_file[$ip_line] = $hosts_file[$ip_line] . ' ' . $domain;
    }

    $output = implode("\n", $hosts_file) . "\n";

    $result = file_put_contents('/etc/hosts', $output);

    return $result;
  }
}
