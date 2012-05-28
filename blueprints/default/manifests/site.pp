# A Puppet manifest to provision a default server

Exec { path  => [ "/bin/", "/sbin/" , "/usr/bin/", "/usr/sbin/" ] }

include drush-vagrant::user
