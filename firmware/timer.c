#include <stdint.h>
#include <stm32f10x.h>
#include <stdio.h>
#include "timer.h"
#include "gpio.h"
#include "system.h"
#include "uart.h"

#define STEPPER1_PIN 1
#define STEPPER2_PIN 5

#define SERVO1_PIN 8
#define SERVO2_PIN 9

#define PULSE_US 1U
#define PW_TICKS PULSE_US // 1 tick = 1 µs with current PSC = 71 for TIM3

volatile static uint32_t stepper1_goal;
volatile static uint32_t stepper1_count = 0;

volatile static uint32_t stepper2_goal;
volatile static uint32_t stepper2_count = 0;

volatile uint32_t dbg_psc = 0;

uint32_t get_stepper1_count(void) {
	return stepper1_count;
}

uint32_t get_stepper2_count(void) {
	return stepper2_count;
}

uint32_t get_stepper1_goal(void) {
	return stepper1_goal;
}

uint32_t get_stepper2_goal(void) {
	return stepper2_goal;
}

void set_stepper1_goal(uint32_t steps) {
	stepper1_goal = steps;
}

void set_stepper2_goal(uint32_t steps) {
	stepper2_goal = steps;
}

void stepper1_start_counting(void) {
	stepper1_count = 0;
	TIM2->CR1 |= 1;
}

void stepper2_start_counting(void) {
	stepper2_count = 0;
	TIM4->CR1 |= 1;
}

void stepper1_stop_counting(void) {
	TIM2->CR1 &= ~TIM_CR1_CEN;
}

void stepper2_stop_counting(void) {
	TIM4->CR1 &= ~TIM_CR1_CEN;
}

void TIM2_init_step_freq(uint16_t psc, uint16_t arr) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // Enable TIM2
	
	TIM2->PSC = psc; // fstep = fclk / ((PSC + 1)(ARR + 1))
	TIM2->ARR = arr;
	TIM2->EGR = 1; // Forces loading of PSC, ARR
	TIM2->DIER |= 1; // Bit 0, UIE, allows TIM2 to raise IRQ
	//TIM2->CR1 |= 1; // Start counting
	NVIC_EnableIRQ(TIM2_IRQn); // Set bit 28 of ISER0 register in NVIC
	
}

void TIM3_init_pulse_width() {
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // Enable TIM3
	
	TIM3->PSC = 71; // fstep = fclk / ((PSC + 1)(ARR + 1))
	TIM3->ARR = 0xFFFF; // Free run forever
	TIM3->EGR = 1; // Forces loading of PSC, ARR
	//TIM3->DIER = TIM_DIER_CC1IE | TIM_DIER_CC2IE; // Bit 0, UIE, allows TIM3 to raise IRQ; CHANGE -> only enable CC1 and CC2 compare events
	TIM3->DIER = 0;
	NVIC_EnableIRQ(TIM3_IRQn); // Set bit 29 of ISER0 register in NVIC
	TIM3->CR1 |= TIM_CR1_CEN; //Start counting
	
}

void TIM4_init_step_freq(uint16_t psc, uint16_t arr) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // Enable TIM4
	
	TIM4->PSC = psc; // fstep = fclk / ((PSC + 1)(ARR + 1))
	TIM4->ARR = arr;
	TIM4->EGR = 1; // Forces loading of PSC, ARR
	TIM4->DIER |= 1; // Bit 0, UIE, allows TIM4 to raise IRQ
	//TIM4->CR1 |= 1; // Start counting
	NVIC_EnableIRQ(TIM4_IRQn); // Set bit 30 of ISER0 register in NVIC
	
}

/*
void TIM5_init_pulse_width(uint16_t psc, uint16_t arr) {
	RCC->APB1ENR |= RCC_APB1ENR_TIM5EN; // Enable TIM5
	
	TIM5->PSC = psc; // fstep = fclk / ((PSC + 1)(ARR + 1))
	TIM5->ARR = arr;
	TIM5->EGR = 1; // Forces loading of PSC, ARR
	TIM5->DIER |= 1; // Bit 0, UIE, allows TIM5 to raise IRQ
	NVIC_EnableIRQ(TIM5_IRQn); // Set bit 50 of ISER0 register in NVIC
	
}
*/

void TIM_set_psc(TIM_TypeDef* TIMx, uint16_t psc) {
	TIMx->PSC = psc;
}

void TIM_set_arr(TIM_TypeDef* TIMx, uint16_t arr) {
	TIMx->ARR = arr;
}


void TIM1_init_servo(void) {
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN | RCC_APB2ENR_IOPAEN; // Enable TIM1 clocks
	TIM1->PSC = 71;
	TIM1->ARR = 20000; // 20 ms
	
	// PA8 = CH1, PA9 = CH2
	GPIOA->CRH &= ~((0xF << ((SERVO1_PIN - 8) * 4)) | (0xF << ((SERVO2_PIN - 8) * 4))); // Clear CNF/MODE for PA8, PA9
	GPIOA->CRH |=  ((0xB << ((SERVO1_PIN - 8) * 4)) | (0xB << ((SERVO2_PIN - 8) * 4))); // Output 50MHz, AF push-pull

	
	// Duty cycle varies between 1.0 ms (-90) and 2.0 ms (90)
	TIM1->CCMR1 &= ~(TIM_CCMR1_OC1M | TIM_CCMR1_CC1S);
	TIM1->CCMR1 |= (0b110 << 4); // Set Channel 1 to PWM Mode 1, bits [6:4]
	TIM1->CCMR1 |= TIM_CCMR1_OC1PE; // Set corresponding preload register for Channel 1
	
	TIM1->CCMR1 &= ~(TIM_CCMR1_OC2M | TIM_CCMR1_CC2S);
	TIM1->CCMR1 |= (0b110 << 12); // Set Channel 2 to PWM Mode 1, bits [14:12]
	TIM1->CCMR1 |= TIM_CCMR1_OC2PE; // Set corresponding preload register for Channel 2
	
	TIM1->CCER |= TIM_CCER_CC1E; // Enable CH1 output
	TIM1->CCER |= TIM_CCER_CC2E; // Enable CH2 output
	
	TIM1->CCR1 = 1500; // 0
	TIM1->CCR2 = 1500; // 0
	
	TIM1->CR1 |= TIM_CR1_ARPE; // Auto-reload preload enable
	TIM1->EGR |= TIM_EGR_UG; // Generate update event to load registers
	TIM1->BDTR |= TIM_BDTR_MOE; // Enable main output for advanced timers
	TIM1->CR1 |= TIM_CR1_CEN; // Enable counter
	
}

void TIM1_set_CCRx(uint8_t channel, uint32_t value) {
	if (channel == 1) TIM1->CCR1 = value;
	else if (channel == 2) TIM1->CCR2 = value;
	
}


void TIM2_IRQHandler(void) {
	uint32_t status = TIM2->SR;

	// Check if UIF was raised
	if (status & TIM_SR_UIF) {
		TIM2->SR &= ~TIM_SR_UIF; // Reset UIF
		if (stepper1_count < stepper1_goal) {
			stepper1_count++;
			//GPIOA->BSRR |= (1 << 1); // Set high
			GPIO_set(GPIOA, STEPPER1_PIN);
			//TIM3->CR1 |= 1; // Start counting
			TIM3->SR &= ~TIM_SR_CC1IF; // Clear Capture/Compare Register 1 Interrupt Flag
			TIM3->CCR1 = TIM3->CNT + PW_TICKS; // Schedule CCR1	
			TIM3->DIER |= TIM_DIER_CC1IE; // DMA/Interrupt Enable Register Capture/Compare Interrupt Flag
		}
		else {
			//TIM2->CR1 &= ~TIM_CR1_CEN; // Stop counting
			stepper1_stop_counting();
			if (get_system_state() != CALIBRATING) {
				USART2_send_ack();
				set_system_state(IDLE);
			}
		}
	}
}

void TIM3_IRQHandler(void) {
	uint32_t status = TIM3->SR;
	
	if (status & TIM_SR_CC1IF) {
		TIM3->SR &= ~TIM_SR_CC1IF;
		GPIO_clear(GPIOA, STEPPER1_PIN);
		TIM3->DIER &= ~TIM_DIER_CC1IE;
	}
	if (status & TIM_SR_CC2IF) {
		TIM3->SR &= ~TIM_SR_CC2IF;
		GPIO_clear(GPIOA, STEPPER2_PIN);
		TIM3->DIER &= ~TIM_DIER_CC2IE;
	}
	
	/*
	// Check if UIF was raised
	if (status & 1) {
		TIM3->SR &= ~1; // Reset UIF
		//GPIOA->BSRR |= (1 << (1 + 16)); // Set low
		GPIO_clear(GPIOA, STEPPER1_PIN);
		TIM3->CR1 &= ~1; // Stop counting
	}
	*/
	
	
}

void TIM4_IRQHandler(void) {
	uint32_t status = TIM4->SR;

	// Check if UIF was raised
	if (status & TIM_SR_UIF) {
		TIM4->SR &= ~TIM_SR_UIF; // Reset UIF
		if (stepper2_count < stepper2_goal) {
			stepper2_count++;
			//GPIOA->BSRR |= (1 << 1); // Set high
			GPIO_set(GPIOA, STEPPER2_PIN);
			//TIM5->CR1 |= 1; // Start counting
			TIM3->SR &= ~TIM_SR_CC2IF;
			TIM3->CCR2 = TIM3->CNT + PW_TICKS;
			TIM3->DIER |= TIM_DIER_CC2IE;
		}
		else {
			//TIM4->CR1 &= ~TIM_CR1_CEN; // Stop counting 
			stepper2_stop_counting();
			if (get_system_state() != CALIBRATING) {
				USART2_send_ack();
				set_system_state(IDLE);
			}
		}
	}
}

/*
void TIM5_IRQHandler(void) {
	uint32_t status = TIM5->SR;
	
	// Check if UIF was raised
	if (status & 1) {
		TIM5->SR &= ~1; // Reset UIF
		//GPIOA->BSRR |= (1 << (1 + 16)); // Set low
		GPIO_clear(GPIOA, STEPPER2_PIN);
		TIM5->CR1 &= ~1; // Stop counting
	}
}
*/


	
	
	
	
	
	
	
	
	
	
	
	
	
	
	