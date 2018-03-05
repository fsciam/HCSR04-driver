#include "hc-sr04.h"
#include "interfaces/gpio.h"
#include <miosix.h>
#include "kernel/scheduler/scheduler.h"


using namespace miosix;
using namespace std;

static map<int,hcsr04*> sensors;
static Thread *waiting[3];


/**
 * Timer 3 interrupt handler actual implementation
 */
void __attribute__((used))  tim3impl()
{
   
    	//raising edge
    	if(TIM3->SR & TIM_SR_CC1IF)
	{
		 TIM3->CR1=TIM_CR1_CEN;
	}
    	//falling edge
    	if(TIM3->SR & TIM_SR_CC2IF)
    	{
		waiting[0]->IRQwakeup();
		if(waiting[0]->IRQgetPriority()>Thread::IRQgetCurrentThread()->IRQgetPriority())
			Scheduler::IRQfindNextThread();
		//Stop the 
		TIM3->CR1=0; 
		TIM3->CNT=0;
		waiting[0]=0;	
    	}
    	 TIM3->SR=0; //Clear interrupt flag
    
}

/**
 * Timer 4 interrupt handler actual implementation
 */
void __attribute__((used))  tim4impl()
{
   
    	//raising edge
    	if(TIM4->SR & TIM_SR_CC1IF)
	{
		 TIM4->CR1=TIM_CR1_CEN;
	}
    	//falling edge
    	if(TIM4->SR & TIM_SR_CC2IF)
    	{
		waiting[1]->IRQwakeup();
		if(waiting[1]->IRQgetPriority()>Thread::IRQgetCurrentThread()->IRQgetPriority())
			Scheduler::IRQfindNextThread();
		//Stop the 
		TIM4->CR1=0; 
		TIM4->CNT=0;
		waiting[1]=0;	
    	}
    	 TIM4->SR=0; //Clear interrupt flag
    
}

/**
 * Timer 5 interrupt handler actual implementation
 */
void __attribute__((used))  tim5impl()
{
   
    	//raising edge
    	if(TIM5->SR & TIM_SR_CC1IF)
	{
		 TIM5->CR1=TIM_CR1_CEN;
	}
    	//falling edge
    	if(TIM5->SR & TIM_SR_CC2IF)
    	{
		waiting[2]->IRQwakeup();
		if(waiting[2]->IRQgetPriority()>Thread::IRQgetCurrentThread()->IRQgetPriority())
			Scheduler::IRQfindNextThread();
		//Stop the 
		TIM5->CR1=0; 
		TIM5->CNT=0;
		waiting[2]=0;	
    	}
    	 TIM5->SR=0; //Clear interrupt flag
    
}
/**
 * Timer 3 interrupt handler
 */
void __attribute__((naked))TIM3_IRQHandler()
{
    saveContext();
    asm volatile("bl _Z8tim3implv");
    restoreContext();
}
/**
 * Timer 4 interrupt handler
 */
void __attribute__((naked))TIM4_IRQHandler()
{
    saveContext();
    asm volatile("bl _Z8tim4implv");
    restoreContext();
}
/**
 * Timer 5 interrupt handler
 */
void __attribute__((naked))TIM5_IRQHandler()
{
    saveContext();
    asm volatile("bl _Z8tim5implv");
    restoreContext();
}
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
hcsr04* hcsr04::getInstance(int _timer)
{
	
	if(sensors.find(_timer) != sensors.end())
		return sensors.find(_timer)->second;
	else
	{
		hcsr04 *new_sensor=new hcsr04(_timer);
		sensors.insert(make_pair(_timer,new_sensor));
		return new_sensor;
	}
		
}
/**
* \return the value needed by timer prescaler to get 1MHz counter update frequency
*/
{
 
    unsigned int prescaler=SystemCoreClock;  
    // The timers are connected to APB1 bus. The frequency at which this bus is clocked can be 
    // a submultiple of the CPU frequency.
    // The RCC->CFGR register APB1 bus clock divider. The timers will be clocked at twice the frequency
    // of the bus.
    if(RCC->CFGR & RCC_CFGR_PPRE1_2) 
    	prescaler/=1<<((RCC->CFGR>>10) & 0x3);
    
    return (prescaler/WANTED_FREQ)-1;
}
/**
* \param _timer number of the timer used by the sensor
* \return the pin to which the echo pin needs to be connected depending on timer number and board
*/
GpioPin  hcsr04::getEchoPortNumber(int _timer)
{
	
	
    	
	if (_timer==3)
		return GpioPin(GPIOC_BASE,6);
	else if (_timer==4)
		return GpioPin(GPIOB_BASE,6);
	else 
		return GpioPin(GPIOA_BASE,0);
			
}
/**
* \param _timer number of the timer used by the sensor
* \return the pin to which the trigger pin needs to be connected depending on timer number and board
*/
GpioPin  hcsr04::getTriggerPortNumber(int _timer)
{
	
	
	if (_timer==3)
		return GpioPin(GPIOC_BASE,7);
	else if (_timer==4)
		return GpioPin(GPIOB_BASE,7);
	else 
		return GpioPin(GPIOA_BASE,2);
			
}
/**
* Constructor that set the timer to be used with the sensor.Calling the constructor publicly 
* is not allowed so the constructor is declared private.
*/
hcsr04::hcsr04(int _timer) :timer(_timer), echo_pin(getEchoPortNumber(_timer)), trigger_pin(getTriggerPortNumber(_timer))
{
	
	
	switch(_timer)
	{
		
		case 3:
			
			// Setting pin mode and enable TIM3 clock
			{
			    FastInterruptDisableLock dLock;
			    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
			    
			    trigger_pin.mode(Mode::OUTPUT);
			    echo_pin.mode(Mode::ALTERNATE);
			    echo_pin.alternateFunction(2);
			    RCC_SYNC();
			}
	
			// Setting prescaler and period on the timer. The CNT updating frequency is set to 1MHz.
			// After setting PSC and ARR TIM_EGR_UG bit is set to load values in the registrers
			TIM3->CNT=0; 
			TIM3->PSC=getPrescaler();
			TIM3->ARR=PERIOD;
			TIM3->EGR=TIM_EGR_UG; 
			TIM3->SR=0;
	
			
			// Setting input capture:
			// - CC1 channel configured as input and mappend to TI1;
			// - CC2 channel configured as input and mappend to TI2;
			// - Enable CC1 capture and CC2 capture;
			// - Select active polarity for TI1FP1(active on raising edge) and active polarity for TI1FP2(active on falling edge);
			// - Select the valid trigger input(TI1FP1 selected)
			// - Configure the slave mode controller in reset mode
			TIM3->CCMR1= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_1;
		    	TIM3->CCER= TIM_CCER_CC2P | TIM_CCER_CC1E | TIM_CCER_CC2E;
		    	TIM3->SMCR= TIM_SMCR_TS_2 | TIM_SMCR_TS_0 | TIM_SMCR_SMS_2;
		    
		    	// enable interrupt for update event, cc1 and cc2
		    	TIM3->DIER= TIM_DIER_UIE | TIM_DIER_CC1IE | TIM_DIER_CC2IE;
		    	NVIC_SetPriority(TIM3_IRQn,4);
		    	NVIC_EnableIRQ(TIM3_IRQn);
		   	break;
		case 4:
			
			//Setting pin mode and enable TIM4 clock
			{
			    FastInterruptDisableLock dLock;
			    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
			    
			    trigger_pin.mode(Mode::OUTPUT);
			    echo_pin.mode(Mode::ALTERNATE);
			    echo_pin.alternateFunction(2);
			    RCC_SYNC();
			}
	
			// Setting prescaler and period on the timer. The CNT updating frequency is set to 1MHz.
			// After setting PSC and ARR TIM_EGR_UG bit is set to load values in the registrers
			TIM4->CNT=0; 
			TIM4->PSC=getPrescaler();
			TIM4->ARR=PERIOD;
			TIM4->EGR=TIM_EGR_UG; 
			TIM4->SR=0;
	
			
			// Setting input capture:
			// - CC1 channel configured as input and mappend to TI1;
			// - CC2 channel configured as input and mappend to TI2;
			// - Enable CC1 capture and CC2 capture;
			// - Select active polarity for TI1FP1(active on raising edge) and active polarity for TI1FP2(active on falling edge);
			// - Select the valid trigger input(TI1FP1 selected)
			// - Configure the slave mode controller in reset mode
			TIM4->CCMR1= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_1;
		    	TIM4->CCER= TIM_CCER_CC2P | TIM_CCER_CC1E | TIM_CCER_CC2E;
		    	TIM4->SMCR= TIM_SMCR_TS_2 | TIM_SMCR_TS_0 | TIM_SMCR_SMS_2;
		    
		    	//enable interrupt for update event, cc1 and cc2
		    	TIM4->DIER= TIM_DIER_UIE | TIM_DIER_CC1IE | TIM_DIER_CC2IE;
		    	NVIC_SetPriority(TIM4_IRQn,4);
		    	NVIC_EnableIRQ(TIM4_IRQn);
		   	break;
		case 5:
			
			//Setting pin mode and enable TIM5 clock
			{
			    FastInterruptDisableLock dLock;
			    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
			    
			    trigger_pin.mode(Mode::OUTPUT);
			    echo_pin.mode(Mode::ALTERNATE);
			    echo_pin.alternateFunction(2);
			    RCC_SYNC();
			}
	
			// Setting prescaler and period on the timer. The CNT updating frequency is set to 1MHz.
			// After setting PSC and ARR TIM_EGR_UG bit is set to load values in the registrers
			TIM5->CNT=0; 
			TIM5->PSC=getPrescaler();
			TIM5->ARR=PERIOD;
			TIM5->EGR=TIM_EGR_UG; 
			TIM5->SR=0;
	
			// Setting input capture:
			// - CC1 channel configured as input and mappend to TI1;
			// - CC2 channel configured as input and mappend to TI2;
			// - Enable CC1 capture and CC2 capture;
			// - Select active polarity for TI1FP1(active on raising edge) and active polarity for TI1FP2(active on falling edge);
			// - Select the valid trigger input(TI1FP1 selected)
			// - Configure the slave mode controller in reset mode
			TIM5->CCMR1= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_1;
		    	TIM5->CCER= TIM_CCER_CC2P | TIM_CCER_CC1E | TIM_CCER_CC2E;
		    	TIM5->SMCR= TIM_SMCR_TS_2 | TIM_SMCR_TS_0 | TIM_SMCR_SMS_2;
		    
		    	//enable interrupt for update event, cc1 and cc2
		    	TIM5->DIER= TIM_DIER_UIE | TIM_DIER_CC1IE | TIM_DIER_CC2IE;
		    	NVIC_SetPriority(TIM5_IRQn,4);
		    	NVIC_EnableIRQ(TIM5_IRQn);
		   	break;
		 }
	
}


/**
* Function used to wait for timer interrupt
*/
void hcsr04::waitForSensor()
{	FastInterruptDisableLock dLock;
	waiting[timer-3]=Thread::IRQgetCurrentThread();
    do {
        Thread::IRQwait();
        {
            FastInterruptEnableLock eLock(dLock);
            Thread::yield();
        }
    } while(waiting[timer-3]);
}
/**
* Function used to retrieve distance read by the sensor. To avoid interferences between measurements
* calls have to be spaced by at least 50-60ms
* \return the distance read by the sensor
*/			
float hcsr04::getDistance()
{	Lock<FastMutex> l(mutex);
	// Send a 10us puls on trigger_pin to start the sensor
	{
		FastInterruptDisableLock dLock;
		trigger_pin.high();
		delayUs(10);
		trigger_pin.low();
	}
	
	waitForSensor();
	float travel_time;
	
	if(timer==3)
		travel_time=TIM3->CCR2;
	else if(timer==4)
		travel_time=TIM4->CCR2;
	else 
		travel_time=TIM5->CCR2;
	// The travel_time variable contains travel time of the ultrasonic wave in us, to obtain
	// the distance in cm we need to multiply by 0.0343(distance covered by sound in 1us) and 
	// divide by 2. 
	return 0.0343*travel_time/2;
}

