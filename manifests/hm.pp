# A Puppet manifest to provision an Aegir Hostmaster server

file { '/etc/motd':
  content => "Welcome to your Aegir Hostmaster virtual machine!
              Built by Vagrant. Managed by Puppet.\n
              Developed and maintained by Ergon Logic Enterprises.\n"
}

include aegir #apt

class {'aegir::sources': }

#class {'aegir::dependencies': }

#class {'aegir::install': }

# dependencies
package {'drush':
  ensure => '4.5-2~bpo60+1',
}

package {'drush-make': 
  ensure => present,
  require => [
    Package['drush'],
    Class['apt::update'],
  ],
}

package {'mysql-server' :
  ensure => present,
  responsefile => '/tmp/vagrant-puppet/manifests/files/mysql-server.preseed',
}
package {'postfix' :
  ensure => present,
  responsefile => '/tmp/vagrant-puppet/manifests/files/postfix.preseed',
}

package { 'aegir':
  ensure => present,
  responsefile => '/tmp/vagrant-puppet/manifests/files/aegir.preseed',
  require => [
    Package['mysql-server'],
    Package['postfix'],
    Package['drush-make'],
    Class['apt::update'],
  ],
}


