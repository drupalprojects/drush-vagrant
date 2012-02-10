
class aegir ($site = undef) {
  Exec {
    path  => [ "/bin/", "/sbin/" , "/usr/bin/", "/usr/sbin/" ],
#  user  => 'aegir',
#  group => 'aegir',
  }

 class { 'aegir::frontend':
    site => $site,
  }
}

class aegir::frontend ($site = undef) {
  include aegir::backend

  if $site {
    exec {'debconf aegir/site':
      command => "echo debconf aegir/site string $site | debconf-set-selections",
      before => Package['aegir'],
    }
  }

  package { 'aegir':
    ensure       => present,
    responsefile => 'files/aegir.preseed',
    require      => Apt::Sources_list['aegir-stable'],
  }
}

class aegir::backend {
  include drush
  include aegir::apt

  package { 'aegir-provision':
    ensure  => present,
    require => [
      Apt::Sources_list['aegir-stable'], 
      Package['drush'],
      ]
  }
}


class aegir::apt {
  include apt

  apt::sources_list { "aegir-stable":
    content => "deb http://debian.aegirproject.org stable main",
    require => Apt::Keys::Key['aegir'],
  }
  apt::keys::key { "aegir": source => "puppet:///modules/aegir/debian.aegirproject.org.key" }
}
