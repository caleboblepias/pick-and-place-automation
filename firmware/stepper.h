#ifndef STEPPER_H
#define STEPPER_H

#include <stdint.h>
#include "stm32f10x.h"
#include "timer.h"

void stepper1_init(void);

void stepper2_init(void);

void set_stepper1_step_freq(uint16_t freq);

//void set_stepper1_pulse_width(uint16_t arr);

void set_stepper2_step_freq(uint16_t freq);

//void set_stepper2_pulse_width(uint16_t arr);

void drive_stepper1(uint32_t steps);

void drive_stepper2(uint32_t steps);

void pulse_width_init(void);

void set_stepper1_dir(uint8_t dir);

void set_stepper2_dir(uint8_t dir);

void stop_stepper1(void);
	
void stop_stepper2(void);


#endif // STEPPER_H