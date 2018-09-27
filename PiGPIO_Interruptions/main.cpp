#include <iostream>
#include <chrono>
#include <thread>

#include "hal.h"
#include "PushButton.h"

using namespace std;

#define DEBOUNCE_US	1000	/* 1 ms */
#define PWM_PIN		21
#define PWM_FREQ	4000
#define INT_PIN		20

void gpio_callback(int pin, int NewLevel, uint32_t CurrentTicks)
{
	if( NewLevel == 2 || pin != INT_PIN )
		return;
	
	static bool CurrentState = 0, LastState = 0;
	static uint32_t LastTicks = 0;
	static int counter = 0;
	
	if( NewLevel != CurrentState )
	{
		if(CurrentTicks - LastTicks >= DEBOUNCE_US)
		{
			LastState = CurrentState;
			CurrentState = NewLevel;
			
			cout << "[" << counter << "][" << CurrentTicks - LastTicks << "] New GPIO state: " << CurrentState << endl;
			
			LastTicks = CurrentTicks;
		}
	}
}

void PushButtonOnStateChanged(button_state_t NewState)
{
	static uint32_t LastTicks = Vfb_GetMicros();
	cout <<"[" << (Vfb_GetMicros()  - LastTicks) << "] Button state changed: " << ((NewState==PushButtonState::DOWN)?"DOWN":"UP") << std::endl;
	LastTicks = Vfb_GetMicros();
}

int main()
{
	Vfb_GpioInitialise();
	
	/* PWM stuff */
	Vfb_SetPinMode(PWM_PIN, OUTPUT);
	cout << "Freq PWM: " << Vfb_InitPwm(PWM_PIN, PWM_FREQ) << endl;
	Vfb_PwmOut(PWM_PIN, 128);
	
	/* Interruption pin state */
	//gpioSetAlertFunc(INT_PIN, gpio_callback);
	
	/* Push button */
	PushButton button(INT_PIN, false, false);
	gpio_callback_t TmpStaticCallback = button.internal_gpio_callback;
	button.SetStateChangedCallback( PushButtonOnStateChanged, gpio_callback);
	
	bool toogleFlag = false;
	while(1)
	{
		//button.ReadState();
		
		this_thread::sleep_for( chrono::milliseconds(1) );
	}
}