#include <stdint.h>
#include <stm32f10x.h>
#include "uart.h"

#define USART_RX_BUFFER_SIZE 10

static volatile uint8_t rx_buffer[USART_RX_BUFFER_SIZE];
static volatile uint8_t data_index;
static volatile uint8_t rx_chk;
static volatile uint8_t calc_chk;

enum ParserState {
	START,
	OPCODE,
	LEN,
	DATA,
	CHK,
	END
};

static volatile enum ParserState state = START;
static volatile uint8_t packet_ready = 0;

const volatile uint8_t* get_rx_buffer(void) {
	return rx_buffer;
}

uint8_t get_packet_ready(void) {
	return packet_ready;
}

void set_packet_ready(uint8_t val) {
	packet_ready = val;
}

void USART2_write(uint8_t byte) {
	// Wait for TXE (transmit data register empty bit) to be set
	while(!(USART2->SR & (1 << 7)));
	// Write to transmit data register
	USART2->DR = byte;
}

void USART2_init(void) {
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // Enable USART2 clock
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // Enable GPIOA clock
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; // Enable AFIO clock
	
	// Configure PA2 as AFIO, push-pull output, 50 MHz max
	GPIOA->CRL &= ~(0xF << ((2) * 4));
	GPIOA->CRL |= (0xB << ((2) * 4));
	
	// Configure PA3 as floating input
	GPIOA->CRL &= ~(0xF << ((3) * 4));
	GPIOA->CRL |= (0x4 << ((3) * 4));
	
	// Set USART2 baud rate = 36 MHz / 115200
	USART2->BRR = 36000000 / 115200;
	
	// Enable transmitter
	USART2->CR1 |= USART_CR1_TE;
	
	// Enable receiver
	USART2->CR1 |= USART_CR1_RE;
	
	// Enable USART2
	USART2->CR1 |= USART_CR1_UE;
	
	// Enable RXNEIE (RXNE interrupt enable)
	USART2->CR1 |= USART_CR1_RXNEIE;
	
	// Enable USART IRQ in NVIC
	NVIC_EnableIRQ(USART2_IRQn);
	
	// Configure PA5 in order to turn on LED for testing
	GPIOA->CRL &= ~(0xF << (5 * 4));
	GPIOA->CRL |= (0x2 << (5 * 4));
	GPIOA->BRR = (1 << 5); // Set LD2 low to start for testing
	
}

void USART2_send_ack(void) {
	USART2_write(0x06);
}

void USART2_send_nack(void) {
	USART2_write(0x15);
}

void USART2_IRQHandler(void) {
	// Check that RXNE (Receive Data Register Not Empty) is set
	if (USART2->SR & (1 << 5)) {
		
		uint8_t data = USART2->DR;
		switch(state) {
			
			case START:
				
				if (data == 0xAA) {
					state = OPCODE;
					data_index = 0;
					// Initialize checksum, both calc and rx
					calc_chk = 0;
					rx_chk = 0;
				}
				break;
				
			case OPCODE:
				
				rx_buffer[0] = data;
				state = LEN;
				// XOR checksum
				calc_chk ^= data;
				break;
			
			case LEN:
				
				rx_buffer[1] = data;
				// If len is more than total length - opcode byte - len byte - chk byte
				if (rx_buffer[1] > USART_RX_BUFFER_SIZE - 3) { 
					state = START;
					data_index = 0;
					break;
				}
				else if (rx_buffer[1] == 0) {
					state = CHK;
					// XOR checksum
					calc_chk ^= data;
					break;
				}
				else {
					state = DATA;
					// XOR checksum
					calc_chk ^= data;
				}
				break;
				
			case DATA:
				
				// If data index is greater than or equal to (0-based) total length - opcode byte - len byte - chk byte
				if (data_index >= USART_RX_BUFFER_SIZE - 3) {
					state = START;
					data_index = 0;
					break;
				} else {
					rx_buffer[data_index++ + 2] = data;
					// XOR checksum
					calc_chk ^= data;
				}
				
				if ((data_index) == rx_buffer[1]) {
					GPIOA->BSRR = (1 << 5); // Set LD2 high for testing
					state = CHK;
				}
				break;
				
			case CHK:
				
				// If data index is greater than or equal to (0-based) total length - opcode byte - len byte - chk byte 
				rx_chk = data;
				rx_buffer[rx_buffer[1] + 2] = data;
				state = END;
				break;
				
			case END:
				
				if (data == 0x55 && calc_chk == rx_chk) {
					packet_ready = 1;
					
				}
				state = START;
				data_index = 0;
				break;
				
		}
			
	}
	
}