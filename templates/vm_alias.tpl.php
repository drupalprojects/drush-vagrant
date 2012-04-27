<?php
/**
 * @file
 * Template to generate a drush alias for a VM.
 *
 * Variables:
 * - $alias: The Vagrant name of the VM.
 * - $hostname: The full hostname of the VM.
 * - $parent_project: The alias of the project this VM is part of.
 * - $ip_addr: The IP address of the VM.
 * - $ssh_options: The parsed output of `vagrant ssh_config`.
 */
?>
<?php print "<?php\n\n" ?>
$aliases['<?php print $alias; ?>'] = array(
  'hostname' => '<?php print $hostname; ?>',
  'parent_project' => '<?php print $parent_project; ?>',
  'ip-addr' => '<?php print $ip_addr; ?>,
  'ssh-options' => "<?php print $ssh_options; ?>",
);
