<?php

/**
 * @file
 * Manage entries in /etc/hosts.
 */

define('HOSTS_BEGIN_MARKER', "#### Drush Vagrant entries begin ####");
define('HOSTS_END_MARKER', "#### Drush Vagrant entries end ####");

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
    $hosts_file = file('/etc/hosts', FILE_IGNORE_NEW_LINES);

    // Find markers that delineate our block of entries
    foreach ($hosts_file as $count => $line) {
      if (strpos($line, HOSTS_BEGIN_MARKER) !== FALSE) {
        $hosts_begin = $count;
      }
      if (strpos($line, HOSTS_END_MARKER) !== FALSE) {
        $hosts_end = $count;
      }
    }
    // Add markers to end of file if none were found
    if (!isset($hosts_begin) && !isset($hosts_end)) {
      $hosts_begin = count($hosts_file);
      $hosts_file[] = HOSTS_BEGIN_MARKER;
      $hosts_end = count($hosts_file);
      $hosts_file[] = HOSTS_END_MARKER;
    }
    // Sanity check
    if (!($hosts_end > $hosts_begin)) {
      drush_set_error('ERROR: Error parsing hosts file.');
      return;
    }

    if ($purge = drush_get_option('purge', FALSE)) {
      if (!drush_confirm('Remove all Drush Vagrant entries from /etc/hosts?')) {
        drush_user_abort();
        return;
      }
      array_splice($hosts_file, $hosts_begin, ($hosts_end - $hosts_begin) + 1, NULL);
      $output = implode("\n", $hosts_file) . "\n";
      $result = file_put_contents('/etc/hosts', $output);
      return;
    }

    if (!($ip_addr = drush_get_option('ip-addr'))) {
      drush_set_error('ERROR: Could not find an IP address to add. Try specifying one with the "--ip-addr" option, or using a VM alias.');
      return;
    }
    $remove = drush_get_option('remove', FALSE);
    if (!($domains = drush_get_option_list('domains')) && !($remove)) {
      drush_set_error('ERROR: Could not find any domains to add. Try specifing one or more with the "--domains" option.');
      return;
    }

    // Extract our entries
    $hosts = array_slice($hosts_file, $hosts_begin + 1, ($hosts_end - $hosts_begin) - 1);

    // Find the IP address
    foreach ($hosts as $count => $line) {
      if (strpos($line, $ip_addr) !== FALSE) {
        $ip_line = $count;
      }
    }
    // Remove a line or domain
    if ($remove) {
      if (!isset($ip_line)) {
        drush_set_error('ERROR: Cannot find IP address ' . $ip_addr . ' in /etc/hosts.');
        return;
      }
      elseif (count($domains) > 0) {
        foreach ($domains as $domain) {
          $hosts[$ip_line] = str_replace(' ' . $domain, '', $hosts[$ip_line]);
        }
        // If there are no domains left on a line, remove it
        if (trim($hosts[$ip_line]) == $ip_addr) {
          unset($hosts[$ip_line]);
        }
      }
      else {
        unset($hosts[$ip_line]);
      }
    }
    // Add an IP address and/or domain
    else {
      if (!isset($ip_line)) {
        $hosts[] = $ip_addr . "   ";
        $ip_line = count($hosts) - 1;
      }

      foreach ($domains as $domain) {
        $hosts[$ip_line] = $hosts[$ip_line] . ' ' . $domain;
      }
    }

    array_splice($hosts_file, $hosts_begin + 1 , ($hosts_end - $hosts_begin) - 1, $hosts);

    $output = implode("\n", $hosts_file) . "\n";
    $result = file_put_contents('/etc/hosts', $output);

    return $result;
  }
}
