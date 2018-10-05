#!/usr/bin/env bash

rm PiGPIO
g++ -o PiGPIO ../main.cpp ../Stepper.cpp -std=c++11 -lpigpio -lpthread
./PiGPIO