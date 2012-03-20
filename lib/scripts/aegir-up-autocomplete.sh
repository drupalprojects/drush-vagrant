_aegirup() 
{
  local cur prev opts base
  COMPREPLY=()
  cur="${COMP_WORDS[COMP_CWORD]}"
  prev="${COMP_WORDS[COMP_CWORD-1]}"

  #
  #  The basic options we'll complete.
  #
  opts="ssh user init delete"


  #
  #  Complete the arguments to some of the basic commands.
  #
  case "${prev}" in
    init | ssh | delete)
      local projects=$(for x in `cd ./projects/ && ls -l | grep ^d | awk '{for (i = 9; i <= NF; i++) printf("%s ",$i);printf("\n")}'`; do echo ${x//} ; done )
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
