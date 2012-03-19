#! /bin/bash

# Script to login to a VM via SSH

HELP="Usage: aegir-up ssh [-h] PROJECT [VM]
Login to a VM via SSH

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

PROJECT=$1
VM=$2

if ! [ -d "$AEGIR_UP_ROOT/projects/$PROJECT" ] ; then
  echo "Could not find a project called '$PROJECT'. Exiting."
  exit 1
fi

. ~/.aegir-up
cd "$AEGIR_UP_ROOT/projects/$PROJECT"
vagrant up

ssh "$(vagrant ssh-config $VM | grep -Eo 'HostName (([0-9]*[.]*)*)'| grep -Eo '(([0-9]*[.]*)*)')" -l $USER_NAME -i $SSH_KEY_PRIVATE_PATH -p "$(vagrant ssh-config $VM | grep -Eo 'Port ([0-9]*)'| grep -Eo '[0-9]*')"

cd -
