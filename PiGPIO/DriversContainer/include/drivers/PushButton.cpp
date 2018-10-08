#include "PushButton.h"
#include "hal.h"

PushButton::PushButton(int _GpioPin)
{
	this->GpioPin = _GpioPin;
	this->Init();
}

PushButton::PushButton(int _GpioPin, int _DebounceTimeMicroseconds)
{
	this->GpioPin = _GpioPin;
	this->DebounceTimeUs = _DebounceTimeMicroseconds;
	this->Init();
}


PushButton::~PushButton()
{

}

void PushButton::Init()
{
	/* Make sure PiGpio is initialized */
	Vfb_GpioInitialise();
	
	/* Set pin as input */
	Vfb_SetPinMode(this->GpioPin, PinMode::INPUT);
	this->SetPullState(PullState::DOWN);
	
	/* Enable interruptions on pin state changed */
	//gpioSetAlertFuncEx(this->GpioPin, &PushButton::static_internal_gpio_callback, this );
	Vfb_SetGpioCallbackFunc(this->GpioPin, &PushButton::static_internal_gpio_callback, this );
	
	this->ReadState();
}

void PushButton::SetPullState(PullState state)
{
	Vfb_SetPullUpDown(this->GpioPin, state);
}

bool PushButton::ReadGpio()
{
	return (bool) (Vfb_ReadGpio(GpioPin));
}

PushButtonState PushButton::ReadState()
{
	PreviousState = CurrentState;
	CurrentState = this->Gpio2State( this->ReadGpio() );
	
	if( CurrentState != PreviousState )
	{
		/* State changed callback function*/
		if(StateChangedCbFunc > 0)
			StateChangedCbFunc(CurrentState);
	}
	return CurrentState;
}

PushButtonState PushButton::Gpio2State(bool GpioVal)
{
	if( this->ReversedPolarity == true )
	{
		return (PushButtonState)(!GpioVal);
	}
	else
	{
		return (PushButtonState)(GpioVal);
	}
}

void PushButton::SetReversedPolarity(bool reveresed)
{
	this->ReversedPolarity = reveresed;
}

void PushButton::SetStateChangedCallback( state_changed_cb_t f)
{
	StateChangedCbFunc = f;
}

void PushButton::static_internal_gpio_callback(int pin, int level, uint32_t tick, void* userdata)
{
	reinterpret_cast<PushButton*>(userdata)->internal_gpio_callback(pin, level, tick);
}

void PushButton::internal_gpio_callback(int pin, int NewLevel, uint32_t CurrentTicks)
{
	if( NewLevel == 2  || pin != this->GpioPin)
		return;
	
	static uint32_t LastTicks = 0;
	
	if(  this->Gpio2State((bool)NewLevel) != CurrentState )
	{
		if(CurrentTicks - LastTicks >= this->DebounceTimeUs)
		{
			PreviousState = CurrentState;
			CurrentState = this->Gpio2State( (bool)NewLevel );
			
			if(this->StateChangedCbFunc > 0)
				StateChangedCbFunc(CurrentState);
			
			LastTicks = CurrentTicks;
		}
	}
}

