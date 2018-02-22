#include "hc-sr04.h"
#include "interfaces/gpio.h"

// Global static pointer used to ensure a single instance of the class
hc-sr04* hc-sr04::instance=NULL;


//Echo pin and Trigger
typedef Gpio<GPIOC_BASE,6>  echo;
typedef Gpio<GPIOC_BASE,7>  trigger;

/** This function is called to create an instance of the class. Calling the constructor publicly 
    is not allowed so the constructor is declared private 
*/
static hc-sr04* hc-sr04::getInstance()
{
	if(!instance)
		instance=new hcsr04;
	return instance;
		
}
/**
	This function is called to get the value of the Prescaler.
*/
unsigned int getPrescaler()
{
 
    unsigned int prescaler=SystemCoreClock;  
    /**
    	Check clock divider for APB1
    */
    if(RCC->CFGR & RCC_CFGR_PPRE1_2) 
    	prescaler/=1<<((RCC->CFGR>>10) & 0x3);
    
    return (prescaler/WANTED_FREQ)-1;
}
//Constructor

hc-sr04::hc-sr04()
{
	//Setting pin mode and enable TIM3 clock
	{
	    FastInterruptDisableLock dLock;
	    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	    
	    trigger::mode(Mode::OUTPUT);
	    echo::mode(Mode::ALTERNATE);
	    echo::alternateFunction(2);
	    RCC_SYNC();
	}
	
	/*Setting prescaler and period on the timer. The CNT updating frequency is set to 1MHz.
	 After setting PSC and ARR TIM_EGR_UG bit is set to load values in the registrers
	 */
	TIM3->CNT=0; 
	TIM3->PSC=getPrescaler();
	TIM3->ARR=PERIOD;
	TIM3->EGR=TIM_EGR_UG; 
	TIM3->SR=0;
	
	//Setting input capture
	TIM3->CCMR1= TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_1;
    	TIM3->CCER= TIM_CCER_CC2P | TIM_CCER_CC1E | TIM_CCER_CC2E;
    	TIM3->SMCR= TIM_SMCR_TS_2 | TIM_SMCR_TS_0 | TIM_SMCR_SMS_2;
    
    	//enable update for update event, cc1 and cc2
    	TIM3->DIER= TIM_DIER_UIE | TIM_DIER_CC1IE | TIM_DIER_CC2IE;
    	NVIC_SetPriority(TIM3_IRQn,4);
    	NVIC_EnableIRQ(TIM3_IRQn);
	
}
