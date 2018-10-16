## How to enable SSH on Raspbbian without monitor and Keyboard?

Simply create an empty file on RPI's sdcard on location /boot called **ssh** and the ssh daemon will start by default at startup so you no longer need hrmi and keyboard. 