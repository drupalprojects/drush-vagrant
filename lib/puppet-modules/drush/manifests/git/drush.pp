class drush::git::drush (
  $git_branch = '',
  $git_tag    = '',
  $git_repo   = 'https://github.com/drush-ops/drush.git',
  $update     = false
  ) inherits drush::defaults {

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

  # Needed to download a Pear library
  exec {'first drush run':
    command     => '/usr/bin/drush status',
    refreshonly => true,
    require     => File['symlink drush'],
  }

}
