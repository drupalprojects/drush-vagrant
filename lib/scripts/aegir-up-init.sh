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

if [ -e ~/.aegir-up ] ; then
  . ~/.aegir-up
  if ! [ -z $DEFAULT_TEMPLATE ]; then
    TEMPLATE=$DEFAULT_TEMPLATE
  else
    TEMPLATE="default"
  fi
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

echo $NEW_PROJECT | egrep "^([a-z0-9][a-z0-9.-]*[a-z0-9])$" >/dev/null
if [ $? -ne 0 ] ; then
  msg "ERROR: the name of your project should only contains lower-case letters, numbers, hyphens and dots (but no leading or trailing dots or hyphens)."
  exit 1
fi

if ! [ -d $AEGIR_UP_ROOT/lib/templates/$TEMPLATE ] ; then
  msg "ERROR: Could not find the '$TEMPLATE' template."
  exit 1
fi


if [ -d $AEGIR_UP_ROOT/projects/$NEW_PROJECT ] ; then
  msg "ERROR: There is already a project called $NEW_PROJECT."
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

INITIAL_SUBNET=10
NEW_SUBNET=

# Get a list of all the subnets already in use in ascending order
ALL_SUBNETS=`grep -h 'Subnet' "$AEGIR_UP_ROOT"/projects/*/$CONFIG_DIR/config.rb 2>/dev/null |perl -nle '/(\d+)/ and print $&'|sort`

# If there aren't any projects yet, use the default
if [ -z "$ALL_SUBNETS" ] ; then
  NEW_SUBNET=$INITIAL_SUBNET
else
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
fi

# Add domain to hosts file
if ! [ -z "$HOSTS_FILE" ]; then
  echo "Enter your password to add an entry for '$NEW_PROJECT' to your hosts file, or press CTRL-c to leave it as is."
  if [ "$TEMPLATE" = "default" ] ; then
    echo "192.168.$NEW_SUBNET.10    aegir.local" | sudo tee -a "$HOSTS_FILE"
  else
    echo "192.168.$NEW_SUBNET.10    $NEW_PROJECT.aegir.local" | sudo tee -a "$HOSTS_FILE"
  fi
fi

# Create the project directory
cp -r $AEGIR_UP_ROOT/lib/templates/$TEMPLATE $AEGIR_UP_ROOT/projects/$NEW_PROJECT
cd $AEGIR_UP_ROOT/projects/$NEW_PROJECT

# Make project-specific changes
ln -s ../../lib/templates/Vagrantfile .
ln -s ../../lib/templates/gitignore ./.gitignore
sed "s/  Subnet    = \"10\"/  Subnet    = \"$NEW_SUBNET\"/g" -i "$CONFIG_DIR/config.rb"
sed "s/  Hostname  = \"hm\"/  Hostname  = \"$NEW_PROJECT\"/g" -i "$CONFIG_DIR/config.rb"

# Get user-specific files & make appropriate changes
if [ -e ~/.aegir-up ] ; then
  DOTFILES_DIR="$CONFIG_DIR/files"
  mkdir $DOTFILES_DIR
  cp $PROFILE_PATH $DOTFILES_DIR
  cp $BASHRC_PATH $DOTFILES_DIR
  cp $BASH_ALIASES_PATH $DOTFILES_DIR
  cp $VIMRC_PATH $DOTFILES_DIR
  cp $SSH_KEY_PUBLIC_PATH "$DOTFILES_DIR/authorized_keys"
  sed "s/  Username  = 'username'/  Username  = '$USER_NAME'/g" -i "$CONFIG_DIR/config.rb"
  sed "s/  Git_name  = 'Firstname Lastname'/  Git_name  = '$GIT_NAME'/g" -i "$CONFIG_DIR/config.rb"
  sed "s/  Git_email = 'username@example.com'/  Git_email = '$GIT_EMAIL'/g" -i "$CONFIG_DIR/config.rb"
else 
  msg "Skipping user-specific settings. Run lib/scripts/aegir-up-user.sh to initialize a .aegir-up file."
fi

if [ "$VERBOSE" = on ]; then
  sed "s/  Verbose   = false/  Verbose   = true/g" -i "$CONFIG_DIR/config.rb"
  LOG_LEVEL='INFO'
fi
if [ "$DEBUG" = on ]; then
  sed "s/  Debug     = false/  Debug     = true/g" -i "$CONFIG_DIR/config.rb"
  LOG_LEVEL='DEBUG'
fi

# Provision the VM(s)
if [ "$UP" = "on" ]; then
  VAGRANT_LOG=$LOG_LEVEL
  export VAGRANT_LOG
  vagrant up
  # Set up SSH
  vagrant ssh-config > $CONFIG_DIR/ssh.conf
  sed "s/User vagrant/User $USER_NAME/g" -i $CONFIG_DIR/ssh.conf
  if ! [ -z $SSH_KEY_PUBLIC_PATH ]; then
    sed "s|IdentityFile $HOME/.vagrant.d/insecure_private_key|IdentityFile $HOME/.ssh/id_rsa|g" -i $CONFIG_DIR/ssh.conf
  fi
else
  echo "Skipping automatic provisioning."
fi


# Set up git
if [ "$GIT" = "on" ] ; then
  git init
  git add modules/
  git add manifests/
  git add settings.rb
  git commit -m"Initial commit."
fi

msg "Project successfully initialized."
msg ""
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
