#ifndef HC-SR04_H
#define HC-SR04_H
#define WANTED_FREQ 1000000
#define PERIOD 10000

#include "interfaces/gpio.h"

namespace miosix
{

	/**
	 * 
	 *	Class to control an HC-SR04 ultrasonic sensor
	 * 
	 */
	class hcsr04
	{
	
		public:
			static hcsr04* getInstance();
			getDistance();
		
		private:
		
			hcsr04();
			
			
			static hcsr04* instance;
			

	};
}//namespace miosix

#endif //HC-SRO4_H
