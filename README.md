Aegir-up
========

Aegir-up deploys a local instance of the Aegir Hosting System atop Vagrant and
Virtualbox, for development and testing purposes.

**N.B.** Aegir-up is *NOT* intended for production hosting. For a fully managed
production-grade Aegir server check out Koumbit Networks' AegirVPS services
(http://www.koumbit.org/en/services/AegirVPS), or other Aegir Service Providers
(http://community.aegirproject.org/service-providers).


Quick start
-----------

Assuming all the dependencies are installed, you can get started by:

    $ ./aegir-up-init my_project
    ...
    $ cd projects/my_project
    $ vagrant up


Installation
------------

See http://community.openatria.com/team/aegir-up/installation


Usage
-----

See http://community.openatria.com/team/aegir-up/usage


Building base boxes
-------------------

See http://community.openatria.com/team/aegir-up/base-boxes


Roadmap
-------

* Enable several Aegir modules:
  * DNS
  * Clone
  * Migrate
* Install several aegir contrib modules:
  * Hosting queue runner (w/supervisord)
  * Hosting backup queue
  * Backup queue garbage collection
  * Hosting site git
  * Hosting platform pathauto
  * Hosting upload
  * Provision CiviCRM
  * Project status
  * xhprof drush extension
* Symlink /backups and /makefiles into /vagrant so they persist after 'vagrant
destroy's.
* Add several other packages
  * vim
    * Drupal syntax highlighting
    * exuberant ctags
  * phpmyadmin/chive
  * xdebug
  * apc
  * virtualbox-utils & dkms/source
* Optionally install Aegir & Drush from git repos
* Support dog
* Strip out extraneous packages and kernel modules (sound, bluetooth, etc)
* Speed up grub
* Squid cache for drupal.org projects
* Automatically create one or more platforms (makefiles.d/?)
* Figure out an easy way to push/pull sites to/from remote Aegir servers
* Customize user login
  * Use user's username (Say that 3 time fast!)
  * Copy ~/.ssh into authorized_keys
  * Copy ~/.[vimrc|bashrc] and provide defaults (for root)
* Add scripts for easy updating, etc.
* Add support for local overrides that won;t be squashed by git pulls
