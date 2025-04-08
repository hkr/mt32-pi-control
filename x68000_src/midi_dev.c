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

#define MIDIORI_CMD_COUNT 16
static unsigned char midiori_command[MIDIORI_CMD_COUNT];
static unsigned char midiori_command_has_data2[MIDIORI_CMD_COUNT];
static unsigned char midiori_data[MIDIORI_CMD_COUNT];
static unsigned char midiori_data2[MIDIORI_CMD_COUNT];
static unsigned char midiori_count;

static void send_midiori_command(int i)
{
	unsigned char buf[] = {0xF0, 0x7D, midiori_command[i], midiori_data[i], midiori_command_has_data2[i] ? midiori_data2[i] : 0xF7, 0xF7};
	mididev_send_bytes(buf, midiori_command_has_data2[i] ? sizeof(buf) : sizeof(buf)-1);
	//printf("send_midiori_command %x %x %x %x %x\n", (int)buf[0], (int)buf[1], (int)buf[2], (int)buf[3], (int)buf[4]);
}

int mididev_init(void) {
	struct sigaction sa;
	int version, i;
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
	
	for (i = 0; i < midiori_count; ++i)
	{
		send_midiori_command(i);
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
			midi_write(buf[i]);
	}

	return 0;
}

void mididev_print_usage(void) {
	printf("-z vol=[0-255]: Volume of the selected synth.\n");
	printf("-z sel=[0 or 1]: Select active synth (0=MT32-Pi 1=Wavetable).\n");
	printf("-z fm=[0-255/0-255]: Volume of the X68000's FM/PCM audio.\n");
	printf("-z pi=[0-255/0-255]: Volume of the MT32-Pi.\n");
	printf("-z wt=[0-255/0-255]: Volume of the wavetable board.\n");
	printf("-z lr=[0 or 1]: Swap stereo channels of wavetable board\n");
	printf("-z int=[0-255/0-255]: Volume of external output.\n");
	printf("-z ext=[0-255/0-255]: Volume of internal output.\n");
}

static char *dev_optstr = "z:";

void mididev_add_optstr(char *optstr) {
	int main_len = strlen(optstr);
	strcpy(optstr+main_len, dev_optstr);
}

static int parse_volume(const char* str) {
	char* end;
	long int arg0, arg1;
	arg0 = strtol(str, &end, 10);
	midiori_data[midiori_count] = arg0 < 0 ? 0 : arg0 > 255 ? 255 : arg0;
	if (*end == '/') {
		arg1 = strtol(end + 1, 0, 10);
		midiori_data2[midiori_count] = arg0 < 0 ? 0 : arg1 > 255 ? 255 : arg1;
	} else {
		midiori_data2[midiori_count] = midiori_data[midiori_count];
	}
	midiori_command_has_data2[midiori_count] = 1;
	++midiori_count;
	return 0;
}

int mididev_parse_arg(int c, const char *optarg) {
	long int arg;
	if(c=='z') {
		if (midiori_count >= MIDIORI_CMD_COUNT)
			return -1;

		midiori_command_has_data2[midiori_count] = 0;
		if (!strncmp(optarg, "vol=", 4)) {
			midiori_command[midiori_count] = 0x37;
			arg = strtol(optarg + 4, 0, 10);
			midiori_data[midiori_count] = arg < 0 ? 0 : arg > 255 ? 255 : arg;
			++midiori_count;
			return 0;
		}
		if (!strncmp(optarg, "sel=", 4)) {
			midiori_command[midiori_count] = 0x36;
			midiori_data[midiori_count] = strtol(optarg + 4, 0, 10) ? 1 : 0;
			++midiori_count;
			return 0;
		}
		if (!strncmp(optarg, "lr=", 3)) {
			midiori_command[midiori_count] = 0x35;
			midiori_data[midiori_count] = strtol(optarg + 3, 0, 10) ? 1 : 0;
			++midiori_count;
			return 0;
		}
		if (!strncmp(optarg, "fm=", 3)) {
			midiori_command[midiori_count] = 0x30;
			return parse_volume(optarg + 3);
		}
		if (!strncmp(optarg, "pi=", 3)) {
			midiori_command[midiori_count] = 0x31;
			return parse_volume(optarg + 3);
		}
		if (!strncmp(optarg, "wt=", 3)) {
			midiori_command[midiori_count] = 0x32;
			return parse_volume(optarg + 3);
		}
		if (!strncmp(optarg, "ext=", 4)) {
			midiori_command[midiori_count] = 0x33;
			return parse_volume(optarg + 4);
		}
		if (!strncmp(optarg, "int=", 4)) {
			midiori_command[midiori_count] = 0x34;
			return parse_volume(optarg + 4);
		}
		return -1;
	}
	return -1;
}
