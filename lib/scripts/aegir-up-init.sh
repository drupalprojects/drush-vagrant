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

AEGIR_UP_ROOT=$(dirname $0)
HELP="Usage: $0 [-v|-q] [-n] [-y] [-t TEMPLATE] PROJECT
Initialize the PROJECT directory

  -t   Specify a template project to use
  -n   Don't initialize a git repo
  -y   Assume answer to any prompts is 'yes'
  -h   This help message"

GIT=on
YES=off
TEMPLATE="$AEGIR_UP_ROOT/lib/templates/default"
NEW_PROJECT=
while getopts vqnyht: opt
do
    case "$opt" in
      n)  GIT=off;;
      y)  YES=on;;
      h)  echo "$HELP"
          exit 0;;
      t)  TEMPLATE=$OPTARG;;
      \?)   # unknown flag
          msg "ERROR: Unknown flag.\n" >&2
          echo "$HELP"
          exit 1;;
    esac
done
shift `expr $OPTIND - 1`

NEW_PROJECT=$@

if ! [ -d $TEMPLATE ] ; then
  msg "ERROR: Could not find the $TEMPLATE directory."
  exit 1
fi


if [ -d $AEGIR_UP_ROOT/projects/$NEW_PROJECT ] ; then
  msg "ERROR: There is already a project called $NEW_PROJECT."
  exit 1
fi

msg "This script will create a new project at $AEGIR_UP_ROOT/projects/$NEW_PROJECT."
msg "It will use $TEMPLATE as a template."
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

INITIAL_SUBNET=10
NEW_SUBNET=

# Get a list of all the subnets already in use in ascending order
ALL_SUBNETS=`grep -h 'Subnet' "$AEGIR_UP_ROOT"/projects/*/settings.rb |perl -nle '/(\d+)/ and print $&'|sort`

# For each possible subnet...
for a in `seq "$INITIAL_SUBNET" 254`
do
  # ... check against all the existing subnets
  for b in $ALL_SUBNETS
  do
    if [ "$a" -lt "$b" ] ; then
      # We've found an available subnet, so let's go with it
      NEW_SUBNET=`expr $a`
      break 2 
    fi
    if [ "$a" -eq "$b" ] ; then
      # We've matched an existing subnet, so remove it from the list
      ALL_SUBNETS=`echo $ALL_SUBNETS | sed "s/$a//g"`
      # We've gone through the entire list, so the next subnet will work
      if [ -z "$ALL_SUBNETS" ] ; then
        NEW_SUBNET=`expr $a + 1`
        break 2
      fi
      break
    fi
  done
  if [ $a -gt 253 ] ; then
    echo "We've run out of subnets!"
    exit 1
  fi
done  

cp -r $TEMPLATE $AEGIR_UP_ROOT/projects/$NEW_PROJECT
cd $AEGIR_UP_ROOT/projects/$NEW_PROJECT
ln -s ../../lib/Vagrantfile .
sed "s/\"$INITIAL_SUBNET\"/\"$NEW_SUBNET\"/g" -i settings.rb
sed "s/\"Aegir\"/\"Aegir($NEW_PROJECT)\"/g" -i settings.rb
sed "s/\"Cluster\"/\"Cluster($NEW_PROJECT)\"/g" -i settings.rb
if [ "$GIT" = "on" ] ; then
  git init
  git add *
  git commit -m"Initial commit."
fi

msg "Project successfully initialized."
msg ""
msg "Your project's root is $AEGIR_UP_ROOT/projects/$NEW_PROJECT"
msg "The subnet for your project has been set to 192.168.$NEW_SUBNET.0"
msg "You can now: * Alter Aegir-up's behaviour by editing $AEGIR_UP_ROOT/projects/$NEW_PROJECT/settings.rb."
msg "             * Redefine the VMs by editing the Puppet manifests in $AEGIR_UP_ROOT/projects/$NEW_PROJECT/manifests."
msg "             * Add additional Puppet modules by copying them to $AEGIR_UP_ROOT/projects/$NEW_PROJECT/modules."
#msg "             * Build platforms in the front-end based on makefiles added to $AEGIR_UP_ROOT/projects/$NEW_PROJECT/makefiles."
#msg "             * Have platforms built outside the VM by mounting $AEGIR_UP_ROOT/projects/$NEW_PROJECT/platforms under NFS."
