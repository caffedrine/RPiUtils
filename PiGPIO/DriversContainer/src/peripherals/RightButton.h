#ifndef DRIVERSCONTAINER_RIGHTBUTTON_H
#define DRIVERSCONTAINER_RIGHTBUTTON_H

#include "drivers/PushButton.h"

#include "Config.h"

class RightButton : public PushButton
{
public:
	RightButton() : PushButton(RIGHT_BUTTON_PIN)
	{
	
	}
	
	void OnStateChanged(PushButtonState new_state) override
	{
		std::cout << "Right button: " << (new_state==PushButtonState::UP?"UP":"DOWN")  << std::endl;
	}
private:
};

RightButton g_RightButton;

#endif //DRIVERSCONTAINER_RIGHTBUTTON_H
