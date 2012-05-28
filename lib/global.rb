class Vm                                 # default virtual machine settings
  def self.descendants
    ObjectSpace.each_object(::Class).select {|klass| klass < self }
  end

  Count     = 1                          # The number of VMs to create
  Basebox   = "lucid32"                  # default basebox
  Box_url   = "http://files.vagrantup.com/lucid32.box"
  Memory    = 512                        # default VM memory
  Domain    = "local"                    # default domain
  Manifests = "manifests"                # puppet manifests folder name
  Modules   = "modules"                  # puppet modules folder name
  Site      = "site"                     # Name of manifest to apply
  Gui       = false                      # start VM with GUI?
  Verbose   = false                      # make output verbose?
  Debug     = false                      # output debug info?
  Options   = ""                         # options to pass to Puppet
  Facts     = {}
end

class Global
  Network   = "192.168"                  # Private network address: ###.###.0.0
  Host_IP   = 10                         # Starting host address: 192.168.0.###
  SSH_range = (32200..32250)
end
