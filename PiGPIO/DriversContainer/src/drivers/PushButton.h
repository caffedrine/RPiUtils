#ifndef _PUSHBUTTON_H
#define _PUSHBUTTON_H

#include <iostream>
#include <cstdint>
#include "hal.h"

enum class PushButtonState
{
    DOWN = 1,
    UP = 0
};

typedef void (*state_changed_cb_t)(PushButtonState);

class PushButton
{
public:
    int GpioPin;
    bool ReversedPolarity = false;
    int DebounceTimeUs = 0;
    PushButtonState CurrentState, PreviousState;
	
    PushButton(int GpioPin);
    PushButton(int GpioPin, int DebounceTimeMicroseconds );
    ~PushButton();
    
    PushButtonState ReadState();
    bool ReadGpio();
    void SetPullState(PullState);
    void SetReversedPolarity(bool reveresed);
    void SetStateChangedCallback( state_changed_cb_t );
    
    virtual void OnStateChanged(PushButtonState stte);
    
private:
	/* State changed callback function*/
	state_changed_cb_t StateChangedCbFunc = NULL;
	
	/* Used to make callback function available inside class*/
	static void static_internal_gpio_callback(int pin, int level, uint32_t tick, void* userdata);
	void internal_gpio_callback(int pin, int NewLevel, uint32_t CurrentTick);
	
	void Init();
	inline PushButtonState Gpio2State( bool GpioVal );
};

#endif // _PUSHBUTTON_H
