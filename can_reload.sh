sudo modprobe -r can_raw
sudo modprobe -r can
sudo modprobe -r mcp251x
sudo modprobe -r can_dev
sudo modprobe -r spidev
sudo modprobe -r spi_bcm2835
sudo modprobe -r spi_bcm2835aux

sleep 1

sudo modprobe spidev

sudo modprobe spi_bcm2835aux
sudo modprobe spi_bcm2835

sudo modprobe can_dev
sudo modprobe mcp251x
sudo modprobe can
sudo modprobe can_raw
