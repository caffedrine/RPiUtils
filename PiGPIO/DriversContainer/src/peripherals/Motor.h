#ifndef DRIVERSCONTAINER_MOTOR_H
#define DRIVERSCONTAINER_MOTOR_H

#include <iostream>
#include "drivers/Stepper.h"
#include "Config.h"
class Motor : public Stepper
{
public:
	Motor() : Stepper(MOTOR_PWM_PIN, MOTOR_DIRECTION_PIN, MOTOR_ENABLE_PIN)
	{
		PwmConfig(1000, 10);	/* 1000Hz - 10% duty */
	}
	
	void OnStepsDone() override
	{
		std::cout << "Motor steps done: " << StepsDone << std::endl;
	}
	
private:
};

Motor g_Motor;

#endif //DRIVERSCONTAINER_MOTOR_H
