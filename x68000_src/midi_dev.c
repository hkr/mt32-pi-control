#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "midi_dev.h"
#include "getopt.h"
#include "delay.h"

/* requires MCDRV to be loaded */

extern int mcdrv_version();
extern void mcdrv_release();
extern void mcdrv_send_midi(int channel_and_size, const void* data);

int mididev_init(void) {
	int version;
	version = mcdrv_version();
	if(version == -1) {
		fprintf(stderr, "ERROR: MCDRV not detected. Please load the driver before running " PROGRAM_NAME ".\n");
		return -1;
	}
	if(version < 0x500000) {
		fprintf(stderr, "ERROR: Version of MCDRV 0.(%x) too old.\n", version / 0x10000);
		return -1;
	}
	
	return 0;
}

int mididev_deinit(void) {
	return 0;
}

int mididev_send_bytes(const unsigned char *buf, int len) {
	int channel_and_size;
	channel_and_size = (0x80 << 16) | len; /* channels 0x80-0x8f are for the first MIDI controller, we don't really care about channels otherwise. */
	mcdrv_send_midi(channel_and_size, buf);

	return 0;
}

void mididev_print_usage(void) {
}

void mididev_add_optstr(char *optstr) {
}

int mididev_parse_arg(int c, const char *optarg) {
	return -1;
}

