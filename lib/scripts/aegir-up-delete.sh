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

  -d   Turn on debugging output
  -y   Assume answer to all prompts is 'yes'
  -h   This help message"

DEBUG=off
YES=off

while getopts dhy opt
do
  case "$opt" in
    d)  DEBUG=on;;
    h)  echo "$HELP"
        exit 0;;
    y)  YES=on;;
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
# See: https://github.com/mitchellh/vagrant/issues/811
if ! [ "$?" -eq 0 ] || [ -f '.vagrant' ]; then
  echo "ERROR: Could not destroy the VM(s), or action aborted."
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

if [ -f ~/.aegir-up ] && ! [ "$EXIT" -eq "1" ]; then
  . ~/.aegir-up
  SUBNET=`grep -Eo 'Subnet    = "([0-9]*)"' .config/config.rb | grep -Eo '[0-9]*'`
  REMOVE_LINE=`grep 192.168."$SUBNET".10 "$HOSTS_FILE"`
  SED_ARG=/"$REMOVE_LINE"/d
  if [ "$DEBUG" = "on" ]; then
    echo "\$SUBNET = $SUBNET"
    echo "\$REMOVE_LINE = $REMOVE_LINE"
    echo "\$SED_ARG = $SED_ARG"
  fi
  echo "Enter your password to remove this projects entry from your hosts file, or press CTRL-c to leave it as is."
  sudo sed "$SED_ARG" -i "$HOSTS_FILE"
fi

cd "$AEGIR_UP_ROOT"
rm -rf "$AEGIR_UP_ROOT/projects/$PROJECT"

echo "'$PROJECT' project deleted."
exit $EXIT
