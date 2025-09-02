#include <stdint.h>
#include <stm32f10x.h>
#include "calibrate.h"
#include "system.h"
#include "stepper.h"
#include "uart.h"


static volatile CalibrationState calibration_state;
static volatile uint32_t y_distance;
static volatile uint32_t z_distance;

#define UP 0
#define DOWN 1
#define RIGHT 0
#define LEFT 1

#define UNTIL_SW 0x7FFFFFFF

CalibrationState get_calibration_state(void) {
	return calibration_state;
}

void set_y_distance(uint32_t steps) {
	y_distance = steps;
}

void set_z_distance(uint32_t steps) {
	z_distance = steps;
}

uint32_t get_y_distance() {
	
	return y_distance;
}

uint32_t get_z_distance() {
	
	return z_distance;
}


void set_calibration_state(CalibrationState state) {
	calibration_state = state;
}

void calibrate(void) {
	set_y_distance(0);
	set_z_distance(0);
	set_system_state(CALIBRATING);
	calibration_state = ZEROING_Z;
	while(1) {
		switch(calibration_state) {
			
			case ZEROING_Z:
				set_stepper2_dir(UP);
				set_stepper2_goal(UNTIL_SW);
				stepper2_start_counting();
				set_calibration_state(WAIT_ZEROING_Z);
				break;
			
			case WAIT_ZEROING_Z:
				break;
			
			case TOLERANCE1_Z:
				set_stepper2_dir(DOWN);
				set_stepper2_goal(END_TOLERANCE);
				stepper2_start_counting();
				set_calibration_state(WAIT_TOLERANCE1_Z);
				break;
			
			case WAIT_TOLERANCE1_Z:
				if (get_stepper2_count() >= get_stepper2_goal()) {
					set_calibration_state(ZEROING_Y);
				}
				break;
			
			case ZEROING_Y:
				set_stepper1_dir(RIGHT);
				set_stepper1_goal(UNTIL_SW);
				stepper1_start_counting();
				set_calibration_state(WAIT_ZEROING_Y);
				break;
			
			case WAIT_ZEROING_Y:
				break;
			
			case TOLERANCE1_Y:
				set_stepper1_dir(LEFT);
				set_stepper1_goal(END_TOLERANCE);
				stepper1_start_counting();
				set_calibration_state(WAIT_TOLERANCE1_Y);
				break;
			
			case WAIT_TOLERANCE1_Y:
				if (get_stepper1_count() >= get_stepper1_goal()) {
					set_calibration_state(MEASURE_Z);
				}
				break;
			
			case MEASURE_Z:
				set_stepper2_dir(DOWN);
				set_stepper2_goal(UNTIL_SW);
				stepper2_start_counting();
				set_calibration_state(WAIT_MEASURE_Z);
				break;
			
			case WAIT_MEASURE_Z:
				break;
			
			case TOLERANCE2_Z:
				set_stepper2_dir(UP);
				set_stepper2_goal(END_TOLERANCE);
				stepper2_start_counting();
				set_calibration_state(WAIT_TOLERANCE2_Z);
				break;
			
			case WAIT_TOLERANCE2_Z:
				if (get_stepper2_count() >= get_stepper2_goal()) {
					set_calibration_state(RETURN_Z);
				}
				break;
			
			case RETURN_Z:
				set_stepper2_dir(UP);
				set_stepper2_goal(z_distance);
				stepper2_start_counting();
				set_calibration_state(WAIT_RETURN_Z);
				break;
			
			case WAIT_RETURN_Z:
				if (get_stepper2_count() >= get_stepper2_goal()) {
					set_calibration_state(MEASURE_Y);
				}
				break;
			
			case MEASURE_Y:
				set_stepper1_dir(LEFT);
				set_stepper1_goal(UNTIL_SW);
				stepper1_start_counting();
				set_calibration_state(WAIT_MEASURE_Y);
				break;
			
			case WAIT_MEASURE_Y:
				break;
			
			case TOLERANCE2_Y:
				set_stepper1_dir(RIGHT);
				set_stepper1_goal(END_TOLERANCE);
				stepper1_start_counting();
				set_calibration_state(WAIT_TOLERANCE2_Y);
				break;
			
			case WAIT_TOLERANCE2_Y:
				if (get_stepper1_count() >= get_stepper1_goal()) {
					set_calibration_state(RETURN_Y);
				}
				break;
			
			case RETURN_Y:
				set_stepper1_dir(RIGHT);
				set_stepper1_goal(y_distance);
				stepper1_start_counting();
				set_calibration_state(WAIT_RETURN_Y);
				break;
			
			case WAIT_RETURN_Y:
				if (get_stepper1_count() >= get_stepper1_goal()) {
					set_calibration_state(SEND_PACKET);
				}
				break;
			
			case SEND_PACKET:
				// Transmit data here
				send_cal_values();
				set_system_state(IDLE);
				return;
			
		}
		
		
		
	}
	
	
	
	
	
	
	
	
	
	
}

void send_cal_values(void) {
	uint32_t y = get_y_distance();
	uint32_t z = get_z_distance();

	USART2_write(0xAA);  
	USART2_write(0x02); 
	USART2_write(8);     

	// Send y distance
	USART2_write((uint8_t)(y)); // LSB
	USART2_write((uint8_t)(y >> 8));
	USART2_write((uint8_t)(y >> 16));
	USART2_write((uint8_t)(y >> 24)); // MSB

	// Send z distance
	USART2_write((uint8_t)(z));
	USART2_write((uint8_t)(z >> 8));
	USART2_write((uint8_t)(z >> 16));
	USART2_write((uint8_t)(z >> 24));

	USART2_write(0x55); 
}

/*

set state as calibrating
stepper2 move up till exti0
stepper2 move down 10 steps
stepper1 move right till exti1
stepper1 move left 10 steps
stepper2 move down till exti2
stepper2 move up 10 steps
record z_distance
stepper2 move up that many steps
stepper1 move left till exti3
stepper1 move right 10 steps
record y_distance
stepper1 move right that many steps
send packet






















*/