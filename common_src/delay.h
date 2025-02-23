/*
 * mt32-pi-control
 *
 * An mt32-pi control program for DOS PCs
 * and Amiga computers
 *
 * Copyright (C) 2021 Andreas Zdziarstek
 */

#ifndef __DELAY_H__
#define __DELAY_H__

#ifdef USE_CUSTOM_DELAY
void delay_ms(unsigned int delay);
#else
#include <time.h>

static void delay_ms(unsigned int delay) {
	clock_t start = clock();
	unsigned int delay_clk;
	if (delay >= 1000) {
		/* First wait for the delay in seconds to avoid potential overflow when compiuting delay_clk below */
		delay_clk = delay / 1000 * CLOCKS_PER_SEC;
		while(clock() - start < delay_clk);
		delay %= 1000;
		start += delay_clk;
	}

	if (delay > 0) {
		delay_clk = delay * CLOCKS_PER_SEC / 1000u;
		while(clock() - start < delay_clk);
	}
}
#endif

#endif /* __DELAY_H__ */
