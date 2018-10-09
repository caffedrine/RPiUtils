//
// Created by curiosul on 10/9/18.
//

#ifndef DRIVERSCONTAINER_LEFTBUTTON_H
#define DRIVERSCONTAINER_LEFTBUTTON_H

#include "Config.h"
#include "drivers/PushButton.h"

#include "RightButton.h"

class LeftButton : public PushButton
{
public:
	LeftButton() : PushButton(LEFT_BUTTON_PIN)
	{
	
	}
	
	void OnStateChanged(PushButtonState new_state) override
	{
		std::cout << "Left button: " << (new_state==PushButtonState::UP?"UP":"DOWN")  << std::endl;
	}
private:
};

LeftButton g_LeftButton;

#endif //DRIVERSCONTAINER_LEFTBUTTON_H
