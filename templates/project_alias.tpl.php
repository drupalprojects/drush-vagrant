<?php
/**
 * @file
 * Template to generate a drush alias for a VM.
 *
 * Variables:
 * - $alias: The name of the project.
 * - $project_path: The filesystem path to the project's directory
 */
?>
<?php print "<?php\n\n" ?>
$aliases['<?php print $alias; ?>'] = array(
  'project_path' => '<?php print $project_path; ?>',
);
