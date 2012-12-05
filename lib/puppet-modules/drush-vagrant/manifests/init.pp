class drush-vagrant::user ($user = [
  {'name' => $username, 'home_dir' => "/home/$username"},
  {'name' => 'root',    'home_dir' => '/root'},
  {'name' => 'vagrant', 'home_dir' => '/home/vagrant'},
  {'name' => 'aegir',   'home_dir' => '/var/aegir'},
  ]) {

  group { 'puppet': ensure => present, }

  drush-vagrant::user_account {$user[0]['name']:
    username => $user[0]['name'],
    home_dir => $user[0]['home_dir'],
  }
  drush-vagrant::user_account {$user[1]['name']:
    username => $user[1]['name'],
    home_dir => $user[1]['home_dir'],
  }
  drush-vagrant::user_account {$user[2]['name']:
    username => $user[2]['name'],
    home_dir => $user[2]['home_dir'],
  }
  drush-vagrant::user_account {$user[3]['name']:
    username => $user[3]['name'],
    home_dir => $user[3]['home_dir'],
  }

}

define drush-vagrant::user_account ($username, $home_dir = "/home/$username") {

  User { ensure => present,
         groups => 'sudo',
         shell  => '/bin/bash',
  }

  if !defined(User[$username]) {
    user {$username:
      home   => $home_dir,
    }
  }

  # Various dotfiles
  File { ensure => present,
         owner  => "${username}",
         group  => "${username}",
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
  file { "${home_dir}/.ssh":
           ensure => directory;
         "${home_dir}/.profile":
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

  if $username != 'vagrant' {
    file { "${home_dir}/.ssh/authorized_keys":
             source  => "/vagrant/.config/files/authorized_keys",
             require => File["${home_dir}/.ssh"];
    }
  }

  #git username & email
  include git

  Exec { user        => $username,
         group       => $username,
         environment => "HOME=${home_dir}",
         path        => '/usr/bin',
         require     => Class['git'],
  }
  if $git_name {
    exec {"git user.name config for $username":
      command => "git config --global user.name '${git_name}'",
    }
  }
  if $git_email {
    exec {"git user.email config for $username":
      command => "git config --global user.email ${git_email}",
    }
  }

}
