class Vm                            # default virtual machine settings
  Basebox   = "debian-LAMP-2012-03-29"   # default basebox
  Box_url   = "http://ergonlogic.com/files/boxes/debian-LAMP-current.box"
  Gui       = true                 # start VM with GUI? Useful for loading CD/DVD ISOs
  Memory    = 512                   # default VM memory
  Manifests = "manifests"           # puppet manifests folder name
  Modules   = "modules"             # puppet modules folder name
  Verbose   = true                 # make output verbose?
  Debug     = true                 # output debug info?
  Options   = ""                    # options to pass to Puppet
  Domain    = "util.local"
end

class Hm                            # settings for our Aegir hostmaster machine
#  Basebox   = "debian-LAMP-2012-03-29"           # pre-built Aegir server base box
#  Box_url   = "http://ergonlogic.com/files/boxes/debian-LAMP-current.box"
  Shortname  = "util"                  # Vagrant name (also used for manifest name, e.g., hm.pp)
  Vmname     = "Utility"               # VirtualBox name
  Aegir_root = "/var/aegir"
  Aegir_user = "aegir"
#  Memory    = 1024                  # override the default memory
end

