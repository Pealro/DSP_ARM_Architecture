#include "systick.h"

#define _RELOAD_VALUE_	0x00FFFFFF
#define CTRL_CLKSRC		(1U<<2)
#define CTRL_ENABLE		(1U<<0)

void systick_counter_init(void){
	//Disable sistick before config
	SysTick->CTRL = 0;
	//1. Program reload value with the maximum value.
	SysTick->LOAD = _RELOAD_VALUE_;
	//2. Clear current value.
	SysTick->VAL = 0;
	//Select internal clock source
	SysTick->CTRL |= CTRL_CLKSRC;
	//3. Program Control and Status register.
	SysTick->CTRL |= CTRL_ENABLE;
}
