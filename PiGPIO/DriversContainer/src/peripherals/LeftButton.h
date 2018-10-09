//
// Created by curiosul on 10/9/18.
//

#ifndef DRIVERSCONTAINER_LEFTBUTTON_H
#define DRIVERSCONTAINER_LEFTBUTTON_H

#include "Config.h"
#include "drivers/PushButton.h"

#include "Motor.h"

class LeftButton : public PushButton
{
public:
	LeftButton() : PushButton(LEFT_BUTTON_PIN, 900)
	{
		ReversedPolarity = true;
	}
	
	void OnStateChanged(PushButtonState new_state) override
	{
		std::cout << "Left button: " << (new_state==PushButtonState::UP?"UP":"DOWN")  << std::endl;
		
		if(new_state == PushButtonState::DOWN)
		{
			g_Motor.SetDirection(StepperDirection::FORWARD);
			g_Motor.Run();
		}
		else
		{
			g_Motor.Stop();
		}
		
	}
private:
};

LeftButton g_LeftButton;

#endif //DRIVERSCONTAINER_LEFTBUTTON_H
