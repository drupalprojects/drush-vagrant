DEPENDENCIES
------------

Drush Vagrant Integration (obviously) depends on Drush and Vagrant (which in
turn depends on VirtualBox, and possibly Ruby and RubyGems). Some features also
require an NFS server.

Vagrant 1.0+ is required, and installation instructions can be found at:
http://vagrantup.com/v1/docs/getting-started/index.html

A recent version of VirtualBox (4.0+) is required by Vagrant, and can be
downloaded from: https://www.virtualbox.org/wiki/Downloads.

Drush 5.x or greater is required. Drush can be installed in a number of ways,
as detailed here:
http://drupalcode.org/project/drush.git/blob/HEAD:/README.txt#l30. Drush Hosts
(http://drupal.org/project/drush-hosts) is also required in order to manage
entries in /etc/hosts during project builds and deletion.

Compatible releases of Vagrant, VirtualBox and Drush are available in Debian's
Testing branch, and so can be installed (along with all dependencies) with a
simple 'apt-get install vagrant drush', assuming the appropriate repos have
been added to /etc/apt/source.list.d/ This last is the developers' preferred
and recommended installation method.

In order to take advantage of Vagrant's ability to mount NFS (Network File
System) shares, an NFS-server is required. This allows the sharing of entire
directory trees transparently between the host machine and the VM. If it isn't
already, a recent version should be easy to install using your OS's preferred
packaging method. For Debian-like systems, this means something like: 'apt-get
install nfs-kernel-server'.

Finally, some blueprints may require a custom Vagrant base box. This will be
automatically downloaded upon first use of that blueprint. Base boxes are
usually built with a Vagrant plugin called veewee (https://github.com/jedi4ever/veewee),
and all definitions needed to build a base box should be included within the
blueprint's project. Note that building a base box is not required for the use
of Drush Vagrant.


INSTALLATION
------------

Installation should be as simple as:

  drush dl drush-vagrant drush-hosts

This should create a folder at ~/.drush/drush-vagrant, but might also be
installed to /path/to/drush/commands/drush-vagrant, depending on how Drush was
itself installed.

Permission issues during installation can be overcome by running the command as
root:

  sudo drush dl drush-vagrant drush-hosts

Or downloading it to your personal .drush folder:

  cd ~
  mkdir .drush
  drush dl drush-vagrant drush-hosts --destination=./.drush

When a Drush Vagrant project is first initialized, user settings are
automatically saved to ~/.drushrc.php. By default, it will use the Vagrant
'projects' folder usually located at ~/vagrant/projects. User settings can be
modified by running 'drush vagrant-user' (or 'drush vuser') at any time.

Now that Vagrant is available in Debian (Testing), .deb packaging is planned,
to further simplify installation.

