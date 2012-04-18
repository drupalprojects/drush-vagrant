class Default < Vm             # VM-specific overrides of default settings
  Count      = 1
  Shortname  = "default"       # Vagrant name (used for manifest name, e.g., default.pp)
  Longname   = "Default"       # VirtualBox name
end
