#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>

#include "midi_dev.h"
#include "getopt.h"
#include "delay.h"
#include "midi.h"

extern int mcdrv_version();
extern void mcdrv_release();
extern void mcdrv_send_midi(int channel_and_size, const void* data);

#define MIDI_DRIVER_NONE 0
#define MIDI_DRIVER_MCDRV 1

static int midi_driver_type = MIDI_DRIVER_NONE;

static sigjmp_buf  midi_init_env;
static void sigbus_handler(int sig) {
    siglongjmp(midi_init_env, 1);
}

int mididev_init(void) {
	struct sigaction sa;
	int version;
	version = mcdrv_version();
	if(version >= 0x500000)
		midi_driver_type = MIDI_DRIVER_MCDRV;
	else {
		sa.sa_handler = &sigbus_handler;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = 0;
		sigaction(SIGBUS, &sa, NULL);
		sigaction(SIGSEGV, &sa, NULL);
		sigaction(SIGILL, &sa, NULL);
		if (sigsetjmp(midi_init_env, 1) == 0) {
			midi_init(0 /*card 1, default address*/);
		}
		else {
			/* hardware not installed */
			return -1;
		}
	}
	
	return 0;
}

int mididev_deinit(void) {
	if (midi_driver_type == MIDI_DRIVER_MCDRV) {
		/* we are not loading the driver, so also don't unload */
		/*mcdrv_release();*/
	} else {
		midi_quit();
	}
	return 0;
}

int mididev_send_bytes(const unsigned char *buf, int len) {
	int channel_and_size, i;
	
	if (midi_driver_type == MIDI_DRIVER_MCDRV) {
		channel_and_size = (0x80 << 16) | len; /* channels 0x80-0x8f are for the first MIDI controller, we don't really care about channels otherwise. */
		mcdrv_send_midi(channel_and_size, buf);
	}
	else
	{
		for (i = 0; i < len; ++i)
			midi_write(buf[len]);
	}

	return 0;
}

void mididev_print_usage(void) {
}

void mididev_add_optstr(char *optstr) {
}

int mididev_parse_arg(int c, const char *optarg) {
	return -1;
}

