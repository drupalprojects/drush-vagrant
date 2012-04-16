DESCRIPTION
-----------

'Blueprints' are Aegir-up project templates. They include the basic components
to define a new funtional project. Blueprints can be posted as projects to
drupal.org, and downloaded via 'drush dl <blueprint-name>'.


STRUCTURE
---------

  dev-aegir/         Template that will build an Aegir server
    makefiles/       Drush makefiles for building Aegir platforms
    manifests/       Puppet manifests defining 'hm' and 'hs' VMs
    modules/         Project-specific Puppet modules
    settings.rb      Parameters for VMs
