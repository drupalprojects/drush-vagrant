class Default                            # default virtual machine settings
  Count     = 1                          # The number of VMs to create
  Basebox   = "debian-LAMP-2012-03-29"   # default basebox
  Box_url   = "http://ergonlogic.com/files/boxes/debian-LAMP-current.box"
  Gui       = false                      # start VM with GUI?
  Memory    = 512                        # default VM memory
  Manifests = "manifests"                # puppet manifests folder name
  Modules   = "modules"                  # puppet modules folder name
  Verbose   = false                      # make output verbose?
  Debug     = false                      # output debug info?
  Options   = ""                         # options to pass to Puppet
  Address   = 10
  Domain    = "local"                    # default domain
end


class Hm < Default             # VM-specific overrides of default settings
  Count      = 2
  Shortname  = "util"          # Vagrant name (used for manifest name, e.g., hm.pp)
  Vmname     = "Utility"       # VirtualBox name
  Aegir_root = "/var/aegir"    # Shared folder(s)
  Aegir_user = "aegir"         # Shared folder owner in VM
end

class Vm
  Types = [ Hm ]
end
