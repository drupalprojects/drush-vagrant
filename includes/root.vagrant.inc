<?php

/**
 * Prints the path to the current vagrant project.
 */
function drush_vagrant_root() {
  // Maybe this should error if there's no actual Vagrant project set and we
  // really are just defaulting to the current working directory.
  // PROJECT_PATH is defined in vagrant_drush_init().
  drush_print(PROJECT_PATH);
  return PROJECT_PATH;
}