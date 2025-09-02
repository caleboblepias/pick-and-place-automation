#include <stdint.h>
#include <stdio.h>
#include <stm32f10x.h>
#include "servo.h"
#include "gpio.h"
#include "timer.h"

#define SERVO1_PIN 4
#define SERVO2_PIN 5

void servo1_init(void) {
	//GPIO_init_output(GPIOA, SERVO1_PIN);
	TIM1_init_servo();
}

void servo2_init(void) {
	//GPIO_init_output(GPIOA, SERVO2_PIN);
	TIM1_init_servo();
}

void set_servo1_angle(uint16_t angle) {
	uint32_t converted_angle = 500 + ((uint32_t) angle * 2000) / 270;
	TIM1_set_CCRx(1, converted_angle);
}

void set_servo2_angle(uint16_t angle) {
	uint32_t converted_angle = 500 + ((uint32_t) angle * 2000) / 270;
	TIM1_set_CCRx(2, converted_angle);
}