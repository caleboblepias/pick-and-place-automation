#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>
#include "stm32f10x.h"
#include "timer.h"

void servo1_init(void);

void servo2_init(void);

void set_servo1_angle(uint16_t angle);

void set_servo2_angle(uint16_t angle);



#endif // SERVO_H