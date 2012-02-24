class Vm                            # default virtual machine settings
  Basebox   = "debian-6.0.4"        # default basebox
  Box_url   = "http://ergonlogic.com/files/boxes/debian-stable-current.box"
  Gui       = 0                     # start VM with GUI? Useful for loading CD/DVD ISOs
  Memory    = 512                   # default VM memory
  Manifests = "manifests"           # puppet manifests folder name
  Modules   = "modules"             # puppet modules folder name
  Subnet    = "32"                  # 192.168.###.0/24 subnet for this network
  Verbose   = 0                     # make output verbose?
  Debug     = 0                     # output debug info?
  Options   = ""                    # options to pass to Puppet
end

class Hm                            # settings for our Aegir hostmaster machine
  Basebox   = "aegir-1.6.0"         # pre-built Aegir server base box
  Box_url   = "http://ergonlogic.com/files/boxes/aegir-current.box"
  Shortname = "hm"                  # Vagrant name (also used for manifest name, e.g., hm.pp)
  Hostname  = "aegir.local"         # host FQDN
  Vmname    = "Aegir"               # VirtualBox name
#  Memory    = 1024                  # override the default memory
end

class Hs                            # settings for our Aegir hostslave machine(s)
#  Basebox   = "debian-LAMP-6.0.4"   # basic LAMP server base box
#  Box_url   = "http://ergonlogic.com/files/boxes/debian-stable-LAMP-current.box"
  Count     = 0                     # number of hostslaves to create (will be used as a suffix to Shortname, Hostname & Vmname)
  Shortname = "hs"                  # Vagrant name (also used for manifest name, e.g., hs.pp)
  Hostname  = "cluster"             # host FQDN
  Vmname    = "Cluster"             # VirtualBox name
#  Memory    = 512                   # override the default memory
end
