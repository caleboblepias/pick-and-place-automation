#ifndef CALIBRATE_H
#define CALIBRATE_H

#include <stdint.h>
#include "stm32f10x.h"

#define END_TOLERANCE 800

typedef enum {

	ZEROING_Z, 			WAIT_ZEROING_Z,
	TOLERANCE1_Z, 	WAIT_TOLERANCE1_Z,
	ZEROING_Y, 			WAIT_ZEROING_Y,
	TOLERANCE1_Y, 	WAIT_TOLERANCE1_Y,
	MEASURE_Z, 			WAIT_MEASURE_Z,
	TOLERANCE2_Z, 	WAIT_TOLERANCE2_Z,
	RETURN_Z, 			WAIT_RETURN_Z,
	MEASURE_Y, 			WAIT_MEASURE_Y,
	TOLERANCE2_Y, 	WAIT_TOLERANCE2_Y,
	RETURN_Y, 			WAIT_RETURN_Y,
	SEND_PACKET
	
} CalibrationState;

void set_y_distance(uint32_t steps);

void set_z_distance(uint32_t steps);

void set_calibration_state(CalibrationState state);

void calibrate(void);

void send_cal_values(void);

uint32_t get_y_distance(void);

uint32_t get_z_distance(void);

CalibrationState get_calibration_state(void);

#endif // CALIBRATE_H