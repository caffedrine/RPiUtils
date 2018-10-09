#ifndef DRIVERSCONTAINER_RIGHTBUTTON_H
#define DRIVERSCONTAINER_RIGHTBUTTON_H

#include "Config.h"
#include "drivers/PushButton.h"

#include "Motor.h"

class RightButton : public PushButton
{
public:
	RightButton() : PushButton(RIGHT_BUTTON_PIN, 900)
	{
		ReversedPolarity = true;
	}
	
	void OnStateChanged(PushButtonState new_state) override
	{
		std::cout << "Right button: " << (new_state==PushButtonState::UP?"UP":"DOWN")  << std::endl;
		
		if(new_state == PushButtonState::DOWN)
		{
			g_Motor.SetDirection(StepperDirection::BACKWARD);
			g_Motor.Run();
		}
		else
		{
			g_Motor.Stop();
		}
	}
private:
};

RightButton g_RightButton;

#endif //DRIVERSCONTAINER_RIGHTBUTTON_H
