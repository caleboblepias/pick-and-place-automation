#include <stdint.h>
#include <stdio.h>
#include <stm32f10x.h>
#include "gpio.h"


void GPIO_init_output(GPIO_TypeDef* port, uint8_t pin) {
	if (port == GPIOA) RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	else if (port == GPIOB) RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	else if (port == GPIOC) RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	else if (port == GPIOD) RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
	else if (port == GPIOE) RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;
	
	port->CRL &= ~(0xF << (pin * 4)); // Clear PApin
	port->CRL |= (0x2 << (pin * 4)); // Set PApin to 0b0010 (output, push/pull, max 2 MHz)
	
}

void GPIO_init_pud_input(GPIO_TypeDef* port, uint8_t pin) {
	if (port == GPIOA) RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	else if (port == GPIOB) RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	else if (port == GPIOC) RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	else if (port == GPIOD) RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
	else if (port == GPIOE) RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;
	
	
	port->CRL &= ~(0xF << (pin * 4)); // Clear PApin
	port->CRL |= (0x8 << (pin * 4)); //Set PApin to 0b1000 (input, pull-up pull-down)
	port->ODR |= (1 << pin);
	
}

uint8_t GPIO_read_IDR(GPIO_TypeDef* port, uint8_t pin) {
	
	
	return (port->IDR & (1U << pin)) ? 1 : 0;
}

void GPIO_set(GPIO_TypeDef *port, uint8_t pin) {
	port->BSRR |= (1 << pin);
	
}

void GPIO_clear(GPIO_TypeDef *port, uint8_t pin) {
	port->BSRR |= (1 << (pin + 16));
	
}