# Remove boot menu delay
echo 'GRUB_HIDDEN_TIMEOUT=true' >> /etc/default/grub
sed 's/GRUB_TIMEOUT=5/GRUB_TIMEOUT=0/g' -i /etc/default/grub
update-grub
