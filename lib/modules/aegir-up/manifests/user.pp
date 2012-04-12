class aegir-up::user {

  user {"${aegir_up_username}":
    ensure => present,
    groups => 'sudo',
    home   => "/home/${aegir_up_username}",
    shell  => '/bin/bash',
  }

  # Various dotfiles
  File { ensure => present,
         owner  => "${aegir_up_username}",
         group  => "${aegir_up_username}",
  }
  file { "/home/${aegir_up_username}":
           ensure => directory,
           before => [ File['.profile'], File['.bashrc'], File['.bash_aliases'], File['.vimrc'], File["/home/${aegir_up_username}/.ssh"] ];
         "/home/${aegir_up_username}/.ssh":
           ensure => directory,
           before => File['.ssh/authorized_keys'];
         ".profile":
           source => "/vagrant/.config/files/.profile",
           path   => "/home/${aegir_up_username}/.profile";
         ".bashrc":
           source => "/vagrant/.config/files/.bashrc",
           path   => "/home/${aegir_up_username}/.bashrc";
         ".bash_aliases":
           source => "/vagrant/.config/files/.bash_aliases",
           path   => "/home/${aegir_up_username}/.bash_aliases";
         ".vimrc":
           source => "/vagrant/.config/files/.vimrc",
           path   => "/home/${aegir_up_username}/.vimrc";
         ".ssh/authorized_keys":
           source => "/vagrant/.config/files/authorized_keys",
           path   => "/home/${aegir_up_username}/.ssh/authorized_keys";
  }

  #git username & email
  Exec { user  =>       "${aegir_up_username}",
         group =>       "${aegir_up_username}",
         environment => "HOME=/home/${aegir_up_username}",
         path =>        '/usr/bin',
  }
  if $aegir_up_git_name {
    exec {"git config --global user.name '${aegir_up_git_name}'":}
  }
  if $aegir_up_git_email {
    exec {"git config --global user.email ${aegir_up_git_email}":}
  }

}
