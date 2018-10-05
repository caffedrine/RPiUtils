#include <iostream>
#include <chrono>
#include <thread>

#include "hal.h"
#include "Stepper.h"

using namespace std;

#define STEPPER_PULSE		23
#define STEPPER_FEEDBACK	20
#define STEPPER_DIRECTION	0
#define STEPPER_ENABLE		0

void OnStepsDone(long Steps)
{
	cout << "Finished to do " << Steps << " steps" << endl;
}

int main()
{
	cout << "---STARTED---" << endl;
	Vfb_GpioInitialise();
	
	/* Stepper */
	Stepper stepper(STEPPER_PULSE, STEPPER_FEEDBACK, STEPPER_DIRECTION);
	stepper.PwmConfig(100, 10);
	stepper.SetStepsDoneCallback( OnStepsDone );
	
	bool toogleFlag = false;
	while(true)
	{
		int result, freq = 1000;
		cout << "Set new frequency: "; cin >> freq;
		result = stepper.PwmConfig(freq, 10);
		stepper.Run();
		cout << "Actual frequncy: " << result << endl << endl;
		
		
		//this_thread::sleep_for( chrono::milliseconds(1) );
	}
}