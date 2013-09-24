#include "daemon.h"
#include "daemon-poll.h"

extern nai_flags_t nai_flags;

void daemon_daemonize(void) {
	// TODO
}

flag_t daemon_process(void) {
	// TODO
	if (!daemon_poll_process())
		return 0;
	return 1;
}

void daemon_init(void) {
	daemon_poll_init();
}

void daemon_deinit(void) {
	// TODO
}
