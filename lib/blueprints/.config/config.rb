require "/path/to/global.rb"
require "./settings.rb"

class Conf < Global
  Workspace = "workspace_name"
  Modules   = "vagrant_modules_path"    # puppet modules folder name
  Subnet    = "subnet"                  # 192.168.###.0/24 subnet for this network
end

class AegirUpUser
  Username  = "username"
  Uid       = "uid"
  Gid       = "gid"
  Git_name  = "Firstname Lastname"
  Git_email = "username@example.com"
end
