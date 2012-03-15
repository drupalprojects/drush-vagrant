#! /bin/bash

# Script to delete a project and destroy it's associated VMs

# simple prompt 
prompt_yes_no() {
  while true ; do
    printf "$* [Y/n] "
    read answer
    if [ -z "$answer" ] ; then
      return 0
    fi
    case $answer in
      [Yy]|[Yy][Ee][Ss])
        return 0
        ;;
      [Nn]|[Nn][Oo])
        return 1
        ;;
      *)
        echo "Please answer yes or no" 
        ;;
    esac
  done
}

HELP="Usage: $0 [-y] [-h] PROJECT
Delete a project and destroy its associated VMs
  -y   Assume answer to all prompts is 'yes'
  -h   This help message"

YES=off

while getopts yh opt
do
  case "$opt" in
    y)  YES=on;;
    h)  echo "$HELP"
        exit 0;;
    \?)   # unknown flag
        echo "ERROR: Unknown flag.\n" >&2
        echo "$HELP"
        exit 1;;
  esac
done
shift `expr $OPTIND - 1`

PROJECT=$@

if ! [ -d "$AEGIR_UP_ROOT/projects/$PROJECT" ] ; then
  echo "Could not find a project called '$PROJECT'. Exiting."
  exit 1
else
  if [ "$YES" = "off" ]; then
    echo "!!! WARNING: This operation cannot be undone. !!!"
    if prompt_yes_no "Are you certain that you want to delete the '$PROJECT' project, and destroy all of its VMs?"; then
      echo "Okay, you asked for it. Don't say we didn't warn you!"
      # pass through
    else
      echo "Aborting."
      exit 0
    fi
  fi
fi

EXIT=0

cd "$AEGIR_UP_ROOT/projects/$PROJECT"
if [ "$YES" = "on" ]; then
  vagrant destroy --force
else
  vagrant destroy
fi
if ! [ "$?" -eq 0 ]; then
  echo "ERROR: Could not destroy the VM(s)."
  EXIT=1
  if ! [ "$YES" = "on" ]; then
    echo "Exiting."
    exit $EXIT
  else
    echo "You may need to delete the VM(s) manually in VirtualBox."
  fi
else
  echo "VM(s) successfully destroyed."
fi

cd "$AEGIR_UP_ROOT"
rm -rf "$AEGIR_UP_ROOT/projects/$PROJECT"

echo "'$PROJECT' project deleted."
exit $EXIT
