# Download our LAMP Puppet manifest locally
wget http://drupalcode.org/project/drush-vagrant.git/blob_plain/refs/heads/7.x-3.x:/lib/veewee/definitions/debian7-LAMP-i386/debian-LAMP.pp

# Install LAMP stack
puppet apply ./debian-LAMP.pp

# Clean up
rm ./debian-LAMP.pp

exit
