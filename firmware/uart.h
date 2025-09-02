#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "stm32f10x.h"

const volatile uint8_t* get_rx_buffer(void);

uint8_t get_packet_ready(void);

void set_packet_ready(uint8_t val);

void USART2_init(void);

void USART2_write(uint8_t byte);

void USART2_send_ack(void);

void USART2_send_nack(void);

#endif // UART_H