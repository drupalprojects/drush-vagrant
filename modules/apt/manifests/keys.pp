class apt::keys {
  include apt

  include common::moduledir
  $base_dir = "${common::moduledir::module_dir_path}/apt"

  file { "${apt::keys::base_dir}/keys.d":
    ensure => "directory",
    mode => 0755, owner => root, group => root,
  }
}


define apt::keys::key (
  $ensure = 'present',
  $source = '',
  $content = undef )
{
  include apt::keys 

  file { "${apt::keys::base_dir}/keys.d/$name":
    ensure => $ensure,
    owner => root, group => 0, mode => 0644;
  }

  if $source {
    File["${apt::keys::base_dir}/keys.d/${name}"] {
      source => $source,
    }
  }
  else {
    File["${apt::keys::base_dir}/keys.d/${name}"] {
      content => $content,
    }
  }

  exec { "apt_key_add_${name}":
    command => "apt-key add ${apt::keys::base_dir}/keys.d/${name}",
    path => "/bin:/usr/bin:/sbin:/usr/sbin",
    refreshonly => true,
    subscribe => File["${apt::keys::base_dir}/keys.d/${name}"],
    notify => Exec['update_apt'];
  }
}


class apt::keys::dir {
  include apt::keys
  if $custom_key_dir {
    File["${apt::keys::base_dir}/keys.d"] {
      source => "$custom_key_dir",
      recurse => true,
    }
    exec { "custom_keys":
      command => "find ${apt::keys::base_dir}/keys.d -type f -exec apt-key add '{}' \\; && /usr/bin/apt-get update",
      subscribe => File["${apt::keys::base_dir}/keys.d"],
      refreshonly => true,
    }
    if $custom_preferences != false {
      Exec["custom_keys"] {
        before => Concatenated_file[apt_config],
      }
    }
  }

}
