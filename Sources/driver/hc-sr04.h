#ifndef HCSR04_H
#define HCSR04_H
#include <map>
#include <miosix.h>

namespace miosix
{

	/**
	 * 
	 *	This class is designed to control up to 3 HCSR04 sensors. This class is thread-safe
	 * 
	 */
	class hcsr04
	{
	
		public:
			/**
			* This function is called to return an instance of the class depending on the timer chosen, 
			* if no instance, that use that timer, exists it will be created.
			* The echo and trigger pins, on board STM32F405** and STM32F407**, have to connected as follow:
			*
			*	-TIM3
			*		echo:    	PC6
			*		trigger: 	PC7
			*
			*	-TIM4
			*		echo:    	PB6
			*		trigger: 	PB7
			*
			*	-TIM5
			*		echo:    	PA0
			*		trigger: 	PA2
			*
			* \return an instance of the class
			* \param _timer which timer to use 
			*  
			*/
			static hcsr04* getInstance(int _timer);
			
			/**
			* Function used to retrieve distance read by the sensor. To avoid interferences between measurements
			* calls have to be spaced by at least 50-60ms
			* \return the distance read by the sensor
			*/
			float getDistance();
			
		private:
			FastMutex mutex;///<Mutex to protect from concurrent access
			int timer;///< Number of the hardware timer used
			GpioPin echo_pin;///<Pin to which the echo pin is connected
			GpioPin trigger_pin;///<Pin to which the trigger pin is connected
			
			/**
			* Constructor that set the timer to be used with the sensor.Calling the constructor publicly 
    			* is not allowed so the constructor is declared private.
			*/
			hcsr04(int _timer);
			
			/**
			* Function used to wait for timer interrupt
			*/
			void waitForSensor();
			/**
			* \return the value needed by timer prescaler to get 1MHz counter update frequency
			*/
			unsigned int getPrescaler();
			/**
			* \param _timer number of the timer used by the sensor
			* \return the pin to which the echo pin needs to be connected depending on timer number and board
			*/
			GpioPin getEchoPortNumber(int _timer);
			/**
			* \param _timer number of the timer used by the sensor
			* \return the pin to which the trigger pin needs to be connected depending on timer number and board
			*/
			GpioPin  getTriggerPortNumber(int _timer);
			
			

	};
}//namespace miosix

#endif //HCSRO4_H
