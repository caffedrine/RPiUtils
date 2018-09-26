cmake_minimum_required(VERSION 3.12)
project(PiGPIO)

set(CMAKE_CXX_STANDARD 11)

add_executable(PiGPIO main.cpp)

target_link_libraries(PiGPIO pigpio)
target_link_libraries(PiGPIO rt)
target_link_libraries(PiGPIO pthread)