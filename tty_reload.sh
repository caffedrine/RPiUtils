sudo modprobe -rf ftdi_sio
sudo modprobe -rf usbserial

sleep 1

sudo modprobe usbserial
sudo modprobe ftdi_sio
