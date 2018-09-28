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

void PushButtonOnStateChanged(PushButtonState NewState)
{
	static uint32_t LastTicks = Vfb_GetMicros();
	cout <<"[" << (Vfb_GetMicros()  - LastTicks) << "] Button state changed: " << ((NewState==PushButtonState::DOWN)?"DOWN":"UP") << std::endl;
	LastTicks = Vfb_GetMicros();
}

int main()
{
	Vfb_GpioInitialise();
	
	/* PWM stuff */
	Vfb_SetPinMode(PWM_PIN, PinMode::OUTPUT);
	cout << "Freq PWM: " << Vfb_InitPwm(PWM_PIN, PWM_FREQ) << endl;
	Vfb_PwmOut(PWM_PIN, 128);
	
	/* Push button */
	PushButton button(INT_PIN, 500000);
	//button.SetPullState(PullState::UP);
	button.SetReversedPolarity(false);
	button.SetStateChangedCallback( PushButtonOnStateChanged );
	
	bool toogleFlag = false;
	while(1)
	{
		this_thread::sleep_for( chrono::milliseconds(1) );
	}
}