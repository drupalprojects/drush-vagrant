# A Puppet manifest to provision an Aegir Hostmaster server

group { 'puppet': ensure => present, }

File { owner => 0, group => 0, mode => 0644 }

file { '/etc/motd':
  content => "Welcome to your Aegir Hostmaster virtual machine!
              Built by Vagrant. Managed by Puppet.\n
              Developed and maintained by Ergon Logic Enterprises.\n"
}

Exec { path  => [ "/bin/", "/sbin/" , "/usr/bin/", "/usr/sbin/" ], }

import "common"

# Build 'manually' or from packages (.debs, &c.)
# $aegir_manual_build = TRUE

# Optional settings for Aegir front-end
#  $aegir_site = 'test.aegir.local' 
#  $aegir_db_host = 'db.aegir.local'
#  $aegir_db_user = 'root'
#  $aegir_db_password = 'password'
#  $aegir_email = 'test@ergonlogic.com'
#  $aegir_makefile = 'aegir.make'

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
