#ifndef HC-SR04_H
#define HC-SR04_H
#define WANTED_FREQ 1000000
#define PERIOD 10000



namespace miosix
{

	/**
	 * 
	 *	Class to control an HC-SR04 ultrasonic sensor using TIM3.
	 *	The sensor pins need to be connected as follows:
	 *		
	 *		-Trig connected to PC7;
	 *		-Echo connected to PC6.
	 * 
	 */
	class hcsr04
	{
	
		public:
			static hcsr04* getInstance();
			getDistance();
		
		private:
		
			hcsr04();
			unsigned int getPrescaler();
			
			static hcsr04* instance;
			

	};
}//namespace miosix

#endif //HC-SRO4_H
