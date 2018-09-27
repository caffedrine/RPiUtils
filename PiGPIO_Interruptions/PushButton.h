#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <iostream>
#include <cstdint>
#include "hal.h"

/* 1000us = 1ms debouncing time */
#define DEBOUNCING_TIME_US  1000

typedef enum PushButtonState
{
    DOWN = 1,
    UP = 0
}button_state_t;

typedef void (*state_changed_cb_t)(button_state_t);
typedef void (*gpio_callback_t)(int, int, uint32_t);

class PushButton
{
public:
    int GpioPin;
    bool ReversedPolarity = false;
    bool Debouncer = false;
    button_state_t CurrentState, PreviousState;
	
    PushButton(int GpioPin);
    PushButton(int GpioPin, bool reversed);
    PushButton(int GpioPin, bool reversed, bool debouncer);
    ~PushButton();
    
    button_state_t ReadState();
    bool ReadGpio();
    void SetStateChangedCallback( state_changed_cb_t, gpio_callback_t );
	void internal_gpio_callback(int pin, int level, uint32_t CurrentTick);

private:
	/* State changed callback function*/
	state_changed_cb_t StateChangedCbFunc = NULL;
	
	void Init();
};

#endif // PUSHBUTTON_H
