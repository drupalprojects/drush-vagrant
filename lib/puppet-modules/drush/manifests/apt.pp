class drush::apt ( $dist = 'stable', $backports = false) {

  if $backports {
    file { "/etc/apt/preferences.d/drush-${backports}.pref":
      ensure  => 'present',
      content => "Package: drush\nPin: release a=${backports}-backports\nPin-Priority: 1001\n",
      owner   => root, group => root, mode => '0644',
      notify  => Exec['drush_update_apt'],
    }
    file { "/etc/apt/sources.list.d/drush-${backports}-backports.list" :
      ensure  => 'present',
      content => "deb http://backports.debian.org/debian-backports ${backports}-backports main",
      owner   => root, group => root, mode => '0644',
      notify  => Exec['drush_update_apt'],
    }
  }
  else {
    file { [
      "/etc/apt/preferences.d/drush-${backports}.pref",
      "/etc/apt/sources.list.d/drush-${backports}-backports.list",
    ]:
      ensure => 'absent',
      notify  => Exec['drush_update_apt'],
    }
  }

  file { "/etc/apt/sources.list.d/drush-${dist}.list" :
    ensure  => 'present',
    content => "deb http://http.debian.net/debian ${dist} main",
    owner   => root, group => root, mode => '0644',
    notify  => Exec['drush_update_apt'],
  }

  exec { 'drush_update_apt':
    command     => 'apt-get update',
    path        => [ '/bin/', '/sbin/' , '/usr/bin/', '/usr/sbin/' ],
    refreshonly => true,
  }

  exec { 'drush_apt_update':
    command  => 'apt-get update && /usr/bin/apt-get autoclean',
    path     => [ '/bin/', '/sbin/' , '/usr/bin/', '/usr/sbin/' ],
    require  => File["/etc/apt/sources.list.d/drush-${dist}.list"],
    schedule => daily,
  }

}
