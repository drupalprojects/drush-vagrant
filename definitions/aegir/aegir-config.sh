# Install git
apt-get install git-core -y

# Download Aegir-up locally
git clone http://git.drupal.org/project/aegir-up.git

# Install Aegir
puppet apply ./aegir-up/definitions/aegir/aegir.pp --modulepath="./aegir-up/modules"

# Clean up
rm -rf ./aegir-up

exit
