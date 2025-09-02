#include <stdint.h>
#include <stm32f10x.h>
#include "system.h"

static volatile SystemState system_state = IDLE;

void set_system_state(SystemState mode) {
	system_state = mode;
}

SystemState get_system_state(void) {
	return system_state;
}