class drush-vagrant::users {

  group { 'puppet': ensure => present, }

  drush-vagrant::user { [$username, 'vagrant']: }
  drush-vagrant::user {'root':
    home_dir => '/root',
  }

}

define drush-vagrant::user ($home_dir = '') {
  # We can't retrieve the resource name in the paramaters themselves to define
  # a default parameter. That is, the following doesn't work, despite indica-
  # tions that it should in the Puppet docs:
  # define drush-vagrant::user ($home_dir = "/home/$name")
  # define drush-vagrant::user ($home_dir = "/home/$title")
  # ... So, we use a wrapper definition, and then pass along the $name in the home_dir, if required.
  if ($home_dir == '') {
    $home = "/home/${name}"
  }
  else {
    $home = $home_dir
  }
  drush-vagrant::user_account {$name:
    home_dir => $home,
  }
}

define drush-vagrant::user_account ($home_dir) {

  User { ensure => present,
         groups => 'sudo',
         shell  => '/bin/bash',
  }

  if !defined(User[$name]) {
    user {$name:
      home   => $home_dir,
    }
  }

  # Various dotfiles
  File { ensure => present,
         owner  => $name,
         group  => $name,
  }
  if !defined(File[$home_dir]) {
    file { $home_dir:
             ensure => directory,
             before => [
               File["${home_dir}/.profile"],
               File["${home_dir}/.bashrc"],
               File["${home_dir}/.bash_aliases"],
               File["${home_dir}/.vimrc"],
               File["${home_dir}/.ssh"]
             ];
    }
  }
  if !defined(File["${home_dir}/.ssh"]) {
    file { "${home_dir}/.ssh":
      ensure => directory;
    }
  }
  file { "${home_dir}/.profile":
           source => ["/vagrant/.config/files/.profile",
                      "puppet:///modules/drush-vagrant/profile.example"];
         "${home_dir}/.bashrc":
           source => ["/vagrant/.config/files/.bashrc",
                      "puppet:///modules/drush-vagrant/bashrc.example"];
         "${home_dir}/.bash_aliases":
           source => ["/vagrant/.config/files/.bash_aliases",
                      "puppet:///modules/drush-vagrant/bash_aliases.example"];
         "${home_dir}/.vimrc":
           source => ["/vagrant/.config/files/.vimrc",
                      "puppet:///modules/drush-vagrant/vimrc.example"];
  }

  if $name != 'vagrant' {
    file { "${home_dir}/.ssh/authorized_keys":
             source  => "/vagrant/.config/files/authorized_keys",
             require => File["${home_dir}/.ssh"];
    }
  }

  #git username & email
  include git

  Exec { user        => $name,
         group       => $name,
         environment => "HOME=${home_dir}",
         path        => '/usr/bin',
         require     => Class['git'],
  }
  if $git_name {
    exec {"git user.name config for ${name}":
      command => "git config --global user.name '${git_name}'",
    }
  }
  if $git_email {
    exec {"git user.email config for ${name}":
      command => "git config --global user.email ${git_email}",
    }
  }

}
