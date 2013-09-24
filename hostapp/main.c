#include "daemon-poll.h"
#include "usb.h"
#include "config.h"
#include "nai.h"
#include "types.h"

#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

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

static void sighandler(int signal) {
	switch (signal) {
		case SIGHUP:
			printf("main: got SIGHUP.\n");
			if (!config_init(configfilename))
				nai_flags.sigexit = 1;
			break;
		case SIGINT:
			if (nai_flags.sigexit) {
				printf("main: got SIGINT again, exiting.\n");
				exit(0);
			}
			printf("main: got SIGINT.\n");
			nai_flags.sigexit = 1;
			break;
		case SIGTERM:
			if (nai_flags.sigexit) {
				printf("main: got SIGTERM again, exiting.\n");
				exit(0);
			}
			printf("main: got SIGTERM.\n");
			nai_flags.sigexit = 1;
			break;
	}
}

int main(int argc, char **argv) {
	signal(SIGHUP, sighandler);
	signal(SIGINT, sighandler);
	signal(SIGTERM, sighandler);
	signal(SIGPIPE, SIG_IGN);

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
	config_deinit();

	return 0;
}
