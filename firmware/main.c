#include <stdint.h>     // Brings in fixed-width types: uint8_t, uint16_t, etc.
#include "stm32f10x.h"
#include "uart.h"
#include "stepper.h"
#include "gpio.h"
#include "servo.h"
#include "system.h"
#include "calibrate.h"
#include "limit.h"

int main(void)
{
	set_system_state(IDLE);
	USART2_init();
	
	limit_init(0); // Y top limit at PB0
	limit_init(1); // Y bottom limit at PB1
	limit_init(2); // Z top limit at PB2
	limit_init(3); // Z bottom limit at PB3
	
	
	pulse_width_init();
	
	stepper1_init();
	stepper2_init();
	
	set_stepper1_step_freq(6400);
	set_stepper2_step_freq(6400);
	
	set_stepper1_dir(1);
	set_stepper2_dir(0);
	
	//drive_stepper1(800);
	//drive_stepper2(800);
	//calibrate();
	
	servo1_init();
	set_servo1_angle(135);
	/*
	for (uint32_t i = 0; i < 8000; i++) {}
	set_servo1_angle(135);
	for (uint32_t i = 0; i < 8000; i++) {}
	set_servo1_angle(270);
	for (uint32_t i = 0; i < 8000; i++) {}
	set_servo1_angle(135);
	for (uint32_t i = 0; i < 8000; i++) {}
	set_servo1_angle(270);
	*/
	
	//GPIO_init_pud_input(GPIOA, 4);
	
	//servo1_init();
	
	/*
	//set_servo1_angle(90);
	set_servo1_angle(0);
	for(uint32_t i = 0; i < (1000 * 8000); ++i) __asm__("nop");
	set_servo1_angle(135);
	for(uint32_t i = 0; i < (1000 * 8000); ++i) __asm__("nop");
	set_servo1_angle(270);
	for(uint32_t i = 0; i < (1000 * 8000); ++i) __asm__("nop");
	for(uint32_t i = 0; i < 270; ++i) {
		set_servo1_angle(i++);
		for(uint32_t j = 0; j < (100 * 8000); ++j) __asm__("nop");
	}
	set_servo1_angle(0);
	*/
	while(1)
	{
			
		
		/*
		if (!(GPIO_read_IDR(GPIOA, 4))) {
			drive_stepper1(10);
		}
		*/
		
		/*
		if (get_packet_ready()) {
			const volatile uint8_t* rx_buffer = get_rx_buffer();
			uint8_t opcode = rx_buffer[0];
			uint8_t len = rx_buffer[1];

			USART2_write(0xAA);
			USART2_write(opcode);
			USART2_write(len);
			for (uint8_t i = 0; i < len; ++i)
				USART2_write(rx_buffer[2 + i]);
			USART2_write(rx_buffer[2 + len]);  
			USART2_write(0x55);

			set_packet_ready(0);               
		}
		*/
		
		// main handler
		if (get_packet_ready() && get_system_state() == IDLE) {
			const volatile uint8_t* rx_buffer = get_rx_buffer();
			uint8_t opcode = rx_buffer[0];
			uint8_t len = rx_buffer[1];
			
			switch(opcode) {
				
				// 0x01: drive stepper 1
				case 0x01:
					if (len > 1) {
						uint8_t dir = rx_buffer[2];
						uint32_t steps = 0;
						/*
						for (uint8_t i = 0; i < len; ++i) {
							steps += rx_buffer[3 + i];
						}
						*/
						for (uint8_t i = 0; i < len - 1; ++i) { // len = 3, 1 dir + 2 step bytes
							steps |= ((uint32_t)rx_buffer[3 + i]) << (8 * i);
						}


						set_stepper1_dir(dir);
						drive_stepper1(steps);
						set_system_state(RUNNING);
						//USART2_write(0x01);
					}
					set_packet_ready(0);
					//USART2_send_ack();
					break;
					
				// 0x02: drive stepper 2
				case 0x02:
					if (len > 1) {
						uint8_t dir = rx_buffer[2];
						uint32_t steps = 0;
						/*
						for (uint8_t i = 0; i < len; ++i) {
							steps += rx_buffer[3 + i];
						}
						*/
						for (uint8_t i = 0; i < len - 1; ++i) { // len = 3, 1 dir + 2 step bytes
							steps |= ((uint32_t)rx_buffer[3 + i]) << (8 * i);
						}
						set_stepper2_dir(dir);
						drive_stepper2(steps);
						set_system_state(RUNNING);
						//USART2_write(0x02);
					}
					set_packet_ready(0);
					//USART2_send_ack();
					break;
			
				// 0x03: set wrist angle
				case 0x03:
					if (len == 1) {
						uint8_t angle = rx_buffer[2];
						set_servo1_angle(angle);
						USART2_write(0x03);
					}
					set_packet_ready(0);
					USART2_send_ack();
					break;
					
					
				// 0x04: set grabber angle
				case 0x04:
					if (len == 1) {
						uint8_t angle = rx_buffer[2];
						set_servo2_angle(angle);
						USART2_write(0x04);
					}
					set_packet_ready(0);
					USART2_send_ack();
					break;
					
				
				case 0x10:
					set_packet_ready(0);
					
					calibrate();
					break;	
					
				default:
					USART2_write(0x00);
					USART2_send_nack();
					break;
				
				
				
			
			
			
			
			
			
			}
			
			
		}
		
		
	}
}
