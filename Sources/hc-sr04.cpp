#include "hc-sr04.h"
#include <stdef.h>

// Global static pointer used to ensure a single instance of the class
hc-sr04* hc-sr04::instance=NULL;


/** This function is called to create an instance of the class. Calling the constructor publicly 
    is not allowed so the constructor is declared private 
*/
static hc-sr04* hc-sr04::getInstance()
{
	if(!instance)
		instance=new hcsr04;
	return instance;
		
}

//Constructor

hc-sr04::hc-sr04()
{
	typedef Gpio<GPIOC_BASE,6>  echo;
	typedef Gpio<GPIOC_BASE,7>  trigger;
	{
	    FastInterruptDisableLock dLock;
	    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	    
	    trigger::mode(Mode::OUTPUT);
	    echo::mode(Mode::ALTERNATE);
	    echo::alternateFunction(2);
	    RCC_SYNC();
	}
	
	
	TIM3->CNT=0; 
	TIM3->PSC=8-1;//TODO 
	TIM3->ARR=10-1;//TODO
	TIM3->EGR=TIM_EGR_UG; 
	TIM3->SR=0;
	
	 TIM3->CCMR1= (TIM3->CCMR1 & ~TIM_CCMR1_CC1S_1) | TIM_CCMR1_CC1S_0 | (TIM3->CCMR1 & ~TIM_CCMR1_IC1F_3) | ((TIM3->CCMR1 & ~TIM_CCMR1_IC1F_2) | TIM_CCMR1_IC1F_1 | TIM_CCMR1_IC1F_0);
    TIM3->CCER= (TIM3->CCER & ~TIM_CCER_CC1P) | (TIM3->CCER & ~TIM_CCER_CC1NP) | TIM_CCER_CC1E; 
	
	TIM3->DIER = TIM_DIER_UIE;
	NVIC_SetPriority(TIM3_IRQn,2);
	NVIC_EnableIRQ(TIM3_IRQn);
	
}
