#include <iostream>
#include <chrono>
#include <thread>

#include "hal.h"

using namespace std;

#define PWM_PIN		21
#define PWM_FREQ	5000
#define INT_PIN		20

void gpio_callback(int pin, int level, uint32_t tick, void *opaque)
{
	cout << "New GPIO state: " << Vfb_ReadGpio(INT_PIN) << endl;
}

int main()
{
	Vfb_GpioInitialise();
	
	Vfb_SetPinMode(PWM_PIN, OUTPUT);
	Vfb_SetPinMode(INT_PIN, INPUT);
	
	/* PWM stuff */
	int pwm_result = gpioPWM(PWM_PIN, 128);
	cout << "PWM result: " << pwm_result << endl;
	int freq_result = gpioSetPWMfrequency(PWM_PIN, PWM_FREQ);
	cout << "Freq result: " << freq_result << endl;
	
	/* Interruption pin state */
	gpioSetAlertFuncEx(INT_PIN, gpio_callback, NULL);
	
	bool toogleFlag = false;
	while(1)
	{
		//gpio_callback(0,0,0,NULL);
		this_thread::sleep_for( chrono::milliseconds(100) );
		continue;
	}
}