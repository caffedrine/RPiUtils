include_directories(drivers)

add_executable(DriversContainer
        Config.h
        Common.h
        time_utils.h

        Motor.cpp
        Motor.h
        Button.cpp
        Button.h

        main.cpp)