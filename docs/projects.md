DESCRIPTION
-----------

Each project in Vagrant represents one or more VMs that share a common subnet,
Vagrantfile, Puppet modules folder, &c.


STRUCTURE
---------

  <my_project>/        Project that will run an Aegir server
    makefiles/         Drush makefiles for building platforms in Aegir
    manifests/         Puppet manifests defining each VM
    modules/           Project-specific Puppet modules
    settings.rb        Parameters for VMs
    Vagrantfile        symlinked to <aegir-up-root>/lib/blueprints/Vagrantfile
    .config/           Folder containing generated configuration
      files/           User dotfiles & public ssh key that get added to VMs
      config.rb        Configuration generated during project initialization


VAGRANTFILE
-----------


SETTINGS.RB
-----------

