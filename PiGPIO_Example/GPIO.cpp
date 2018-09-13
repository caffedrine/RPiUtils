#include <chrono>
#include <thread>
#include <iostream>
#include <pigpio.h>

using namespace std;
using namespace this_thread;
using namespace chrono;

#define PIN 20

int main(int argc, char *argv[])
{
	cout << "--- start --- " << endl;
   	if (gpioInitialise() < 0)
   	{
   		cout << "Failed to initialize" << endl; 
   		return 1;
   	}

   	gpioSetMode(PIN, PI_INPUT);

   	bool toggleFlag = false;
   	while(1)
   	{
   		if(toggleFlag)
   		{
   			gpioWrite(PIN, 1);	
	   		toggleFlag = false;
   		}
   		else
   		{
   			gpioWrite(PIN, 0);
   			toggleFlag = true;
   		}

   		sleep_for( microseconds(10) );
	}

   gpioTerminate();
}

