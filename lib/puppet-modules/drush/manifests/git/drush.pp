class drush::git::drush (
  $git_branch = '',
  $git_tag    = '',
  $git_repo   = 'https://github.com/drush-ops/drush.git',
  $update     = false
  ) inherits drush::defaults {

  Exec { path    => ['/bin', '/usr/bin', '/usr/local/bin', '/usr/share'], }

  if !defined(Package['git']) and !defined(Package['git-core']) {
    package { 'git': ensure => present, before => Drush::Git[$git_repo]}
  }

  if !defined(Package['php5-cli']) {
    package { 'php5-cli': ensure => present, }
  }

  drush::git { $git_repo :
    path       => '/usr/share',
    git_branch => $git_branch,
    git_tag    => $git_tag,
    update     => $update,
  }

  file {'symlink drush':
    ensure  => link,
    path    => '/usr/bin/drush',
    target  => '/usr/share/drush/drush',
    require => Drush::Git[$git_repo],
    notify  => Exec['first drush run'],
  }

  exec {'Install composer' :
    command => 'curl -sS https://getcomposer.org/installer | php',
    require => Package['php5-cli'],
  }

  exec {'Make Composer globally executable' :
    command => 'mv composer.phar /usr/local/bin/composer',
    require => Exec['Install composer'],
    before  => Exec['Install Drush dependencies'],
  }

  exec {'Install Drush dependencies' :
    command => 'composer install',
    cwd     => '/usr/share/drush',
  }

  # Needed to download a Pear library
  exec {'first drush run':
    command     => 'drush cache-clear drush',
    refreshonly => true,
    require     => [
      File['symlink drush'],
      Package['php5-cli'],
      Exec['Install Drush dependencies'],
    ],
  }

}
