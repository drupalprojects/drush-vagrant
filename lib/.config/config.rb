require "/path/to/global.rb"
require "./settings.rb"

class Conf < Global
  Project   = "project_name"
  Modules   = "vagrant_modules_path"    # puppet modules folder name
  Subnet    = "subnet"                  # 192.168.###.0/24 subnet for this network
  Facts     = [
              # Fix for broken $fqdn fact on Debian
              [ "fqdn",               $hostname ],
              # Add user variables to be processed in drush-vagrant::user
              [ "drush_vagrant_username",  "username" ],
              [ "drush_vagrant_git_name",  "Firstname Lastname" ],
              [ "drush_vagrant_git_email", "username@example.com" ],
              # For NFS, these can be used to match uid/gid to the host user
              [ "drush_vagrant_uid",       "uid" ],
              [ "drush_vagrant_gid",       "gid" ],
              ] 
end

