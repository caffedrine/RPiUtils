#include <iostream>
#include <signal.h>
#include <string.h>
#include <chrono>
#include <thread>

#include "Common.h"
#include "peripherals/RightButton.h"
#include "peripherals/LeftButton.h"
//#include "peripherals/Motor.h"

bool _ProgramContinue = true;

void OnExit()
{
	//motor.Stop();
}

void SigHandler(int signum)
{
	std::cout << "Interrupt signal (" << strsignal(signum) << " - " << signum << ") received.\n";
	_ProgramContinue = false;
	OnExit();
	exit(signum);
}

int main()
{
	g_LeftButton.ReadState();
	g_RightButton.ReadState();
	
	std::cout << "---START---" << std::endl;
	signal(SIGINT, SigHandler);
	signal(SIGQUIT, SigHandler);
	signal(SIGTSTP, SigHandler);
	
	while( true )
	{
		if(!_ProgramContinue)
			break;
		
		std::this_thread::sleep_for( std::chrono::milliseconds(1) );
	}
	return 0;
}