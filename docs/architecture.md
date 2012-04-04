DESCRIPTION
-----------

Aegir-up is a system that applies templates to build Vagrant environments in
VirtualBox. It is implemented as a Drush extension.


STRUCTURE
---------

  aegirup.drush.inc
  aegirup.drush.load.inc
  docs/
  lib/
    definitions/          Veewee base box definitions
      debian/             Bare-bones, but current, Debian
      debian-LAMP/        Debian with a basic LAMP-stack pre-installed
      debian-LAMP-i386/   Identical to 'debian-LAMP', but 32-bit
    modules/              Public Puppet modules (included via subtrees)
      aegir/              Aegir module
      drush/              Drush module
      common/             Rise-up shared common module
      apt/                Koumbit Apt module
      aegir-up/           Extra-config specific to Aegir-up
    scripts/              Utility scrips (deprecated)
    blueprints/           Templates from which to create user projects
      default/            The default blueprint
      aegir-dev/          Build Aegir from git repos
    Vagrantfile           Vagrant control file
  LICENSE.txt
  README.md
