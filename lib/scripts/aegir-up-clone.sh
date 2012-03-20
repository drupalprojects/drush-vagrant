#! /bin/bash

# Script to clone a project from Git and initialize it locally.

. "$AEGIR_UP_ROOT/lib/scripts/aegir-up-functions.sh"

HELP="Usage: aegir-up clone [-h] <git-repo-url> <project-directory>
Clone a project from a Git repo, and initialize it locally.

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

GIT_REPO=$1
DIRECTORY=$2

if [ -z $GIT_REPO ]; then
  echo "ERROR: You must specify a Git repository URL to clone."
  echo "$HELP"
  exit 1
fi

if [ -z $DIRECTORY ]; then
  echo "ERROR: You must specify a directory to clone the project into."
  echo "$HELP"
  exit 1
fi

validate_project_name
if [ "$?" -eq "1" ]; then
  exit 1
fi

NEW_SUBNET=`new_subnet`
if [ "$?" -eq "1" ]; then
  exit 1
fi

# Clone the directory
cd "$AEGIR_UP_ROOT/projects/"
git clone $GIT_REPO $DIRECTORY
cd "$AEGIR_UP_ROOT/projects/$DIRECTORY"

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

echo "Project successfully cloned." 
echo "" 
echo "Your project's root is $AEGIR_UP_ROOT/projects/$NEW_PROJECT" 
echo "The subnet for your project has been set to 192.168.$NEW_SUBNET.0" 
echo "You may want to add the following line to your /etc/hosts:" 
if [ "$TEMPLATE" = "default" ] ; then 
  echo "           192.168.$NEW_SUBNET.10    aegir.local" 
else 
  echo "           192.168.$NEW_SUBNET.10    $NEW_PROJECT.aegir.local" 
fi 
