_aegirup() 
{
  local cur prev opts base
  if [ -f ~/.aegir-up ]; then
    . ~/.aegir-up
  fi
  COMPREPLY=()
  cur="${COMP_WORDS[COMP_CWORD]}"
  prev="${COMP_WORDS[COMP_CWORD-1]}"

  #
  #  The basic options we'll complete.
  #
  opts="ssh user init delete clone"


  #
  #  Complete the arguments to some of the basic commands.
  #
  case "${prev}" in
    init | ssh | delete)
      local projects=$(for x in `ls -l "$AEGIR_UP_ROOT/projects/" | grep ^d | awk '{for (i = 9; i <= NF; i++) printf("%s ",$i);printf("\n")}'`; do echo ${x//} ; done )
      COMPREPLY=( $(compgen -W "${projects}" ${cur}) )
      return 0
      ;;
    *)
      ;;
  esac

  COMPREPLY=($(compgen -W "${opts}" ${cur}))  
  return 0
}

complete -F _aegirup aegir-up
