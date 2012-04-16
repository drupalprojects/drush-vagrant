class Util < Vm             # VM-specific overrides of default settings
  Count      = 1
  Shortname  = "util"          # Vagrant name (used for manifest name, e.g., hm.pp)
  Longname   = "Utility"       # VirtualBox name
#  NFS_shares = [ "aegir_root" => "/var/aegir", ]
  Facts      = [ "" => "",
                 "" => "",
                 "" => "",]
  Aegir_root = "/var/aegir"    # Shared folder(s)
  Aegir_user = "aegir"         # Shared folder owner in VM
end
