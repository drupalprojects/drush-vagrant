DESCRIPTION
-----------

Aegir-up deploys a local instance of the Aegir Hosting System atop Vagrant and
Virtualbox, for development and testing purposes. It provides all the ease-of-
use of Aegir, wrapped in the convenience of Vagrant-based virtual machines.
Creating sites and platforms is a matter of a click or two, while rebuilding
the entire environment is a matter of minutes.

Aegir-up provides a collaborative, distributed development environment, that
encourages the use of Drupal development best-practices. Workspaces can easily
be cloned and shared via Git, or even bundled into blueprints that others can
use as templates for their own development environments.

One of Aegir-up's more powerful features is that Aegir's entire home directory
is mounted locally via NFS. This includes the platforms directory, within which
sites are installed. So you can edit any file on your site or platform directly
instead of having to SSH into the VM, or move them via SFTP.


DEPENDENCIES
------------

Aegir-up depends on Drush and Vagrant (which in turn depends on VirtualBox,
and possibly Ruby and RubyGems), along with NFS.

Vagrant 1.0+ is required, and installation instructions can be found at:
http://vagrantup.com/docs/getting-started/index.html

A recent version of VirtualBox (4.0+) is required by Vagrant, and can be
downloaded from: https://www.virtualbox.org/wiki/Downloads.

Drush 4.x is required (and 5.x support is planned). Drush can be installed in a
number of ways, as detailed here:
http://drupalcode.org/project/drush.git/blob/HEAD:/README.txt#l30

NFS (Network File System) is pre-installed on many, if not most Unix-like OSes.
If it isn't already, a recent version should be easy to install using your OS's
preferred packaging method.

Compatible releases of Vagrant, VirtualBox and Drush are available in Debian's
Testing branch, and so can be installed (along with all dependencies) with a
simple 'apt-get install vagrant', assuming the appropriate repos have been
added to /etc/apt/source.list.d/
This last is the developers' preferred and recommended installation method.

Finally, a custom Vagrant base box is required, and will automatically be
downloaded upon first use of Aegir-up. This base box is built with a tool
called veewee (https://github.com/jedi4ever/veewee), and all definitions needed
to build this base box are included with the Aegir-up package. Note that
building a base box is not required for the use of Aegir-up.


INSTALLATION
------------

Installation should be as simple as:

  drush dl aegir-up

This should create a folder at ~/.drush/aegir-up, but might also be installed
to /path/to/drush/commands/aegir-up, depending on how Drush was installed.

When Aegir-up is first initialized, user settings are automatically saved to
~/.drushrc.php, and a folder for workspace created (by default) at ~/aegir-up.
User settings can be modified by running 'drush aegir-up-user' (or 'drush auu')
at any time.

Now that Vagrant is available in Debian (Testing), .deb packaging is planned.


USAGE
-----

Usage documentation is in Drush's built-in help system. To see a list of
Aegir-up commands, you can run the following:

  drush --filter=aegirup

More detailed usage information is provided by running:

  drush <command> --help


OTHER DOCS & HELP
-----------------

More detailed documentation can be found in the docs/ folder, which are also
available as Drush topics:

  drush topic

Additional documentation, including user-contributed guides can be found on the
wiki at: http://community.openatria.com/c/aegir-up

Bug reports, feature and support requests should be submitted to the drupal.org
issue queue: http://drupal.org/project/issues/aegir-up

Also, the developers (See Credits section, below) can usually be found on IRC
in the #aegir and #openatria channels on irc.freenode.net


CAVEAT
------

**N.B.** Aegir-up is *NOT* intended for production hosting.

While we hope you find Aegir-up useful for development and testing, out-of-the-
box it is not equiped for production use (i.e., minimal or no security,
monitoring, backup/restore facilities, &c.)

For fully managed, production-grade Aegir servers and services, check out
Koumbit's AegirVPS services:
  <http://www.koumbit.org/en/services/AegirVPS>

Or other Aegir Service Providers:
  <http://community.aegirproject.org/service-providers>


CREDITS
-------

Developed by Christopher Gervais (aka ergonlogic) <http://ergonlogic.com/>
Maintained by C. Gervais and Guillaume Boudrias <http://openatria.com/>
