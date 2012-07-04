DESCRIPTION
-----------

'Blueprints' are Drush Vagrant project templates. They include the basic compo-
nents to define a new funtional project. Blueprints are included in Drush
extensions, and can thus be posted as projects to drupal.org, and downloaded
via 'drush dl'.

By virtue of being Drush extensions, they can declare their own Drush commands,
implement their own pre- and post- Drush hooks, and several related blueprints
can be bundled together in a single Drush extension.


STRUCTURE
---------

  example.drush.inc     Drush command file
  blueprint1/           Template for building a Vagrant project (set of VMs)
    manifests/          Puppet manifests defining VMs
      nodes.pp          Manifest defining individual VMs (nodes)
      site.pp           Principle control manifest; includes/runs others
    modules/            Project-specific Puppet modules
      ...               As required; can include custom modules
    settings.rb         Parameters for VMs; basic variables used in Vagrantfile
  blueprint2/
    ...


IMPLEMENTATION
--------------

Since blueprints are implemented within Drush extensions, hook_drush_command()
must be called, in order for Drush to recognize it. At an absolute minimum,
such a command would look like this:

<?php

  function example_drush_command() {
    $items = array();

    $items['example'] = array(
      'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
      'hidden' => TRUE,
    );

    return $items;
  }

?>

In addition, any blueprints provided must be exposed to Drush Vagrant by imple-
menting hook_vagrant_blueprints(). For more details see the API docs
(drush topic docs-vagrant-api). Here is a minimalist example:

<?php

/**
 * Implementation of hook_vagrant_blueprints().
 */
function example_vagrant_blueprints() {
  $blueprints = array(
    'example' => 'blueprints/example',
  );
  return $blueprints;
}

?>


EXAMPLES
--------

For further examples of Drush extensions providing blueprints, see the Aegir-up
project (http://drupal.org/project/aegir-up).

