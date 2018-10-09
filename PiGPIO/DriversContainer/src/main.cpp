#include <iostream>
#include <signal.h>
#include <string.h>
#include <chrono>
#include <thread>

#include "Common.h"
#include "communication/ServerTCP.h"
#include "drivers/hal.h"
#include "peripherals/RightButton.h"
#include "peripherals/LeftButton.h"
#include "peripherals/Motor.h"

bool _ProgramContinue = true;

void OnExit()
{
	g_Motor.Stop();
	std:: cout << "Executing STOP routine..." << std::endl;
}

void SigHandler(int signum)
{
	std::cout << "\nInterrupt signal (" << strsignal(signum) << " - " << signum << ") received.\n";
	OnExit();
	_ProgramContinue = false;
	exit(signum);
}

void Initialize()
{
	/* Handle interruption signals */
	signal(SIGINT, SigHandler);
	signal(SIGQUIT, SigHandler);
	signal(SIGTSTP, SigHandler);
}

int main()
{
	std::cout << "---STARTED---" << std::endl;
	Initialize();
	
	while( true )
	{
		if(!_ProgramContinue)
			break;
		
		std::this_thread::sleep_for( std::chrono::milliseconds(1) );
	}
	return 0;
}