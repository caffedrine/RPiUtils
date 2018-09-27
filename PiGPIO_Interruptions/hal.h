#ifndef HAL_H
#define HAL_H

#define PIGPIO

#ifdef PIGPIO
#include <pigpio.h>
#endif
//////////////////////////////
// PIN Schema: BCM/GPIO
//////////////////////////////

/** **************************************** **/

/** **************************************** **/

#define ERROR_LOG(str)	0

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

enum ReturnStatus
{
    SUCCES = 0,
    FAIL = -1
};

static inline int _initialise_gpio_interface()
{
#ifdef PIGPIO
//    for(int i = 1337; i <= 1347; i++)
//    {
//        if( gpioCfgSocketPort(i)  == 0 )
//            break;
//    }
    if(gpioInitialise() == PI_INIT_FAILED)
    	ERROR_LOG("Failed to initialise PiGPIO");
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
    	//ERROR_LOG("Failed to set " << pin << " as INPUT");
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

static inline int _pwm_output(unsigned pin, unsigned u8Duty)
{
#ifdef PIGPIO
    return gpioPWM(pin, u8Duty);
#else
    return FAIL;
#endif
}

static inline int _pwm_init(unsigned pin, unsigned freq)
{
#ifdef PIGPIO
    _pwm_output(pin, 0);
    return gpioSetPWMfrequency(pin, freq);
#else
    return FAIL;
#endif
}

static inline int _get_micros()
{
#ifdef PIGPIO
	return gpioTick();
#else
	return FAIL;
#endif
}

static inline int _get_millis()
{
#ifdef PIGPIO
	return (gpioTick()/1000);
#else
	return FAIL;
#endif
}

#define Vfb_GpioInitialise()                _initialise_gpio_interface()
#define Vfb_SetPinMode(pin, mode)          	_set_pin_mode(pin, mode)
#define Vfb_SetPullUpDown(pin, pull)        _set_pull_updown(pin, pull)
#define Vfb_ReadGpio(arg)         			_read_gpio(arg)
#define Vfb_WriteGpio(arg1, arg2)           _write_gpio(arg1, arg2)
#define Vfb_SleepGPIO(arg1)					_gpio_sleep(arg1)

#define Vfb_InitPwm(pin, freq)				_pwm_init(pin, freq)
#define Vfb_PwmOut(pin, u8duty)             _pwm_output(pin, u8duty)

#define Vfb_GetMicros()						_get_micros()
#define Vfb_GetMillis()						_get_millis()

#endif // HAL_H
