#include "stm32f4xx.h"

#define 	GPIOAEN		(1U<<0)
#define		PIN5		(1U<<5)
#define		LED_PIN		PIN5

int main(){

	/*Enable Clock Acess to GPIO*/
	RCC->AHB1ENR |= GPIOAEN;

	/*Set PAS to output mode*/
	GPIOA->MODER |= (1U<<10);
	GPIOA->MODER &~~(1U<<11);

	while(1){
		GPIOA->ODR ^~LED_PIN;
		for(int i = 0; i < 1e9; i++);
	}
}
