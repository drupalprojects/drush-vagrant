define aegir::platform ($makefile, $force_complete = false, $working_copy = false) {

  if ! $aegir_root { $aegir_root = '/var/aegir' }
  if ! $aegir_user { $aegir_user = 'aegir' }

  include aegir::defaults

  exec {"provision-save-${name}":
    command => "drush --root=${aegir_root}/platforms/${name} --context_type='platform' --makefile='${makefile}' provision-save @platform_${name}",
    creates => "${aegir_root}/.drush/platform_${name}.alias.drushrc.php",
    require => [ Class['aegir::backend'],
                 #TODO: This shouldn't require the front-end, but fails if run before the front-end is installed
                 Class['aegir::frontend'],
               ],
    notify  => Exec["hosting-import-${name}"],
  }

  if ($force_complete or $working_copy) {
    if $force_complete { $force_opt = ' --force-complete' }
    if $working_copy { $working_opt = ' --working-copy' }

    # we need to run drush make (and not verify) in order to override
    # the drush make settings, because provision-verify won't take the
    # --working-copy or --force-complete settings and pass them to
    # drush make. hosting-import (below) will make the frontend run
    # provision-verify through the queue eventually anyways.    
    exec {"drush make ${name}":
      command => "drush make $makefile ${name} $force_opt $working_opt",
      creates => "${aegir_root}/platforms/${name}",
      cwd     => "${aegir_root}/platforms",
      require => Exec["provision-save-${name}"],
      notify  => Exec["hosting-import-${name}"],
    }

  }

  exec {"hosting-import-${name}":
    command => "drush @hostmaster hosting-import @platform_${name}",
    require => [ Exec["provision-save-${name}"], 
                 Class['aegir::frontend'],
               ],
    refreshonly => true,
  }
                          
}
