#ifndef DRIVERSCONTAINER_BUTTON_H
#define DRIVERSCONTAINER_BUTTON_H

#include <drivers/PushButton.h>

#include "Config.h"
//#include "Common.h"

class Button : public PushButton
{
public:
	Button() : PushButton(BUTTON_PIN){}
	void OnStateChanged(PushButtonState new_state) override;
private:
};

#endif //DRIVERSCONTAINER_BUTTON_H
