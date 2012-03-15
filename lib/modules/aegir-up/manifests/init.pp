# A Puppet module to add some Aegir-up-specific config

class aegir-up {
  notice("\n
          Running Puppet manifests to install and/or update Aegir.\n
          This may take awhile, so please be patient.
          For more detail on the operations being run, edit settings.rb,
          and set 'verbose = 1'.")

  group { 'puppet': ensure => present, }

  # Set some defaults, and make output less verbose
  Group { loglevel => 'info', }
  Package { loglevel => 'info', }
  Notify { loglevel => 'info', }
  User { loglevel => 'info', }
  Exec { path  => [ "/bin/", "/sbin/" , "/usr/bin/", "/usr/sbin/" ], loglevel => 'info', }
  File { owner => 0, group => 0, mode => 0644, loglevel => 'info', }

  file { '/etc/motd':
    content => "\n
                Welcome to your Aegir Hostmaster virtual machine!\n
                Built by Vagrant. Managed by Puppet.\n
                Developed and maintained by Ergon Logic Enterprises.\n"
  }

  import "common"

  include aegir-up::user

  include aegir

  include aegir::queue_runner

}
