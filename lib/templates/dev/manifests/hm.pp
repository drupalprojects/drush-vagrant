# A Puppet manifest to provision an Aegir Hostmaster server

# Optional settings for Aegir front-end
  # For now it's best to specify the front-end URL here, though this should no
  # longer be necessary once http://drupal.org/node/1459126 lands
  $aegir_hostmaster_url = 'aegir.local'
#  $aegir_db_host = 'db.aegir.local'
#  $aegir_db_user = 'root'
#  $aegir_db_password = 'password'
#  $aegir_email = 'test@ergonlogic.com'
  $aegir_makefile = '/vagrant/makefiles/custom-aegir.make'
#  $aegir_force_login_link = 'true'    # Print a login link each time the manifest is run

# Additional optional settings available since $aegir_dev_build = TRUE

#  $aegir_user = 'aegir'
#  $aegir_root = '/var/aegir'
#  $aegir_version = '6.x-1.6'
#  $aegir_drush_make_version = '6.x-2.3'
#  $aegir_http_service_type = 'apache' 
#  $aegir_web_group = 'www-data'

# Build 'manually' using git repos
  $aegir_dev_build = true
  $aegir_provision_repo = 'http://git.drupal.org/project/provision.git'
  $aegir_provision_branch = '6.x-1.x'

# Include blocks like the following to automatically build platforms
/*
aegir::platform {'Open_Atrium':
  makefile       => 'http://drupalcode.org/project/openatria_makefiles.git/blob_plain/refs/heads/master:/stub-openatrium.make',
  force_complete => true,
}
*/

include aegir-up
