#ifndef HAL_H
#define HAL_H

#include <pigpio.h>

//////////////////////////////
// PIN Schema: BCM/GPIO
//////////////////////////////

/** **************************************** **/

#define VERTICAL_MOTOR_PULSE_PIN            17
#define VERTICAL_MOTOR_DIRECTION_PIN        27
#define HORIZONTAL_MOTOR_RUN_PIN            22
#define HORIZONTAL_MOTOR_DIRECTION_PIN      10
#define CUTTER_MOTOR_PIN                    9

#define VERTICAL_MOTOR_INIT_BUTTON_PIN      2
#define HORIZONTAL_MOTOR_INIT_BUTTON_PIN    3
#define CROP_FINISHED_SENSOR_PIN            4

/** **************************************** **/

typedef enum LogicalLevel
{
    LOW = 0,
    HIGH = 1
}logic_level_t;

typedef enum PinMode
{
    OUTPUT = 1,
    INPUT = 0
}pin_mode_t;

int _initialize_gpio_interface()
{
    return gpioInitialise();
}

int _set_pin_mode(int pin, int  mode)
{
	return gpioSetMode(pin, mode);
}

int _read_gpio(int pin)
{
    return gpioRead(pin);
}

int _write_gpio(int pin, int level)
{
    return gpioWrite(pin, level);
}

int _gpio_sleep(int microseconds)
{
	return gpioSleep(0, 0, microseconds); 
}

#define Vfp_GpioInitialize()                _initialize_gpio_interface()
#define Vfb_SetPinMode(arg1, arg2)          _set_pin_mode(arg1, arg2)

#define Vfb_ReadGpio(arg)         			_read_gpio(arg)
#define Vfb_WriteGpio(arg1, arg2)           _write_gpio(arg1, arg2)
#define Vfb_SleepGPIO(arg1)					_gpio_sleep(arg1)

#endif // HAL_H
