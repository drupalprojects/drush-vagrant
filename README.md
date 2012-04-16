DESCRIPTION
-----------

Drush Vagrant Integration provides Drush commands to accomplish common Vagrant
tasks, and enables a powerful templating framework ('blueprints'). It simpli-
fies building collaborative, distributed development and testing environments,
that encourage the use of Drupal development best-practices. Workspaces can
easily be cloned and shared via Git, or even bundled into blueprints that
others can use as templates for their own development environments.


DEPENDENCIES
------------

Drush Vagrant Integration (obviously) depends on Drush and Vagrant (which in
turn depends on VirtualBox, and possibly Ruby and RubyGems). Some features also
require an NFS server.

Vagrant 1.0+ is required, and installation instructions can be found at:
http://vagrantup.com/docs/getting-started/index.html

A recent version of VirtualBox (4.0+) is required by Vagrant, and can be
downloaded from: https://www.virtualbox.org/wiki/Downloads.

Drush 4.x is required (and 5.x support is planned). Drush can be installed in a
number of ways, as detailed here:
http://drupalcode.org/project/drush.git/blob/HEAD:/README.txt#l30

Compatible releases of Vagrant, VirtualBox and Drush are available in Debian's
Testing branch, and so can be installed (along with all dependencies) with a
simple 'apt-get install vagrant', assuming the appropriate repos have been
added to /etc/apt/source.list.d/ This last is the developers' preferred and
recommended installation method.

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

  drush dl drush-vagrant

This should create a folder at ~/.drush/drush-vagrant, but might also be
installed to /path/to/drush/commands/aegir-up, depending on how Drush was
itself installed.

Permission issues during installation can be overcome by running the command as
root:

  sudo drush dl drush-vagrant

Or downloading it to your personal .drush folder:

  cd ~
  mkdir .drush
  drush dl drush-vagrant --destination=./.drush

When a Drush Vagrant workspace is first initialized, user settings are
automatically saved to ~/.drushrc.php, and a folder for workspace created (by
default) at ~/vagrant. User settings can be modified by running 'drush
vagrant-user' (or 'drush vuser') at any time.

Now that Vagrant is available in Debian (Testing), .deb packaging is planned,
to further simplify installation.


USAGE
-----

Usage documentation is in Drush's built-in help system. To see a list of
Drush Vagrant commands, you can run the following:

  drush --filter=vagrant

More detailed usage information is provided by running:

  drush <command> --help


OTHER DOCS & HELP
-----------------

More detailed documentation can be found in the docs/ folder, which are also
available as Drush topics:

  drush topic

Bug reports, feature and support requests should be submitted to the drupal.org
issue queue: http://drupal.org/project/issues/drush-vagrant

Also, the developers (See Credits section, below) can usually be found on IRC
in the #aegir and #openatria channels on irc.freenode.net


CREDITS
-------

Originally developed by Steven Merrill <http://www.stevenwmerrill.com/>
The 2.x branch is co-maintained by Christopher Gervais <http://ergonlogic.com/>