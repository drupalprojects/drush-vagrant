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

  -t   Specify a template project to use
  -n   Don't initialize a git repo
  -x   Don't provision the VM(s)
  -y   Assume answer to all prompts is 'yes'
  -h   This help message"

GIT=on
YES=off
UP=on
TEMPLATE="default"
NEW_PROJECT=
while getopts nyxht: opt
do
    case "$opt" in
      n)  GIT=off;;
      y)  YES=on;;
      x)  UP=off;;
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
ALL_SUBNETS=`grep -h 'Subnet' "$AEGIR_UP_ROOT"/projects/*/settings.rb 2>/dev/null |perl -nle '/(\d+)/ and print $&'|sort`

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

# Create the project directory
cp -r $AEGIR_UP_ROOT/lib/templates/$TEMPLATE $AEGIR_UP_ROOT/projects/$NEW_PROJECT
cd $AEGIR_UP_ROOT/projects/$NEW_PROJECT

# Make project-specific changes
ln -s ../../lib/templates/Vagrantfile .
ln -s ../../lib/templates/gitignore ./.gitignore
sed "s/\"$INITIAL_SUBNET\"/\"$NEW_SUBNET\"/g" -i settings.rb
sed "s/\"Aegir\"/\"Aegir($NEW_PROJECT)\"/g" -i settings.rb
sed "s/\"Cluster\"/\"Cluster($NEW_PROJECT)\"/g" -i settings.rb
if ! [ "$TEMPLATE" = "default" ] ; then
  sed "s/\"aegir.local\"/\"$NEW_PROJECT.aegir.local\"/g" -i settings.rb
  sed "s/'aegir.local'/'$NEW_PROJECT.aegir.local'/g" -i manifests/hm.pp
fi

# Get user-specific files & make appropriate changes
if [ -e ~/.aegir-up ] ; then
  . ~/.aegir-up
  USER_DOTFILES_DIR=manifests/files
  mkdir $USER_DOTFILES_DIR
  cp $PROFILE_PATH $USER_DOTFILES_DIR
  cp $BASHRC_PATH $USER_DOTFILES_DIR
  cp $BASH_ALIASES_PATH $USER_DOTFILES_DIR
  cp $VIMRC_PATH $USER_DOTFILES_DIR
  cp $SSH_KEY_PUBLIC_PATH "$USER_DOTFILES_DIR/authorized_keys"
  #cp $SSH_KEY_PRIVATE_PATH $USER_DOTFILES_DIR
  sed "s/#  \$aegir_up_username = 'username'/  \$aegir_up_username = '$USER_NAME'/g" -i manifests/hm.pp
  sed "s/#  \$aegir_up_git_name = 'Firstname Lastname'/  \$aegir_up_git_name = '$GIT_NAME'/g" -i manifests/hm.pp
  sed "s/#  \$aegir_up_git_email = 'username@example.com'/  \$aegir_up_git_email = '$GIT_EMAIL'/g" -i manifests/hm.pp
else 
  msg "Skipping user-specific settings. Run lib/scripts/aegir-up-user.sh to initialize a .aegir-up file."
fi

# Provision the VM(s)
if [ "$UP" = "on" ]; then
  vagrant up
  # Set up SSH
  vagrant ssh-config > .ssh.conf
  sed "s/User vagrant/User $USER_NAME/g" -i .ssh.conf
  if ! [ -z $SSH_KEY_PUBLIC_PATH ]; then
    sed "s|IdentityFile $HOME/.vagrant.d/insecure_private_key|IdentityFile $HOME/.ssh/id_rsa|g" -i .ssh.conf
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
msg "You may want to add the following line to your /etc/hosts:"
if [ "$TEMPLATE" = "default" ] ; then
  msg "           192.168.$NEW_SUBNET.10    aegir.local"
else
  msg "           192.168.$NEW_SUBNET.10    $NEW_PROJECT.aegir.local"
fi
msg "You can now: * Alter Aegir-up's behaviour by editing '$AEGIR_UP_ROOT/projects/$NEW_PROJECT/settings.rb.'"
msg "             * Redefine the VMs by editing the Puppet manifests in '$AEGIR_UP_ROOT/projects/$NEW_PROJECT/manifests.'"
msg "             * Add additional Puppet modules by copying them to '$AEGIR_UP_ROOT/projects/$NEW_PROJECT/modules.'"
#msg "             * Build platforms in the front-end based on makefiles added to $AEGIR_UP_ROOT/projects/$NEW_PROJECT/makefiles."
#msg "             * Have platforms built outside the VM by mounting $AEGIR_UP_ROOT/projects/$NEW_PROJECT/platforms under NFS."
