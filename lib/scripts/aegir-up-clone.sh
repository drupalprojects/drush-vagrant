#! /bin/bash

# Script to clone a project from Git and initialize it locally.

. "$AEGIR_UP_ROOT/lib/scripts/aegir-up-functions.sh"

HELP="Usage: aegir-up clone [-h] <git-repo-url> <project-directory>
Clone a project from a Git repo, and initialize it locally.

  -h   This help message"

while getopts h opt
do
  case "$opt" in
    h)  echo "$HELP"
        exit 0;;
    \?)   # unknown flag
        echo "ERROR: Unknown flag.\n" >&2
        echo "$HELP"
        exit 1;;
  esac
done
shift `expr $OPTIND - 1`

GIT_REPO=$1
DIRECTORY=$2

if [ -z $GIT_REPO ]; then
  echo "ERROR: You must specify a Git repository URL to clone."
  echo "$HELP"
  exit 1
fi

if [ -z $DIRECTORY ]; then
  echo "ERROR: You must specify a directory to clone the project into."
  echo "$HELP"
  exit 1
fi

# Ensure new project name is unique and well-formed (see: lib/scripts/aegir-up-functions.sh)
validate_project_name
if [ "$?" -eq "1" ]; then
  exit 1
fi

# Find the next available subnet (see: lib/scripts/aegir-up-functions.sh)
NEW_SUBNET=`new_subnet`
if [ "$?" -eq "1" ]; then
  exit 1
fi

# Clone the directory
cd "$AEGIR_UP_ROOT/projects/"
git clone $GIT_REPO $DIRECTORY
cd "$AEGIR_UP_ROOT/projects/$DIRECTORY"

# Set up the new project (see: lib/scripts/aegir-up-functions.sh)
setup_new_project
if [ "$?" -eq "1" ]; then
  exit 1
fi

echo "Project successfully cloned." 
echo "" 

# Provide some next steps (see: lib/scripts/aegir-up-functions.sh)
further_instructions
