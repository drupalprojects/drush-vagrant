#! /bin/sh

# Script to initialize a ~/.aegir-up file

# User & group
USER=`whoami`
read -p "What username would you like to use? ($USER)" answer
if [ -z "$answer" ]; then
  USER_NAME=$USER
else
  USER_NAME=$answer
fi

GROUP=`id -gnr`
read -p "What group would you like to use? ($GROUP)" answer
if [ -z "$answer" ]; then
  USER_GROUP=$GROUP
else
  USER_GROUP=$answer
fi


# BASH
BASHRC="$HOME/.bashrc"
read -p "What .bashrc file would you like to use? ($BASHRC)" answer
if [ -z "$answer" ]; then
  BASHRC_PATH=$BASHRC
else
  BASHRC_PATH=$answer
fi

BASH_ALIASES="$HOME/.bash_aliases"
read -p "What .bash_aliases file would you like to use? ($BASH_ALIASES)" answer
if [ -z "$answer" ]; then
  BASH_ALIASES_PATH=$BASH_ALIASES
else
  BASH_ALIASES_PATH=$answer
fi


# Vim
VIMRC="$HOME/.vimrc"
read -p "What .vimrc file would you like to use? ($VIMRC)" answer
if [ -z "$answer" ]; then
  VIMRC_PATH=$VIMRC
else
  VIMRC_PATH=$answer
fi


# SSH
SSH_KEY_PUBLIC="$HOME/.ssh/id_rsa.pub"
read -p "What public SSH key would you like to use? ($SSH_KEY_PUBLIC)" answer
if [ -z "$answer" ]; then
  SSH_KEY_PUBLIC_PATH=$SSH_KEY_PUBLIC
else
  SSH_KEY_PUBLIC_PATH=$answer
fi
#SSH_KEY_PRIVATE=$HOME/.ssh/id_rsa
#read -p "What private SSH key would you like to use? ($SSH_KEY_PRIVATE)" answer
#if [ -z "$answer" ]; then
#  SSH_KEY_PRIVATE_PATH=$SSH_KEY_PRIVATE
#else
#  SSH_KEY_PRIVATE_PATH=$answer
#fi

# Git
NAME=`git config --global --get user.name`
read -p "What name would you like to use for Git commits? ($NAME)" answer
if [ -z "$answer" ]; then
  GIT_NAME=$NAME
else
  GIT_NAME=$answer
fi
EMAIL=`git config --global --get user.email`
read -p "What email would you like to use for Git commits? ($EMAIL)" answer
if [ -z "$answer" ]; then
  GIT_EMAIL=$EMAIL
else
  GIT_EMAIL=$answer
fi

# .aegir-up template

cat > $HOME/.aegir-up <<End-of-template
#! /bin/sh
# Default settings to use in Aegir-up VMs

# User & group
USER_NAME=$USER_NAME
USER_GROUP=$USER_GROUP

# BASH
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
