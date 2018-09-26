# Install sshd
apt-get update
apt-get install -y openssh-server

# Modify `sshd_config`
sed -ri 's/PermitEmptyPasswords no/PermitEmptyPasswords yes/' /etc/ssh/sshd_config
sed -ri 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config
sed -ri 's/^UsePAM yes/UsePAM no/' /etc/ssh/sshd_config
sed -ri 's/^#PermitEmptyPasswords yes/PermitEmptyPasswords yes/' /etc/ssh/sshd_config

/etc/init.d/ssh start

# Delete root password (set as empty)
passwd -d root

/usr/sbin/sshd -D