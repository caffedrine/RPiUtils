# Install sshd
#apt-get update
#apt-get install -y openssh-server

# Modify `sshd_config`
sed -i '/PermitEmptyPasswords/c\PermitEmptyPasswords yes' /etc/ssh/sshd_config
sed -i '/PermitRootLogin/c\PermitRootLogin yes' /etc/ssh/sshd_config
sed -i '/UsePAM/c\UsePAM no' /etc/ssh/sshd_config

# Delete root password (set as empty)
passwd -d root

/etc/init.d/ssh restart

/usr/sbin/sshd -D

# Optional
userdel pi