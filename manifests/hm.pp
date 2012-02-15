# A Puppet manifest to provision an Aegir Hostmaster server

group { 'puppet': ensure => present, }

# Set some defaults, and make output less verbose
Group { loglevel => 'info', }
Package { loglevel => 'info', }
Notify { loglevel => 'info', }
Exec { path  => [ "/bin/", "/sbin/" , "/usr/bin/", "/usr/sbin/" ], loglevel => 'info', }
File { owner => 0, group => 0, mode => 0644, loglevel => 'info', }

file { '/etc/motd':
  content => "Welcome to your Aegir Hostmaster virtual machine!
              Built by Vagrant. Managed by Puppet.\n
              Developed and maintained by Ergon Logic Enterprises.\n"
}

import "common"

# Optional settings for Aegir front-end
#  $aegir_site = 'test.aegir.local' #req'd for manual builds
#  $aegir_db_host = 'db.aegir.local'
#  $aegir_db_user = 'root'
#  $aegir_db_password = 'password'
#  $aegir_email = 'test@ergonlogic.com'
#  $aegir_makefile = 'aegir.make'

# Build 'manually' or from packages (.debs, &c.)
# N.B. Manual installs require $aegir_site to be set
#  $aegir_manual_build = TRUE

# Additional optional settings available if $aegir_manual_build = TRUE
#
# WARNING! Only change these if you really know what you are doing, and even
# then, think twice. Changing these can result in a broken and/or unusable
# Aegir installation.
#
#  $http_service_type = 'apache' 
#  $drush_make_version = '6.x-2.3'
#  $script_user = 'aegir'
#  $web_group = 'www-data'
#  $aegir_version = '6.x-1.6'
#  $aegir_root = '/var/aegir'

notice("\nRunning Puppet manifests to install and/or update Aegir.\n
        This may take awhile, so please be patient.
        For more detail on the operations being run, edit settings.rb,
        and set 'verbose = 1'.")

include aegir

#class {'drush::status':
#  site_alias => 'hostmaster',
#  require => Class['aegir'],
#}

#class {'aegir::contrib': }
#class {'aegir::queue_runner': }

#aegir::platform {'Open_Atria':
#  makefile => 'http://drupalcode.org/project/openatria_makefiles.git/blob_plain/refs/heads/master:/stub-openatria.make',
#}
