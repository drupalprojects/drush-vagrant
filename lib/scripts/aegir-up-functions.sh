# Load user-specific settings, if they exist
if [ -e ~/.aegir-up ] ; then
  . ~/.aegir-up
fi


# Return the next available subnet
new_subnet() {

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

  echo $NEW_SUBNET

}


# Ensure project name is unique and well-formed
validate_new_project() {

  if [ -d $AEGIR_UP_ROOT/projects/$NEW_PROJECT ] ; then
    msg "ERROR: There is already a project called $NEW_PROJECT."
    exit 1
  fi

  echo $NEW_PROJECT | egrep "^([a-z0-9][a-z0-9.-]*[a-z0-9])$" >/dev/null
  if [ $? -ne 0 ] ; then
    msg "ERROR: the name of your project should only contains lower-case letters, numbers, hyphens and dots (but no leading or trailing dots or hyphens)."
    exit 1
  fi

}


# Format messages
msg() {
  echo "==> $*"
}


# Simple 'yes/no' prompt
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


# Create a new project
setup_new_project() {

  # Make project-specific changes
  ln -s ../../lib/templates/Vagrantfile .
  ln -s ../../lib/templates/gitignore ./.gitignore
  cp -r ../../lib/templates/.config .
  sed "s/  Subnet    = \"10\"/  Subnet    = \"$NEW_SUBNET\"/g" -i "$CONFIG_DIR/config.rb"
  sed "s/  Hostname  = \"hm\"/  Hostname  = \"$NEW_PROJECT\"/g" -i "$CONFIG_DIR/config.rb"

  # Get user-specific files & make appropriate changes
  if [ -e ~/.aegir-up ] ; then
    DOTFILES_DIR="$CONFIG_DIR/files"
    mkdir $DOTFILES_DIR
    if ! [ -z $PROFILE_PATH ]; then
      cp $PROFILE_PATH $DOTFILES_DIR
    fi
    if ! [ -z $BASHRC_PATH ]; then
      cp $BASHRC_PATH $DOTFILES_DIR
    fi
    if ! [ -z $BASH_ALIASES_PATH ]; then
      cp $BASH_ALIASES_PATH $DOTFILES_DIR
      echo "alias aegir-up='sudo su aegir -l'" >> $DOTFILES_DIR/.bash_aliases
    fi
    if ! [ -z $VIMRC_PATH ]; then
      cp $VIMRC_PATH $DOTFILES_DIR
    fi
    if ! [ -z $SSH_KEY_PUBLIC_PATH ]; then
      cp $SSH_KEY_PUBLIC_PATH "$DOTFILES_DIR/authorized_keys"
    fi
    if ! [ -z $USER_NAME ]; then
      sed "s/  Username  = 'username'/  Username  = '$USER_NAME'/g" -i "$CONFIG_DIR/config.rb"
    fi
    if ! [ -z "$GIT_NAME" ]; then
      sed "s/  Git_name  = 'Firstname Lastname'/  Git_name  = '$GIT_NAME'/g" -i "$CONFIG_DIR/config.rb"
    fi
    if ! [ -z $GIT_EMAIL ]; then
      sed "s/  Git_email = 'username@example.com'/  Git_email = '$GIT_EMAIL'/g" -i "$CONFIG_DIR/config.rb"
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
  else 
    msg "Skipping user-specific settings. Run lib/scripts/aegir-up-user.sh to initialize a .aegir-up file."
  fi

  if [ "$VERBOSE" = on ]; then
    sed "s/  Verbose   = false/  Verbose   = true/g" -i "$CONFIG_DIR/config.rb"
    LOG_LEVEL='INFO'
  fi
  if [ "$DEBUG" = on ]; then
    sed "s/  Debug     = false/  Debug     = true/g" -i "$CONFIG_DIR/config.rb"
  fi

}


# Provide some next steps for the new project
further_instructions() {

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

}
