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

if [ -z $VM ]; then
  read -r FIRSTLINE < "$AEGIR_UP_ROOT/projects/$PROJECT/.ssh.conf"
  VM=`echo $FIRSTLINE | awk ' {print $2} '`
fi

ssh -F "$AEGIR_UP_ROOT/projects/$PROJECT/.ssh.conf" $VM
