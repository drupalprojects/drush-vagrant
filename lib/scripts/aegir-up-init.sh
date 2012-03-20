#! /bin/sh

. $AEGIR_UP_ROOT/lib/scripts/aegir-up-functions.sh
if [ -e ~/.aegir-up ] ; then
  . ~/.aegir-up
fi

HELP="Usage: aegir-up init [-n] [-y] [-x] [-t TEMPLATE] PROJECT
Initialize the PROJECT directory

  -d   Debugging output
  -g   Skip git repo initializing
  -h   This help message
  -t   Specify a template project to use
  -v   Verbose output
  -x   Don't provision the VM(s)
  -y   Assume answer to all prompts is 'yes'"

DEBUG=off
GIT=on

if ! [ -z $DEFAULT_TEMPLATE ]; then
  TEMPLATE=$DEFAULT_TEMPLATE
else
  TEMPLATE="default"
fi

VERBOSE=off
UP=on
YES=off

while getopts dght:vxy opt
do
    case "$opt" in
      d)  DEBUG=on;;
      g)  GIT=off;;
      h)  echo "$HELP"
          exit 0;;
      t)  TEMPLATE=$OPTARG;;
      v)  VERBOSE=on;;
      x)  UP=off;;
      y)  YES=on;;
      \?)   # unknown flag
          msg "ERROR: Unknown flag.\n" >&2
          echo "$HELP"
          exit 1;;
    esac
done
shift `expr $OPTIND - 1`

NEW_PROJECT=$@

if [ -z $NEW_PROJECT ] ; then
  NEW_PROJECT="default"
fi

if ! [ -d $AEGIR_UP_ROOT/lib/templates/$TEMPLATE ] ; then
  msg "ERROR: Could not find the '$TEMPLATE' template."
  exit 1
fi

# Ensure new project name is unique and well-formed (see: lib/scripts/aegir-up-functions.sh)
validate_new_project
if [ "$?" -eq "1" ]; then
  exit 1
fi

msg "This script will create a new project at $AEGIR_UP_ROOT/projects/$NEW_PROJECT."
msg "It will use the '$TEMPLATE' template."
if [ "$GIT" = "on" ] ; then
  msg "A Git repo will be initialized in the new project directory."
fi

if [ "$YES" = "off" ] ; then
  if prompt_yes_no "Do you want to proceed with initializing the project?" ; then
    true
  else
    msg "Project initialization aborted by user."
    exit 1
  fi
fi

# Find the next available subnet (see: lib/scripts/aegir-up-functions.sh)
NEW_SUBNET=`new_subnet`
if [ "$?" -eq "1" ]; then
  exit 1
fi

# Create the project directory
cp -r $AEGIR_UP_ROOT/lib/templates/$TEMPLATE $AEGIR_UP_ROOT/projects/$NEW_PROJECT
cd $AEGIR_UP_ROOT/projects/$NEW_PROJECT

# Set up the new project (see: lib/scripts/aegir-up-functions.sh)
setup_new_project
if [ "$?" -eq "1" ]; then
  exit 1
fi

# Initial Git setup
if [ "$GIT" = "on" ] ; then
  git init
  git add modules/
  git add manifests/
  git add settings.rb
  git commit -m"Initial commit."
fi

# Save the install log within the project
mv ../"$NEW_PROJECT_install.log" $CONFIG_DIR/install.log

msg "Project successfully initialized."
msg ""

further_instructions
msg "Your project's root is $AEGIR_UP_ROOT/projects/$NEW_PROJECT"
msg "The subnet for your project has been set to 192.168.$NEW_SUBNET.0"
if [ -z $HOSTS_FILE ]; then
  msg "You may want to add the following line to your /etc/hosts:"
  if [ "$TEMPLATE" = "default" ] ; then
    msg "           192.168.$NEW_SUBNET.10    aegir.local"
  else
    msg "           192.168.$NEW_SUBNET.10    $NEW_PROJECT.aegir.local"
  fi
  msg "you can have Aegir-up do this for you automatically by specifying your hosts file in ~/.aegir-up"
fi
msg "You can now: * Alter Aegir-up's behaviour by editing '$AEGIR_UP_ROOT/projects/$NEW_PROJECT/settings.rb.'"
msg "             * Redefine the VMs by editing the Puppet manifests in '$AEGIR_UP_ROOT/projects/$NEW_PROJECT/manifests.'"
msg "             * Add additional Puppet modules by copying them to '$AEGIR_UP_ROOT/projects/$NEW_PROJECT/modules.'"
#msg "             * Build platforms in the front-end based on makefiles added to $AEGIR_UP_ROOT/projects/$NEW_PROJECT/makefiles."
#msg "             * Have platforms built outside the VM by mounting $AEGIR_UP_ROOT/projects/$NEW_PROJECT/platforms under NFS."
