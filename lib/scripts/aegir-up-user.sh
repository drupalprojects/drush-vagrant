#! /bin/sh

# Script to initialize a ~/.aegir-up file

# Set defaults
USER=`whoami`
GROUP=`id -gnr`
PROFILE="$HOME/.profile"
BASHRC="$HOME/.bashrc"
BASH_ALIASES="$HOME/.bash_aliases"
VIMRC="$HOME/.vimrc"
SSH_KEY_PUBLIC="$HOME/.ssh/id_rsa.pub"
#SSH_KEY_PRIVATE=$HOME/.ssh/id_rsa
NAME=`git config --global --get user.name`
EMAIL=`git config --global --get user.email`

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

HELP="Usage: $0 [-y] [-h]
Initialize the ~/.aegir-up file

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

if [ "$YES" = "off" ]; then
  if [ -e "$HOME/.aegir-up" ] ; then
    if prompt_yes_no "A file $HOME/.aegir-up already exists. Overwrite it?" ; then
      break
    else
      echo "Aborting."
      exit 0
    fi
  fi
fi

cat <<End-of-message
Your .aegir-up file will be initialized with the following settings:
Username:           $USER
Group:              $GROUP
.profile file       $PROFILE
.bashrc file:       $BASHRC
.bash_aliases file: $BASH_ALIASES
.vimrc file:        $VIMRC
Public SSH key:     $SSH_KEY_PUBLIC
Git username:       $NAME
Git email address:  $EMAIL
End-of-message
#Private SSH key:   $SSH_KEY_PRIVATE

if [ "$YES" = "off" ]; then
  if prompt_yes_no "Are these settings correct?" ; then
    YES=on
  fi
fi

if [ "$YES" = "on" ] ; then  # use our defaults
  USER_NAME=$USER
  USER_GROUP=$GROUP
  PROFILE_PATH=$PROFILE
  BASHRC_PATH=$BASHRC
  BASH_ALIASES_PATH=$BASH_ALIASES
  VIMRC_PATH=$VIMRC
  SSH_KEY_PUBLIC_PATH=$SSH_KEY_PUBLIC
  #SSH_KEY_PRIVATE_PATH=$SSH_KEY_PRIVATE
  GIT_NAME=$NAME
  GIT_EMAIL=$EMAIL
fi

if [ "$YES" = "off" ] ; then  #Prompt for everything

  # User & group
  read -p "What username would you like to use? ($USER)" answer
  if [ -z "$answer" ]; then
    USER_NAME=$USER
  else
    USER_NAME=$answer
  fi

  read -p "What group would you like to use? ($GROUP)" answer
  if [ -z "$answer" ]; then
    USER_GROUP=$GROUP
  else
    USER_GROUP=$answer
  fi


  # BASH
  read -p "What .profile file would you like to use? ($PROFILE)" answer
  if [ -z "$answer" ]; then
    PROFILE_PATH=$PROFILE
  else
    PROFILE_PATH=$answer
  fi

  read -p "What .bashrc file would you like to use? ($BASHRC)" answer
  if [ -z "$answer" ]; then
    BASHRC_PATH=$BASHRC
  else
    BASHRC_PATH=$answer
  fi

  read -p "What .bash_aliases file would you like to use? ($BASH_ALIASES)" answer
  if [ -z "$answer" ]; then
    BASH_ALIASES_PATH=$BASH_ALIASES
  else
    BASH_ALIASES_PATH=$answer
  fi


  # Vim
  read -p "What .vimrc file would you like to use? ($VIMRC)" answer
  if [ -z "$answer" ]; then
    VIMRC_PATH=$VIMRC
  else
    VIMRC_PATH=$answer
  fi


  # SSH
  read -p "What public SSH key would you like to use? ($SSH_KEY_PUBLIC)" answer
  if [ -z "$answer" ]; then
    SSH_KEY_PUBLIC_PATH=$SSH_KEY_PUBLIC
  else
    SSH_KEY_PUBLIC_PATH=$answer
  fi
  #read -p "What private SSH key would you like to use? ($SSH_KEY_PRIVATE)" answer
  #if [ -z "$answer" ]; then
  #  SSH_KEY_PRIVATE_PATH=$SSH_KEY_PRIVATE
  #else
  #  SSH_KEY_PRIVATE_PATH=$answer
  #fi

  # Git
  read -p "What name would you like to use for Git commits? ($NAME)" answer
  if [ -z "$answer" ]; then
    GIT_NAME=$NAME
  else
    GIT_NAME=$answer
  fi
  read -p "What email would you like to use for Git commits? ($EMAIL)" answer
  if [ -z "$answer" ]; then
    GIT_EMAIL=$EMAIL
  else
    GIT_EMAIL=$answer
  fi

fi

# Write ~/.aegir-up from this template

cat > $HOME/.aegir-up <<End-of-template
#! /bin/sh
# Default settings to use in Aegir-up VMs

# User & group
USER_NAME=$USER_NAME
USER_GROUP=$USER_GROUP

# BASH
PROFILE_PATH=$PROFILE_PATH
BASHRC_PATH=$BASHRC_PATH
BASH_ALIASES_PATH=$BASH_ALIASES_PATH

# Vim
VIMRC_PATH=$VIMRC_PATH

# SSH
SSH_KEY_PUBLIC_PATH=$SSH_KEY_PUBLIC_PATH
#SSH_KEY_PRIVATE_PATH=$SSH_KEY_PRIVATE_PATH

# Git
GIT_NAME="$GIT_NAME"
GIT_EMAIL=$GIT_EMAIL
End-of-template

if [ -e "$HOME/.aegir-up" ] ; then
  echo "$HOME/.aegir-up successfully written."
fi
