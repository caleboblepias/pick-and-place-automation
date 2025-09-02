#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>
#include "stm32f10x.h"

typedef enum {
	IDLE,
	CALIBRATING,
	RUNNING,
	ERR
	
} SystemState;

SystemState get_system_state(void);

void set_system_state(SystemState state);





#endif // SYSTEM_H