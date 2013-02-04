DESCRIPTION
-----------

Drush Vagrant Integration provides Drush commands to accomplish common Vagrant
tasks, and provides a powerful templating framework ('blueprints'). It also has
tools to implement Drush aliases for Vagrant projects and VMs, thus allowing
simpler remote control of Vagrant projects.

Drush Vagrant is intended to simplify building collaborative, distributed
development and testing environments, that encourage the use of Drupal best-
practices. Projects can easily be cloned and shared via Git, or even bundled
into blueprints that others can use as templates for their own development
environments.

While initially developed (via the Aegir-up project) as a Drupal and Aegir
development tool, Drush Vagrant itself is not Drupal-specific. It provides
generally useful wrappers around Vagrant, and can thus assist in building and
testing non-Drupal projects, such as Puppet modules.


USAGE
-----

Usage documentation is in Drush's built-in help system. To see a list of
Drush Vagrant commands, you can run the following:

  drush --filter=vagrant

More detailed usage information is provided by running:

  drush <command> --help


QUICK START
-----------

Build a new VM:
  drush vagrant-build

Change into the new vagrant project directory:
  cd ~/vagrant/project/new-vagrant-project

SSH into the new VM:
  drush vagrant-shell


NEXT STEPS
----------

By itself, Drush-vagrant will only provide a 'default' blueprint. You'll proba-
bly be interested in checking out these blueprints:

http://drupal.org/project/drupal-up  (drush dl drupal-up)
http://drupal.org/project/aegir-up   (drush dl aegir-up)

OTHER DOCS & HELP
-----------------

More detailed documentation can be found in the docs/ folder, which are also
available as Drush topics:

  drush topic

Bug reports, feature and support requests should be submitted to the drupal.org
issue queue: http://drupal.org/project/issues/drush-vagrant

Also, the developers (See Credits section, below) can usually be found on IRC
in the #aegir and #openatria channels on irc.freenode.net.


CREDITS
-------

Originally developed by Steven Merrill <http://www.stevenwmerrill.com/>
The 2.x branch was developed by Christopher Gervais <http://ergonlogic.com/>
and is maintained by Christopher Gervais and Herman Van Rink (helmo)
