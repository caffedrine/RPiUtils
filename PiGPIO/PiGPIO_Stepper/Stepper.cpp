#include <iostream>
#include "Stepper.h"

Stepper::Stepper(uint8_t Pulse, uint8_t Feedback, uint8_t Direction)
{
	this->PulsePin = Pulse;
	this->FeedbackPin = Feedback;
	this->DirectionPin = Direction;
	this->Init();
}

Stepper::Stepper(uint8_t Pulse, uint8_t Feedback, uint8_t Direction, uint8_t Enable)
{
	this->PulsePin = Pulse;
	this->FeedbackPin = Feedback;
	this->DirectionPin = Direction;
	this->EnablePin = Enable;
	Vfb_SetPinMode(this->EnablePin, PinMode::OUTPUT);
	this->Init();
}

Stepper::~Stepper()
{

}

void Stepper::Init()
{
	Vfb_SetPinMode( this->PulsePin, PinMode::OUTPUT);
	Vfb_SetPinMode( this->FeedbackPin, PinMode::INPUT);
	
	/* Feedback function callback */
	Vfb_SetGpioCallbackFunc(this->FeedbackPin, static_internal_step_callback, this);
}

long Stepper::PwmConfig(unsigned Frequency, uint8_t DutyProcents)
{
	this->PwmFreq = Frequency;
	this->PwmDutyProcents = DutyProcents;
	
	/* Set up pwm frequeny */
	long result = Vfb_InitPwm(this->PulsePin, this->PwmFreq);
	/* Stop motor until next command for safety reasons */
	Vfb_PwmOut(this->PulsePin, 0 );
	
	return result;
}

long Stepper::map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void Stepper::Stop()
{
	this->StepsToDo = 0;
	this->StepsDone = 0;
	
	if(!this->IsRunning())
		return;
	this->SetState(StepperState::STOPPED);
	Vfb_PwmOut(this->PulsePin, 0);
}

void Stepper::Run()
{
	this->SetState(StepperState::RUNNING);
	this->StepsToDo = 0;
	this->StepsDone = 0;
	Vfb_PwmOut(this->PulsePin, (uint8_t)map(this->PwmDutyProcents, 0, 100, 0, 255) );
}

void Stepper::SetDirection(StepperDirection NewDirection)
{
	Vfb_WriteGpio(this->DirectionPin, (LogicalLevel)NewDirection);
}

void Stepper::SetState(StepperState _currentState)
{
	this->LastState = this->CurrentState;
	this->CurrentState = _currentState;
}

bool Stepper::IsRunning()
{
	return !(CurrentState == StepperState::STOPPED);
}

void Stepper::RunSteps(long steps)
{
	this->SetState(StepperState::RUNNING_STEPS);
	this->StepsToDo = steps;
	this->StepsDone = 0;
	Vfb_PwmOut(this->PulsePin, (uint8_t)map(this->PwmDutyProcents, 0, 100, 0, 255) );
}

void Stepper::static_internal_step_callback(int pin, int level, uint32_t tick, void *userdata)
{
	reinterpret_cast<Stepper*>(userdata)->internal_step_callback(pin, level, tick);
}

void Stepper::internal_step_callback(int pin, int NewLevel, uint32_t CurrentTick)
{
	/* Not our interrupt */
	if( NewLevel == 2  || pin != this->FeedbackPin)
		return;
	
	/* Count only high pulses */
	if( NewLevel == 0 )
		return;
	
	/* Count only when RunSteps() was called */
	if(this->CurrentState != StepperState::RUNNING_STEPS)
		return;
	
	/* At this point one step was made */
	StepsDone++;
	
	if( this->StepsToDo > 0 && this->StepsDone == StepsToDo)
	{
		Vfb_PwmOut(this->PulsePin, 0);
		SetState(StepperState::STOPPED);
		
		if(StepsDoneCb > 0)
			StepsDoneCb(StepsDone);
	}
}

void Stepper::SetStepsDoneCallback(steps_finished_t f)
{
	this->StepsDoneCb = f;
}
