#include <iostream>
#include <signal.h>
#include <string.h>
#include <chrono>

#include "Common.h"
#include "Button.h"

void OnExit()
{
	//motor.Stop();
}

void SigHandler(int signum)
{
	std::cout << "Interrupt signal (" << strsignal(signum) << " - " << signum << ") received.\n";
	OnExit();
	exit(signum);
}

Button button;

int main()
{
	signal(SIGINT, SigHandler);
	signal(SIGQUIT, SigHandler);
	signal(SIGTSTP, SigHandler);
	
	while( true )
	{
	
	}
	return 0;
}