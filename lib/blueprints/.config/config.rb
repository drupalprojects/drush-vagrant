require "./settings.rb"

class VmAuto                        # default virtual machine settings
  Modules   = "aegirup_modules_path"   # puppet modules folder name
  Subnet    = "10"                  # 192.168.###.0/24 subnet for this network
end

class HmAuto                        # settings for our Aegir hostmaster machine
  Hostname  = "hm"
  FQDN      = "#{Hostname}.#{Vm::Domain}"
  Vmname    = "#{Hm::Vmname}(#{Hostname})"               # VirtualBox name
end

if defined?(Hs)
  class HsAuto                        # settings for our Aegir hostslave machine(s)
    Hostname  = "cluster"
    Vmname    = "#{Hs::Vmname}(#{HmAuto::Hostname})"             # VirtualBox name
  end
end

class AegirUpUser
  Username  = "username"
  Uid       = "uid"
  Gid       = "gid"
  Git_name  = "Firstname Lastname"
  Git_email = "username@example.com"
end

