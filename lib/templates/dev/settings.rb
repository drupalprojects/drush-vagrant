class Vm                            # default virtual machine settings
  Basebox   = "debian-LAMP-6.0.4"   # default basebox
  Box_url   = "http://ergonlogic.com/files/boxes/debian-LAMP-current.box"
  Gui       = false                 # start VM with GUI? Useful for loading CD/DVD ISOs
  Memory    = 512                   # default VM memory
  Manifests = "manifests"           # puppet manifests folder name
  Modules   = [ "modules", "../../lib/modules" ]             # puppet modules folder name
  Subnet    = "10"                  # 192.168.###.0/24 subnet for this network
  Verbose   = false                 # make output verbose?
  Debug     = false                 # output debug info?
  Options   = ""                    # options to pass to Puppet
end

class Hm                            # settings for our Aegir hostmaster machine
#  Basebox   = "aegir-1.6"           # pre-built Aegir server base box
#  Box_url   = "http://ergonlogic.com/files/boxes/aegir-current.box"
  Shortname = "hm"                  # Vagrant name (also used for manifest name, e.g., hm.pp)
  Hostname  = "aegir.local"         # host FQDN
  Vmname    = "Aegir"               # VirtualBox name
#  Memory    = 1024                  # override the default memory
end

###############################################################################
# WARNING: We haven't tested Hostslaves for a LONG time. Patches welcome.
###############################################################################

class Hs                            # settings for our Aegir hostslave machine(s)
#  Basebox   = "debian-LAMP-6.0.4"   # basic LAMP server base box
#  Box_url   = "http://ergonlogic.com/files/boxes/debian-LAMP-current.box"
  Count     = 0                     # number of hostslaves to create (will be used as a suffix to Shortname, Hostname & Vmname)
  Shortname = "hs"                  # Vagrant name (also used for manifest name, e.g., hs.pp)
  Hostname  = "cluster"             # host FQDN
  Vmname    = "Cluster"             # VirtualBox name
#  Memory    = 512                   # override the default memory
end
