rm gpio
g++ -o gpio GPIO.cpp -std=c++11 -lpigpio -lrt
./gpio
