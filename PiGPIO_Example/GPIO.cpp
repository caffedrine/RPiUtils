#include <chrono>
#include <thread>
#include <iostream>

#include "hal.h"

using namespace std;
using namespace this_thread;
using namespace chrono;

#define PIN 20

int main(int argc, char *argv[])
{
   cout << "--- start --- " << endl;
      if (Vfp_GpioInitialize() < 0)
      {
         cout << "Failed to initialize" << endl; 
         return 1;
      }

      Vfb_SetPinMode(PIN, OUTPUT);

      bool toggleFlag = false;
      while(1)
      {
         if(toggleFlag)
         {
            Vfb_WriteGpio(PIN, HIGH);
            toggleFlag = false;
         }
         else
         {
            Vfb_WriteGpio(PIN, LOW);
            toggleFlag = true;
         }

         sleep_for( microseconds(100) );
      }

   gpioTerminate();
}

