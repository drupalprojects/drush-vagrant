# Remove boot menu delay
echo 'GRUB_HIDDEN_TIMEOUT=0' >> /etc/default/grub
update-grub
