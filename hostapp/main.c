#include "daemon.h"
#include "usb.h"
#include "types.h"

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <libusb-1.0/libusb.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#define VERSION "0.1"

static int daemonize = 1;

nai_flags_t nai_flags = {0};

static void printversion(void) {
	printf("nalarminterface v" VERSION "\n");
	printf("built: " __TIME__ " " __DATE__ "\n");
}

static void printusage(void) {
	printversion();
	printf("usage: -h         - this help\n");
	printf("       -v         - print version\n");
	printf("       -f         - don't fork to the background\n");
}

static void processcommandline(int argc, char **argv) {
	int r;

	while ((r = getopt(argc, argv, "hvf")) != -1) {
		switch (r) {
			case 'v':
				printversion();
				break;
			case 'f':
				daemonize = 0;
				break;
			case 'h':
				printusage();
				exit(0);
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

	if (!usb_init())
		return 1;

	if (daemonize)
		daemon_daemonize();

	while (!nai_flags.sigexit) {
		usb_process();
		daemon_process();
	}

	usb_deinit();
	daemon_deinit();

	return 0;
}
