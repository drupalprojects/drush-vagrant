# User-specific settings. Run aegir-up-user.sh to initalize these settings
# globally.

#  $aegir_up_username = 'username'
#  $aegir_up_git_name = 'Firstname Lastname'
#  $aegir_up_git_email = 'username@example.com'

if $aegir_up_username {
  include aegir-up::user
}
