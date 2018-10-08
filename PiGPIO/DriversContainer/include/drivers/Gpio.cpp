#include "Gpio.h"

Gpio::Gpio(int GpioPin)
{
	this->Pin = GpioPin;
	this->Init();
}

Gpio::Gpio(int GpioPin, int DebounceTimeMicroseconds)
{
	this->Pin = GpioPin;
	this->DebounceTimeUs = DebounceTimeMicroseconds;
	this->Init();
}

Gpio::~Gpio()
{

}

void Gpio::Init()
{
	Vfb_SetPinMode(this->Pin, PinMode::INPUT);
	Vfb_SetPullUpDown(this->Pin, PullState::UP);
	
	Vfb_SetGpioCallbackFunc(this->Pin, &Gpio::static_internal_gpio_callback, this );
}

LogicalLevel Gpio::Read()
{
	this->PreviousState = this->CurrentState;
	this->CurrentState = (LogicalLevel)Vfb_ReadGpio(this->Pin);
	
	if(this->CurrentState != this->PreviousState)
	{
		if(this->LevelChangedCbFunc > 0)
			this->LevelChangedCbFunc(this->CurrentState);
	}
	
	return CurrentState;
}

void Gpio::SetPullState(PullState ps)
{
	Vfb_SetPullUpDown(this->Pin, ps);
}

void Gpio::SetReversedPolarity(bool reveresed)
{
	this->ReversedPolarity = reveresed;
}

void Gpio::SetStateChangedCallback(level_changed_cb_t f)
{
	this->LevelChangedCbFunc = f;
}

void Gpio::static_internal_gpio_callback(int pin, int level, uint32_t tick, void* userdata)
{
	reinterpret_cast<Gpio*>(userdata)->internal_gpio_callback(pin, level, tick);
}

void Gpio::internal_gpio_callback(int pin, int NewLevel, uint32_t CurrentTicks)
{
	if( NewLevel == 2  || pin != this->Pin)
		return;
	
	static uint32_t LastTicks = 0;
	
	if(  (LogicalLevel)NewLevel != CurrentState )
	{
		if(CurrentTicks - LastTicks >= this->DebounceTimeUs)
		{
			PreviousState = CurrentState;
			CurrentState = (LogicalLevel)NewLevel;
			
			if(this->LevelChangedCbFunc > 0)
				this->LevelChangedCbFunc(CurrentState);
			
			LastTicks = CurrentTicks;
		}
	}
}




