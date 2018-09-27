#include "PushButton.h"
#include "hal.h"

PushButton::PushButton(int _GpioPin)
{
	this->GpioPin = _GpioPin;
	this->Init();
}

PushButton::PushButton(int _GpioPin, bool _reversed)
{
	this->GpioPin = _GpioPin;
	this->ReversedPolarity = _reversed;
	this->Init();
}

PushButton::PushButton(int _GpioPin, bool _reversed, bool _debounce)
{
	this->GpioPin = _GpioPin;
	this->ReversedPolarity = _reversed;
	this->Debouncer = _debounce;
	this->Init();
}

PushButton::~PushButton()
{

}

void PushButton::Init()
{
	/* Make sure PiGpio is initialized */
	//Vfb_GpioInitialise();
	
	/* Set pin as input */
	Vfb_SetPinMode(this->GpioPin, INPUT);
	
	if( this->ReversedPolarity )
	{
		Vfb_SetPullUpDown(this->GpioPin, PULL_UP);
	}
	else
	{
		Vfb_SetPullUpDown(this->GpioPin, PULL_DOWN);
	}

	/* Enable interruptions on pin state changed */
	//gpioSetAlertFunc(this->GpioPin, (this->*func) );
	
	this->ReadState();
}

bool PushButton::ReadGpio()
{
	return (bool) (!Vfb_ReadGpio(GpioPin));
}

button_state_t PushButton::ReadState()
{
	PreviousState = CurrentState;
	CurrentState = (button_state_t) this->ReadGpio();
	
	if( CurrentState != PreviousState )
	{
		/* State changed callback function*/
		if(StateChangedCbFunc > 0)
			StateChangedCbFunc(CurrentState);
	}
	return CurrentState;
}

void PushButton::SetStateChangedCallback( state_changed_cb_t f, gpio_callback_t g)
{
	StateChangedCbFunc = f;
	gpioSetAlertFunc(this->GpioPin, g );
}

void PushButton::internal_gpio_callback(int pin, int level, uint32_t tick)
{
	if( pin != this->GpioPin )
		return;
	
		
	std::cout << "Interrupt triggered!" << std::endl;
	
	if(StateChangedCbFunc > 0)
		StateChangedCbFunc(CurrentState);
}
