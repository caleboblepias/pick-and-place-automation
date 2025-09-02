#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include "stm32f10x.h"

uint32_t get_stepper1_count(void);
	
uint32_t get_stepper2_count(void);

uint32_t get_stepper1_goal(void);

uint32_t get_stepper2_goal(void);

void set_stepper1_goal(uint32_t steps);

void set_stepper2_goal(uint32_t steps);

void stepper1_start_counting(void);

void stepper2_start_counting(void);

void stepper1_stop_counting(void);
	
void stepper2_stop_counting(void);

void TIM2_init_step_freq(uint16_t psc, uint16_t arr);

void TIM3_init_pulse_width();

void TIM4_init_step_freq(uint16_t psc, uint16_t arr);

//void TIM5_init_pulse_width(uint16_t psc, uint16_t arr);

void TIM_set_psc(TIM_TypeDef* TIMx, uint16_t psc);

void TIM_set_arr(TIM_TypeDef* TIMx, uint16_t arr);

void TIM1_init_servo(void);

void TIM1_set_CCRx(uint8_t channel, uint32_t value);

#endif // TIMER_H