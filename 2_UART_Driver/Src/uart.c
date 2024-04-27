#include "uart.h"

#define GPIOAEN			(1U<<0)
#define UART2EN			(1U<<17)
#define CR1_EN			(1U<<3)
#define CR1_UE			(1U<<13)
#define USART_SR_TX3	(1U<<7)

#define SYS_FREQ		16e6
#define APB1			SYS_FREQ

#define UART_BAUDRATE	115200


static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate);
static uint16_t uart2_set_baudrate(uint32_t periph_clk, uint32_t baudrate);
void uart2_write(int ch);

int __io_putchar(int ch){
	uart2_write(ch);

	return ch;
}

void uart2_tx_init(void){
	/*****Configure UART GPIO pin*****/
	/*Enable Clock acess to GPIOA*/
	RCC->AHB1ENR |= GPIOAEN;
	/*Set PA2 mode to alternate function mode*/
		//set [10] in 4 and 5 position register, referent to MODER2
	GPIOA->MODER &=    ~(1U<<4);
	GPIOA->MODER |=  	(1U<<5);
	/*Set PA2 alternate function type to UART_TX(AF07), referent to AFR2*/
	GPIOA->AFR[0] |=   ~(1U<<8)|
						(1U<<9)|
						(1U<<10);
	GPIOA->AFR[0] &=   ~(1U<<11);

	/*****Configure UART*****/
	/*Enable clock acess to UART2*/
	RCC->APB1ENR |=		UART2EN;
	/*Configure baudrate*/
	uart2_set_baudrate(APB1,UART_BAUDRATE);
	/*Configure the transfer direction*/
	USART2->CR1 =		CR1_EN;
	/*Enable UART module*/
	USART2->CR1 |=		CR1_UE;
}

void uart2_write(int ch){
	//Make sure transmit data register is empty
	while(!(USART2->SR & USART_SR_TX3)){}
	//Write to the transmit data register
	USART2->DR =  (uint8_t) ch;
}

static uint16_t compute_uart_bd(uint32_t periph_clk, uint32_t baudrate){
	return  ((periph_clk + (baudrate/2U))/baudrate);
}

static uint16_t uart2_set_baudrate(uint32_t periph_clk, uint32_t baudrate){
	//USART2 points to baudrate register and receive compute baudrate
	USART2->BRR = compute_uart_bd(periph_clk, baudrate);
	return 0;
}
