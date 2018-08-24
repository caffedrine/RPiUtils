# The usual update routine
sudo apt-get update -y
sudo apt-get upgrade -y

# Update the kernel - it also should install the headers
sudo rpi-update

sudo apt-get install raspberrypi-kernel-headers