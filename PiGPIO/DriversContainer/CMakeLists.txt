<<<<<<< HEAD
cmake_minimum_required(VERSION 3.7)
set(PROJECT_NAME DriversContainer)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)

include(src/CMakeLists.txt)
=======
cmake_minimum_required(VERSION 3.12)
project(DriversContainer)

set(CMAKE_CXX_STANDARD 11)

# Enable PiGPIO if installed
# add_compile_definitions(PIGPIO)

file(GLOB_RECURSE DriversContainer_SOURCES "src/*.cpp")
file(GLOB_RECURSE DriversContainer_HEADERS "src/*.h")

set (DriversContainer_INCLUDE_DIRS "")
foreach (_headerFile ${DriversContainer_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND DriversContainer_INCLUDE_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES DriversContainer_INCLUDE_DIRS)

add_executable (DriversContainerExe ${DriversContainer_SOURCES})
target_include_directories(DriversContainerExe PRIVATE ${DriversContainer_INCLUDE_DIRS})

target_link_libraries(DriversContainerExe pthread)

# Enable pigpio if on RPi
#if(COMMAND PIGPIO)
    target_link_libraries(DriversContainerExe pigpio)
    target_link_libraries(DriversContainerExe rt)
#endif()
>>>>>>> ac7a99c05fc5da45a5f2d93278c52454f64446e3
