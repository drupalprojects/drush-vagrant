class drush-vagrant::users ($users = '') {

  if $users != '' {
    drush-vagrant::user { [$users]: }
  }
  drush-vagrant::user {'root': home_dir => '/root', }

}
