#include <stdint.h>
#include <stm32f10x.h>
#include "stepper.h"
#include "timer.h"
#include "gpio.h"
#include "system.h"

#define DEFAULT_STEP_PSC 71
#define DEFAULT_STEP_ARR 9

void pulse_width_init(void) {
	TIM3_init_pulse_width();
} 

void stepper1_init(void) {
	GPIO_init_output(GPIOA, 1);
	GPIO_init_output(GPIOA, 0);
	GPIO_set(GPIOA, 0);
	TIM2_init_step_freq(DEFAULT_STEP_PSC, DEFAULT_STEP_ARR);
}

void stepper2_init(void) {
	GPIO_init_output(GPIOA, 5);
	GPIO_init_output(GPIOA, 4);
	GPIO_set(GPIOA, 4);
	TIM4_init_step_freq(DEFAULT_STEP_PSC, DEFAULT_STEP_ARR);
}

void set_stepper1_step_freq(uint16_t freq) {
	uint16_t arr = (72000000 / ((freq) * (TIM2->PSC + 1))) - 1;
	TIM_set_arr(TIM2, arr);
}

/*
void set_stepper1_pulse_width(uint16_t arr) {
	TIM_set_arr(TIM3, arr);
}
*/

void set_stepper2_step_freq(uint16_t freq) {
	uint16_t arr = (72000000 / ((freq) * (TIM4->PSC + 1))) - 1;
	TIM_set_arr(TIM4, arr);
}

/*
void set_stepper2_pulse_width(uint16_t arr) {
	TIM_set_arr(TIM5, arr);
}
*/

void drive_stepper1(uint32_t steps) {
	set_stepper1_goal(steps);
	stepper1_start_counting();
}

void drive_stepper2(uint32_t steps) {
	set_stepper2_goal(steps);
	stepper2_start_counting();
}

void set_stepper1_dir(uint8_t dir) {
	if (dir == 0) GPIO_clear(GPIOA, 0);
	else if (dir == 1) GPIO_set(GPIOA, 0);
	
}

void set_stepper2_dir(uint8_t dir) {
	if (dir == 0) GPIO_clear(GPIOA, 4);
	else if (dir == 1) GPIO_set(GPIOA, 4);
	
}

void stop_stepper1(void) {
	stepper1_stop_counting();
}

void stop_stepper2(void) {
	stepper2_stop_counting();
}
	
	
	
	
	
	
	