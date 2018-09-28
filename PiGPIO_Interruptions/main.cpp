#include <iostream>
#include <chrono>
#include <thread>

#include "hal.h"
#include "PushButton.h"
#include "Gpio.h"
#include "Stepper.h"

using namespace std;

#define DEBOUNCE_US	1000	/* 1 ms */
#define PWM_FREQ	4000
#define BUTTON_PIN	19
#define INT_PIN		16

#define STEPPER_PULSE		21
#define STEPPER_FEEDBACK	20
#define STEPPER_DIRECTION	0
#define STEPPER_ENABLE		0

void PushButtonOnStateChanged(PushButtonState NewState)
{
	static uint32_t LastTicks = Vfb_GetMicros();
	cout <<"[" << (Vfb_GetMicros()  - LastTicks) << "] Button state changed: " << ((NewState==PushButtonState::DOWN)?"DOWN":"UP") << std::endl;
	LastTicks = Vfb_GetMicros();
}

void GpioLevelChanged(LogicalLevel NewLogicalLevel)
{
	static uint32_t LastTicks = Vfb_GetMicros();
	cout <<"[" << (Vfb_GetMicros()  - LastTicks) << "] Pin state changed: " << ((NewLogicalLevel==LogicalLevel::HIGH)?"HIGH":"LOW") << std::endl;
	LastTicks = Vfb_GetMicros();
}

void OnStepsDone(long Steps)
{
	cout << "Finished to do " << Steps << " steps" << endl;
}

int main()
{
	cout << "---STARTED---" << endl;
	Vfb_GpioInitialise();
	
	/* Push button */
	PushButton button(BUTTON_PIN, 10000);
	button.SetPullState(PullState::DOWN);
	button.SetReversedPolarity(false);
	button.SetStateChangedCallback( PushButtonOnStateChanged );
	
	/* Gpio pin */
	Gpio pin(INT_PIN, 1000);
	pin.SetStateChangedCallback( GpioLevelChanged );
	
	/* Stepper */
	Stepper stepper(STEPPER_PULSE, STEPPER_FEEDBACK, STEPPER_DIRECTION);
	stepper.PwmConfig(100, 10);
	stepper.SetStepsDoneCallback( OnStepsDone );
	
	bool toogleFlag = false;
	while(true)
	{
		int c = getchar();
		
		if(c == '1') stepper.RunSteps(100);
		else if(c == '2') stepper.Stop();
		else if(c == '3') stepper.Run();
		else if(c == 'q') break;
		
		//this_thread::sleep_for( chrono::milliseconds(1) );
	}
}