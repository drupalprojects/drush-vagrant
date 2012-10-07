class Default < Vm             # VM-specific overrides of default settings
  Shortname  = "default"       # Vagrant name
  Longname   = "Default"       # VirtualBox name

  #Count     = 1                          # number of VMs to create
  #Basebox   = "debian-LAMP-2012-03-29"   # default 64-bit basebox
  #Box_url   = "http://ergonlogic.com/files/boxes/debian-LAMP-current.box"
  #Memory    = 512                        # default VM memory
  #Domain    = "local"                    # default domain
  #Manifests = "manifests"                # puppet manifests folder name
  #Modules   = {}                         # hash of puppet module folder names
  #Site      = "site"                     # name of manifest to apply
  #Gui       = true                       # start VM with GUI?
  #Verbose   = true                       # make output verbose?
  #Debug     = true                       # output debug info?
  #Options   = ""                         # options to pass to Puppet
  #Facts     = {}                         # hash of Factor facts

  #NFS_shares = { "www" => "/var/www", } # one or more NFS shares {"local-dir" => vm-dir", }
end
