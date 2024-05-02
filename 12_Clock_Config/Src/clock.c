/*
 * clock.c
 *
 *  Created on: May 1, 2024
 *      Author: LSE
 */
/*
 * System Clock Source = HSE
 * SYSCLK			   = 100MHz
 * HCLK				   = 100MHz
 * AHB Prescaler	   = 1
 * APB1 Prescaler	   = 2
 * APB2 Prescaler	   = 1
 * HSE				   = 8MHz
 * */

#include "clock.h"

#define PLL_M	4
#define PLL_N	200
#define PLL_P	4
#define PLL_Q	9

void clock_100MHz_config(void){
	/*Enable HSE Oscillator*/
	RCC->CR |= RCC_CR_HSEON;

	/*Wait till HSERDY goes LOW*/ /*Tive que comentar pois
	 * a posição RCC_CR_HSERDY nao ia para low após setar RCC_CR_HSEON*/
//	while((RCC->CR & RCC_CR_HSERDY) == (RCC_CR_HSERDY)){}

	/*Set HCLK Prescaler*/
	RCC->CFGR &= ~RCC_CFGR_HPRE;
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

	/*Set APB1 Prescaler*/
	RCC->CFGR &= ~RCC_CFGR_PPRE1;
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

	/*Set APB2 Prescaler*/
	RCC->CFGR &= ~RCC_CFGR_PPRE2;
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

	/*Configure PLL*/
	RCC->PLLCFGR = (PLL_M) 		|
				   (PLL_N << 6) |
				   (((PLL_P >> 1) - 1) << 16)|
				   (PLL_Q << 24)|
				   (RCC_PLLCFGR_PLLSRC_HSE);

	/*Turn PLL ON*/
	RCC->CR |= RCC_CR_PLLON;

	/*Wait for PLL to come ON*/
	while((RCC->CR & RCC_CR_PLLON) == 0){}

	/*Set FLASH latency*/
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |= FLASH_ACR_LATENCY_3WS;

	/*Select the main PLL as the clock source*/
	RCC->CFGR &= RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	/*Wait for main PLL to be used as the clock source*/
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL){}
}


