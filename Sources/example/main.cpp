#include <cstdio>
#include <miosix.h>
#include "hc-sr04.h"

using namespace std;
using namespace miosix;

typedef Gpio<GPIOB_BASE,2>  yellow;
typedef Gpio<GPIOE_BASE,8> red;
typedef Gpio<GPIOB_BASE,0> green;



int main()
{
	
// To make it works, on board STM32F405** and STM32F407**, you have to connect echo and trigger pins as follows:
//		echo:    	PC6
//		trigger: 	PC7
   hcsr04 *sensor=hcsr04::getInstance(3);
    {
    	FastInterruptDisableLock dLock;
    	green::mode(Mode::OUTPUT);
    	yellow::mode(Mode::OUTPUT);
    	red::mode(Mode::OUTPUT);
    }
    
    for(;;)
    {
    
   	
	float space= sensor->getDistance();
	
	printf("distance:\t%.1f\n",space);
	if(space>=0.0 && space<50.0)
		{
			red::high();
			yellow::high();
			green::high();
		}
	else if(space>=50.0 && space<150.0)
		{
			red::low();
			yellow::high();
			green::high();	
		}	
	else
		{
			green::high();
			red::low();
			yellow::low();
		}
	sleep(0.5);
    }
  
  return 0;
  
}


