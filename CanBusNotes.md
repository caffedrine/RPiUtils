MPC2515 -  Even if configured like this: "sudo ip link set can0 up type can bitrate 1000000 restart-ms 1000" bitrate wull still be 500000
It doesn't support CanFD. It will not work with cars since max baudrate is too low.

SeedStudio Shield - Supports CanFD and bit rate works fine. Works with cars, you can connect to car bus and read data.
