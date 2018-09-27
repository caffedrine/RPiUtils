#!/usr/bin/env bash

rm PiGPIO
g++ -o PiGPIO ../main.cpp  ../PushButton.cpp -std=c++11 -lpigpio -lrt -lpthread
./PiGPIO