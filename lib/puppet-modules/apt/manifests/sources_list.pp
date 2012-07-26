define apt::sources_list (
  $ensure = 'present',
  $source = '',
  $content = undef )
{

  if $source == '' and $content == undef {
    fail("One of \$source or \$content must be specified for apt_sources_snippet ${name}")
  }
  if $source != '' and $content != undef {
    fail("Only one of \$source or \$content must specified for apt_sources_snippet ${name}")
  }

  file { "/etc/apt/sources.list.d/${name}.list":
    ensure => $ensure,
    owner => root, group => 0, mode => 0644,
    notify => Exec['update_apt'];
  }

  if $source {
    File["/etc/apt/sources.list.d/${name}.list"] {
      source => $source,
    }
  }
  else {
    File["/etc/apt/sources.list.d/${name}.list"] {
      content => $content,
    }
  }
}

