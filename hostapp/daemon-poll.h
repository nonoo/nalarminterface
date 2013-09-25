#ifndef DAEMON_POLL_H_
#define DAEMON_POLL_H_

#include "types.h"

void daemon_poll_addfd(int fd, short events);
void daemon_poll_addfd_read(int fd);
void daemon_poll_addfd_write(int fd);
void daemon_poll_addfd_readwrite(int fd);

void daemon_poll_removefd(int fd);

void daemon_poll_setmaxtimeout(int timeout);

flag_t daemon_poll_isfdreadable(int fd);
flag_t daemon_poll_isfdwritable(int fd);

flag_t daemon_poll_process(void);
void daemon_poll_init(void);
void daemon_poll_deinit(void);

#endif
