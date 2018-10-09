#ifndef DRIVERSCONTAINER_MOTOR_H
#define DRIVERSCONTAINER_MOTOR_H

#include "drivers/Stepper.h"
#include "Config.h"
class Motor : public Stepper
{
public:
	Motor() : Stepper(MOTOR_PWM_PIN, MOTOR_DIRECTION_PIN, MOTOR_ENABLE_PIN)
	{
	
	}
	
	void OnStepsDone() override
	{
		std::cout << "Motor steps done: " << StepsDone << std::endl;
	}
	
private:
};

extern Motor g_Motor;

#endif //DRIVERSCONTAINER_MOTOR_H
