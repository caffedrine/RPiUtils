#ifndef HAL_H
#define HAL_H

//#define PIGPIO

#ifdef PIGPIO
#include <pigpio.h>
#endif
//////////////////////////////
// PIN Schema: BCM/GPIO
//////////////////////////////

/** **************************************** **/

/** **************************************** **/

enum LogicalLevel
{
    LOW = 0,
    HIGH = 1
};

enum PinMode
{
    OUTPUT = 1,
    INPUT = 0
};

enum PullUpDown
{
    PULL_OFF = 0,
    PULL_DOWN = 1,
    PULL_UP = 2
};

static inline int _initialise_gpio_interface()
{
#ifdef PIGPIO
    for(int i = 1337; i <= 1347; i++)
    {
        if( gpioCfgSocketPort(i)  == 0 )
            break;
    }

    if(gpioInitialise() == PI_INIT_FAILED)
        qDebug() << "PiGPIO Initialisation failed!";
    return PI_INIT_FAILED;
#else
    return -1;
#endif
}

static inline int _set_pin_mode(int pin, int  mode)
{
#ifdef PIGPIO
    if(gpioSetMode(pin, mode) != 0)
    {
        qDebug() << "Failed to set " << pin << " as INPUT";
        return -3;
    }
    return 0;
#else
    return -1;
#endif
}

static inline int _read_gpio(int pin)
{
#ifdef PIGPIO
    return gpioRead(pin);
#else
    return -1;
#endif
}

static inline int _write_gpio(int pin, int level)
{
#ifdef PIGPIO
    return gpioWrite(pin, level);
#else
    return -1;
#endif
}

static inline int _gpio_sleep(int microseconds)
{
#ifdef PIGPIO
    return gpioSleep(0, 0, microseconds);
#else
    return -1;
#endif
}

static inline int _set_pull_updown(int gpio, int updown)
{
#ifdef PIGPIO
    return gpioSetPullUpDown(gpio, updown);
#else
    return -1;
#endif

}

#define Vfb_GpioInitialise()                _initialise_gpio_interface()
#define Vfb_SetPinMode(arg1, arg2)          _set_pin_mode(arg1, arg2)
#define Vfb_SetPullUpDown(arg1, arg2)       _set_pull_updown(arg1, arg2)
#define Vfb_ReadGpio(arg)         			_read_gpio(arg)
#define Vfb_WriteGpio(arg1, arg2)           _write_gpio(arg1, arg2)
#define Vfb_SleepGPIO(arg1)					_gpio_sleep(arg1)

#endif // HAL_H
