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

validate_project_name
if [ "$?" -eq "1" ]; then
  exit 1
fi

NEW_SUBNET=`new_subnet`
if [ "$?" -eq "1" ]; then
  exit 1
fi

# Clone the directory
cd "$AEGIR_UP_ROOT/projects/"
git clone $GIT_REPO $DIRECTORY
cd "$AEGIR_UP_ROOT/projects/$DIRECTORY"


setup_new_project
if [ "$?" -eq "1" ]; then
  exit 1
fi

echo "Project successfully cloned." 
echo "" 
echo "Your project's root is $AEGIR_UP_ROOT/projects/$NEW_PROJECT" 
echo "The subnet for your project has been set to 192.168.$NEW_SUBNET.0" 
echo "You may want to add the following line to your /etc/hosts:" 
if [ "$TEMPLATE" = "default" ] ; then 
  echo "           192.168.$NEW_SUBNET.10    aegir.local" 
else 
  echo "           192.168.$NEW_SUBNET.10    $NEW_PROJECT.aegir.local" 
fi 
