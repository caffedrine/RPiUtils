#include "Button.h"

void Button::OnStateChanged(PushButtonState new_state)
{
	std::cout << "Buttons new state: " << (new_state==PushButtonState::UP?"UP":"DOWN")  << std::endl;
}
