#include <iostream>
#include <chrono>
#include <thread>

#include "hal.h"
#include "Stepper.h"

using namespace std;

#define STEPPER_PULSE		21
#define STEPPER_DIRECTION	0
#define STEPPER_ENABLE		0

void OnStepsDone(long Steps)
{
	cout << "Finished to do " << Steps << " steps" << endl;
}

/*
       1: 40000 20000 10000 8000 5000 4000 2500 2000 1600
           1250  1000   800  500  400  250  200  100   50

       2: 20000 10000  5000 4000 2500 2000 1250 1000  800
            625   500   400  250  200  125  100   50   25

       4: 10000  5000  2500 2000 1250 1000  625  500  400
            313   250   200  125  100   63   50   25   13
sample
 rate
 (us)  5:  8000  4000  2000 1600 1000  800  500  400  320
            250   200   160  100   80   50   40   20   10

       8:  5000  2500  1250 1000  625  500  313  250  200
            156   125   100   63   50   31   25   13    6

      10:  4000  2000  1000  800  500  400  250  200  160
            125   100    80   50   40   25   20   10    5
 */


int main()
{
	cout << "---STARTED---" << endl;
	Vfb_GpioInitialise();
	
	int rangeResult = gpioSetPWMrange(STEPPER_PULSE, 400);
	cout << "PWM  range: " << rangeResult << endl;
	
	/* Stepper */
	Stepper stepper(STEPPER_PULSE, STEPPER_DIRECTION);
	stepper.PwmConfig(4000, 10);
	stepper.SetStepsDoneCallback( OnStepsDone );
	
	stepper.RunSteps(1000);
	
	bool toogleFlag = false;
	while(true)
	{
//		int result, freq = 1000;
//		cout << "Set new frequency: "; cin >> freq;
//		result = stepper.PwmConfig(freq, 10);
//		stepper.RunSteps(10000);
//		cout << "Actual frequncy: " << result << endl << endl;
		this_thread::sleep_for( chrono::milliseconds(1) );
	}
}