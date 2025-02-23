#include "delay.h"

#include <stdlib.h>

void delay_ms(unsigned int delay) {
	if (delay >= 1000) {
		sleep(delay / 1000);
		delay %= 1000;
	}
	if (delay > 0)
		usleep(delay);
}

