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

