# Download Aegir-up locally
git clone http://git.drupal.org/project/aegir-up.git

# Install LAMP stack
puppet apply ./aegir-up/lib/definitions/debian-LAMP/debian-LAMP.pp --modulepath="./aegir-up/lib/modules"

exit
