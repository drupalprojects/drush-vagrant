DESCRIPTION
-----------

'Workspaces' in Aegir-up are roughly equivalent to 'projects' in Vagrant. Each
workspace represents one or more VMs that share a common subnet, Vagrantfile,
Puppet modules folder, &c.


STRUCTURE
---------

  <my_workspace>/      Workspace that will run an Aegir server
    makefiles/         Drush makefiles for building platforms in Aegir
    manifests/         Puppet manifests defining each VM
    modules/           Workspace-specific Puppet modules
    settings.rb        Parameters for VMs
    Vagrantfile        symlinked to <aegir-up-root>/lib/blueprints/Vagrantfile
    .config/           Folder containing generated configuration
      files/           User dotfiles & public ssh key that get added to VMs
      config.rb        Configuration generated during workspace initialization


VAGRANTFILE
-----------


SETTINGS.RB
-----------

