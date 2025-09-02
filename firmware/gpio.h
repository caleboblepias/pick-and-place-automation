#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "stm32f10x.h"

void GPIO_init_output(GPIO_TypeDef* port, uint8_t pin);

void GPIO_init_pud_input(GPIO_TypeDef* port, uint8_t pin);

uint8_t GPIO_read_IDR(GPIO_TypeDef* port, uint8_t pin);

void GPIO_set(GPIO_TypeDef* port, uint8_t pin);

void GPIO_clear(GPIO_TypeDef* port, uint8_t pin);

#endif // GPIO_H
