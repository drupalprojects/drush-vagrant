# A Puppet manifest to provision an Aegir Hostmaster server

# Optional settings for Aegir front-end
  # For now it's best to specify the front-end URL here, though this should no
  # longer be necessary once http://drupal.org/node/1459126 lands
  $aegir_hostmaster_url = 'aegir.local'
#  $aegir_db_host = 'db.aegir.local'
#  $aegir_db_user = 'root'
#  $aegir_db_password = 'password'
#  $aegir_email = 'test@ergonlogic.com'
#  $aegir_makefile = 'aegir.make'
#  $aegir_force_login_link = 'true'    # Print a login link each time the manifest is run

# Build 'manually' instead of from packages (.debs, &c.)
#  $aegir_manual_build = true

# Additional optional settings available if $aegir_manual_build = TRUE
#
# WARNING! Only change these if you really know what you are doing, and even
# then, think twice. Changing these can result in a broken and/or unusable
# Aegir installation.
#
#  $aegir_user = 'aegir'
#  $aegir_root = '/var/aegir'
#  $aegir_version = '6.x-1.6'
#  $aegir_drush_make_version = '6.x-2.3'
#  $aegir_http_service_type = 'apache' 
#  $aegir_web_group = 'www-data'

# Build 'manually' using latest git repos
#  $aegir_dev_build = true

# Include blocks like the following to automatically build platforms
/*
aegir::platform {'Open_Atrium':
  makefile       => 'http://drupalcode.org/project/openatria_makefiles.git/blob_plain/refs/heads/master:/stub-openatrium.make',
  force_complete => true,
}
*/

# User-specific settings. Run aegir-up-user.sh to initalize these settings
# globally.
#  $aegir_up_username = 'username'
#  $aegir_up_git_name = 'Firstname Lastname'
#  $aegir_up_git_email = 'username@example.com'

include aegir-up

if $aegir_up_username {
  include aegir-up::user
}
