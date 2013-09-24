#include "daemon-poll.h"
#include "usb.h"
#include "config.h"
#include "nai.h"
#include "types.h"

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <libusb-1.0/libusb.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#define VERSION "0.1"

nai_flags_t nai_flags = {0};
static char *configfilename = "nai.conf";

static void printversion(void) {
	printf("nalarminterface v" VERSION " built: " __TIME__ " " __DATE__ "\n");
}

static void printusage(void) {
	printversion();
	printf("usage: -h         - this help\n");
	printf("       -v         - print version\n");
	printf("       -c [file]  - use config file \"file\"\n");
}

static void processcommandline(int argc, char **argv) {
	int r;

	while ((r = getopt(argc, argv, "hvc:")) != -1) {
		switch (r) {
			case 'v':
				printversion();
				break;
			case 'h':
				printusage();
				exit(0);
				break;
			case 'c':
				configfilename = optarg;
				break;
			default:
				if (isprint(optopt))
					fprintf(stderr, "unknown option '-%c'.\n", optopt);
				else
					fprintf(stderr, "unknown option character '-%x'.\n", optopt);
				exit(1);
		}
	}

	if (optind < argc) {
		printusage();
		exit(1);
	}
}

int main(int argc, char **argv) {
	processcommandline(argc, argv);

	if (!config_init(configfilename))
		return 1;

	if (!usb_init())
		return 1;

	daemon_poll_init();

	while (!nai_flags.sigexit) {
		if (!usb_process())
			break;
		if (!daemon_poll_process())
			break;
		if (!nai_process())
			break;
	}

	usb_deinit();
	daemon_poll_deinit();

	return 0;
}
