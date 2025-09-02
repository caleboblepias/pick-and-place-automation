#include <stdint.h>
#include <stm32f10x.h>
#include "limit.h"
#include "system.h"
#include "timer.h"
#include "stepper.h"
#include "calibrate.h"
#include "uart.h"

void limit_init(uint8_t pin) {
	// External interrupts lie in GPIOB by choice
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE; // Clear out JTAG from PB3
	GPIOB->CRL &= ~(0xF << (4 * pin));  // Clear mode and config
	GPIOB->CRL |=  (0b1000 << (4 * pin)); // Input with pull-up/down
	GPIOB->ODR |= (1 << pin); // Pull-up (or clear for pull-down)
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	AFIO->EXTICR[pin / 4] |= (0b001 << (4 * (pin % 4)));
	EXTI->IMR  |= (1 << pin);  // Interrupt mask
	//EXTI->RTSR |= (1 << pin); // For rising edge
	//EXTI->RTSR &= ~(1 << pin);
	EXTI->FTSR |= (1 << pin); // For falling edge
	if (pin <= 4)
		NVIC_EnableIRQ(EXTI0_IRQn + pin); // EXTI0–4
	else if (pin <= 9)
		NVIC_EnableIRQ(EXTI9_5_IRQn); // EXTI5–9
	else
		NVIC_EnableIRQ(EXTI15_10_IRQn); // EXTI10–15



	
	
}

void EXTI0_IRQHandler(void) { // Y top stopper
	if (EXTI->PR & (1 << 0)) {
		EXTI->PR = (1 << 0); // Clear bit
		
		if ((GPIOB->IDR & (1 << 0)) == 0) {
			
			if (get_system_state() == RUNNING) {
				stop_stepper1();
			}
			else if (get_system_state() == CALIBRATING && get_calibration_state() == WAIT_ZEROING_Y) {
				stop_stepper1();
				set_calibration_state(TOLERANCE1_Y);
				
			}
			else if (get_system_state() == IDLE) {
				stop_stepper1();
				set_stepper1_dir(1);
				drive_stepper1(300);
			}
		}
		
	}
}

void EXTI1_IRQHandler(void) { // Y bottom stopper
	if (EXTI->PR & (1 << 1)) {
		
		EXTI->PR = (1 << 1); // Clear bit
		
		if ((GPIOB->IDR & (1 << 1)) == 0) {
			
			
		
			if (get_system_state() == RUNNING) {
				stop_stepper1();
			}
			else if (get_system_state() == CALIBRATING && get_calibration_state() == WAIT_MEASURE_Y) {
				stop_stepper1();
				set_y_distance(get_stepper1_count() - END_TOLERANCE);
				set_calibration_state(TOLERANCE2_Y);
				
			}
			else if (get_system_state() == IDLE) {
				stop_stepper1();
				set_stepper1_dir(0);
				drive_stepper1(300);
			}
		}
	}
}

void EXTI2_IRQHandler(void) { // Z top stopper
	if (EXTI->PR & (1 << 2)) {
		EXTI->PR = (1 << 2); // Clear bit
		
		
		if ((GPIOB->IDR & (1 << 2)) == 0) {
			if (get_system_state() == RUNNING) {
				stop_stepper2();
			}
			else if (get_system_state() == CALIBRATING && get_calibration_state() == WAIT_ZEROING_Z) {
				stop_stepper2();
				set_calibration_state(TOLERANCE1_Z);
				
			}
			else if (get_system_state() == IDLE) {
				stop_stepper2();
				set_stepper2_dir(1);
				drive_stepper2(300);
			}
		}
	}
}

void EXTI3_IRQHandler(void) { // Z bottom stopper
	if (EXTI->PR & (1 << 3)) {
		EXTI->PR = (1u << 3); // Clear bit
		
		if ((GPIOB->IDR & (1 << 3)) == 0) {
			if (get_system_state() == RUNNING) {
				stop_stepper2();
			}
			else if (get_system_state() == CALIBRATING && get_calibration_state() == WAIT_MEASURE_Z) {
				stop_stepper2();
				set_z_distance(get_stepper2_count() - END_TOLERANCE);
				set_calibration_state(TOLERANCE2_Z);
			}
			else if (get_system_state() == IDLE) {
				stop_stepper2();
				set_stepper2_dir(0);
				drive_stepper2(300);
			}
			
		}
		
	}
}
