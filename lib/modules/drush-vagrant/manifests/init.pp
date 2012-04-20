class drush-vagrant::user {

  group { 'puppet': ensure => present, }

  user {"${drush_vagrant_username}":
    ensure => present,
    groups => 'sudo',
    home   => "/home/${drush_vagrant_username}",
    shell  => '/bin/bash',
  }

  # Various dotfiles
  File { ensure => present,
         owner  => "${drush_vagrant_username}",
         group  => "${drush_vagrant_username}",
  }
  file { "/home/${drush_vagrant_username}":
           ensure => directory,
           before => [ File['.profile'], File['.bashrc'], File['.bash_aliases'], File['.vimrc'], File["/home/${drush_vagrant_username}/.ssh"] ];
         "/home/${drush_vagrant_username}/.ssh":
           ensure => directory,
           before => File['.ssh/authorized_keys'];
         ".profile":
           source => "/vagrant/.config/files/.profile",
           path   => "/home/${drush_vagrant_username}/.profile";
         ".bashrc":
           source => "/vagrant/.config/files/.bashrc",
           path   => "/home/${drush_vagrant_username}/.bashrc";
         ".bash_aliases":
           source => "/vagrant/.config/files/.bash_aliases",
           path   => "/home/${drush_vagrant_username}/.bash_aliases";
         ".vimrc":
           source => "/vagrant/.config/files/.vimrc",
           path   => "/home/${drush_vagrant_username}/.vimrc";
         ".ssh/authorized_keys":
           source => "/vagrant/.config/files/authorized_keys",
           path   => "/home/${drush_vagrant_username}/.ssh/authorized_keys";
  }

  #git username & email
  package {'git-core':
    ensure => present,
  }
  
  Exec { user        => "${drush_vagrant_username}",
         group       => "${drush_vagrant_username}",
         environment => "HOME=/home/${drush_vagrant_username}",
         path        => '/usr/bin',
         require     => Package['git-core'],
  }
  if $drush_vagrant_git_name {
    exec {"git config --global user.name '${drush_vagrant_git_name}'":}
  }
  if $drush_vagrant_git_email {
    exec {"git config --global user.email ${drush_vagrant_git_email}":}
  }

}
