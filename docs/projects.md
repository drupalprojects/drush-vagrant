DESCRIPTION
-----------

Each project in Vagrant represents one or more VMs that share a common subnet,
Vagrantfile, Puppet modules folder, &c.


STRUCTURE
---------

  <my_project>/
    manifests/         Puppet manifests
      nodes.pp         Manifest defining individual VMs (nodes)
      site.pp          Principle control manifest; includes/runs others
    modules/           Project-specific Puppet modules
      ...              As required; can include custom modules
    settings.rb        Parameters for VMs
    Vagrantfile        symlinked to <drush-vagrant-root>/lib/Vagrantfile
    .config/           Folder containing generated configuration
      files/           User dotfiles & public ssh key that get added to VMs
      config.rb        Configuration generated during project initialization


VAGRANTFILE
-----------


SETTINGS.RB
-----------

