#! /bin/sh

# Script to initialize a ~/.aegir-up file

# Set defaults
USER=`whoami`
if [ -f "$HOME/.profile" ]; then
  PROFILE="$HOME/.profile"
fi
if [ -f "$HOME/.bashrc" ]; then
  BASHRC="$HOME/.bashrc"
fi
if [ -f "$HOME/.bash_aliases" ]; then
  BASH_ALIASES="$HOME/.bash_aliases"
fi
if [ -f "$HOME/.vimrc" ]; then
  VIMRC="$HOME/.vimrc"
fi
if [ -f "$HOME/.ssh/id_rsa.pub" ]; then
  SSH_KEY_PUBLIC="$HOME/.ssh/id_rsa.pub"
fi

NAME=`git config --global --get user.name`
EMAIL=`git config --global --get user.email`

if [ -f /etc/hosts ]; then
  HOSTS_FILE=/etc/hosts
fi

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
.profile file       $PROFILE
.bashrc file:       $BASHRC
.bash_aliases file: $BASH_ALIASES
.vimrc file:        $VIMRC
Public SSH key:     $SSH_KEY_PUBLIC
Git username:       $NAME
Git email address:  $EMAIL
Hosts file:         $HOSTS_FILE
End-of-message

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
  GIT_NAME=$NAME
  GIT_EMAIL=$EMAIL
fi

if [ "$YES" = "off" ] ; then  #Prompt for everything

  # Username
  read -p "What username would you like to use? ($USER)" answer
  if [ -z "$answer" ]; then
    USER_NAME=$USER
  else
    USER_NAME=$answer
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

# Username
USER_NAME=$USER_NAME

# BASH
PROFILE_PATH=$PROFILE_PATH
BASHRC_PATH=$BASHRC_PATH
BASH_ALIASES_PATH=$BASH_ALIASES_PATH

# Vim
VIMRC_PATH=$VIMRC_PATH

# SSH
SSH_KEY_PUBLIC_PATH=$SSH_KEY_PUBLIC_PATH

# Git
GIT_NAME="$GIT_NAME"
GIT_EMAIL=$GIT_EMAIL

# Hosts
HOSTS_FILE=$HOSTS_FILE

# Aegir-up
DEFAULT_TEMPLATE=$DEFAULT_TEMPLATE
End-of-template

if [ -e "$HOME/.aegir-up" ] ; then
  echo "$HOME/.aegir-up successfully written."
fi
