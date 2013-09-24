#include "daemon.h"
#include "daemon-poll.h"

extern nai_flags_t nai_flags;

void daemon_daemonize(void) {
	// TODO
}

void daemon_process(void) {
	// TODO
	daemon_poll_process();
}

void daemon_init(void) {
	daemon_poll_init();
}

void daemon_deinit(void) {
	// TODO
}
