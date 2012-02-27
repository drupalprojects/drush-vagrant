#! /bin/sh

########################################################################
# functions

msg() {
  echo "==> $*"
}

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

########################################################################

if [ $# -ne 1 ] ; then
  msg "This script requires one (and only one) argument: a name for your new project."
  exit 1
fi

AEGIR_UP_ROOT=$(dirname $0)
NEW_PROJECT=$1

if [ -d $AEGIR_UP_ROOT/projects/$NEW_PROJECT ] ; then
  msg "There is already a project called $NEW_PROJECT."
  exit 1
fi

msg "This script will create a new project at $AEGIR_UP_ROOT/projects/$NEW_PROJECT and initialize a git repo within it."

if prompt_yes_no "Do you want to proceed with initializing the project?" ; then
  true
else
  msg "Project initialization aborted by user."
  exit 1
fi

cp -r $AEGIR_UP_ROOT/lib/project_template $AEGIR_UP_ROOT/projects/$NEW_PROJECT
cd $AEGIR_UP_ROOT/projects/$NEW_PROJECT
git init
git add *
git commit -m"Initial commit."


msg "Project initialized."
msg "You can alter Aegir-up's behaviour by editing $AEGIR_UP_ROOT/projects/$NEW_PROJECT/settings.rb."
msg "You can redefine the VMs by editing the manifests in $AEGIR_UP_ROOT/projects/$NEW_PROJECT/manifests."
msg "You can add additional Puppet modules by copying them to $AEGIR_UP_ROOT/projects/$NEW_PROJECT/modules."
