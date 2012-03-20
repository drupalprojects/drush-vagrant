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

# Provide some next steps (see: lib/scripts/aegir-up-functions.sh)
further_instructions
