# A minimal Puppet manifest to provision an Aegir Hostmaster server

notice("\n
        Running Puppet manifests to install Aegir.\n")

Exec { path => '/usr/bin',  }

import "common"

include aegir

include aegir::queue_runner

group { 'puppet': ensure => present, }

file { '/etc/motd':
  content => "\n
              Welcome to your Aegir Hostmaster virtual machine!\n
              Built with Vagrant & Veewee. Managed with Puppet.\n
              Developed and maintained by Ergon Logic Enterprises.\n"
}
