<?php
/**
 * @file
 * Template to generate a file to record the blueprint used for a project.
 *
 * Variables:
 * - $extension: The Drush extension that provides the blueprint.
 * - $blueprint: The blueprint used to build a project.
 */
?>
<?php print "<?php\n\n" ?>
$options['blueprint'] = array(
  '<?php print $extension; ?>' => '<?php print $blueprint; ?>',
);
