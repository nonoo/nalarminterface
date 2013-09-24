#ifndef DAEMON_H_
#define DAEMON_H_

#include "types.h"

void daemon_daemonize(void);
flag_t daemon_process(void);
void daemon_init(void);
void daemon_deinit(void);

#endif
