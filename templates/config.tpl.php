<?php
/**
 * @file
 * Default implementation to output a Drush Vagrant config file.
 *
 * Variables:
 * - $global_path: The filesystem path to global.rb config file
 * - $project_name: The name of the Vagrant project
 * - $vagrant_modules_path: The filesystem path to Puppet modules directory
 * - $subnet: The /24 subnet the project will use.
 * - $username: The user's system username
 * - $git_name: The user's full name, as registered with Git
 * - $git_email: The user's email address, as registered with Git
 * - $uid: The user's system user ID, used for NFS
 * - $gid: The user's system group ID, used for NFS
 */
?>
require "<?php print $global_path; ?>"
require "./settings.rb"

class Conf < Global
  Project   = "<?php print $project_name; ?>"
  Modules   = "<?php print $vagrant_modules_path; ?>"    # puppet modules folder name
  Subnet    = "<?php print $subnet; ?>"                  # 192.168.###.0/24 subnet for this network
  Facts     = { # Fix for broken $fqdn fact on Debian
                "fqdn"                    => $hostname,
                # Add user variables to be processed in drush-vagrant::user
                "drush_vagrant_username"  => "<?php print $username; ?>",
                "drush_vagrant_git_name"  => "<?php print $git_name; ?>",
                "drush_vagrant_git_email" => "<?php print $git_email; ?>",
                # For NFS, these can be used to match uid/gid to the host user
                "drush_vagrant_uid"       => "<?php print $uid; ?>",
                "drush_vagrant_gid"       => "<?php print $gid; ?>",
              }
end
