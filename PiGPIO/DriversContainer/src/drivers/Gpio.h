#ifndef _GPIO_H
#define _GPIO_H

#include <iostream>

#include "hal.h"

typedef void (*level_changed_cb_t)(LogicalLevel);

class Gpio
{
public:
	int Pin;
	bool ReversedPolarity = false;
	int DebounceTimeUs = 0;
	LogicalLevel CurrentState, PreviousState;
	
	Gpio(int GpioPin);
	Gpio(int GpioPin, int DebounceTimeMicroseconds);
	~Gpio();
	
	LogicalLevel Read();
	void SetPullState(PullState);
	void SetReversedPolarity(bool reveresed);
	void SetStateChangedCallback( level_changed_cb_t );

private:
	/* State changed callback function*/
	level_changed_cb_t  LevelChangedCbFunc = NULL;
	
	/* Used to make callback function available inside class*/
	static void static_internal_gpio_callback(int pin, int level, uint32_t tick, void* userdata);
	void internal_gpio_callback(int pin, int NewLevel, uint32_t CurrentTick);
	
	void Init();
};


#endif //_GPIO_H
