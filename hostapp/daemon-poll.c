#include "daemon-poll.h"

#include <sys/poll.h>
#include <stdlib.h>
#include <stdio.h>

static struct pollfd *pfd = NULL; // This stores the watched file descriptors as pollfd structures.
static int pfdcount = 0; // How much pollfd structures we store.
static int polltimeout = 0;

// Reallocates the file descriptor storage array.
static flag_t daemon_poll_pfdrealloc(int count) {
	struct pollfd *newpfd = (struct pollfd *)realloc(pfd, sizeof(struct pollfd) * count);

	if (count > 0 && !newpfd)
		return 0;

	pfd = newpfd;
	return 1;
}

// Returns the array index of the given file descriptor.
static int daemon_poll_getpfdindex(int fd) {
	int i = 0;

	for (i = 0; i < pfdcount; i++) {
		if (pfd[i].fd == fd)
			return i;
	}
	return -1;
}

// This function adds the given file descriptor to the watched file descriptor list.
// Events represents the events we need to watch on this fd (see "man poll").
void daemon_poll_addfd(int fd, short events) {
	int i = daemon_poll_getpfdindex(fd);

	if (i >= 0) { // If we already watch the fd, we update the watched events
		pfd[i].events |= events;
		pfd[i].revents = 0;
		printf("daemon-poll: fd %d already added, updated events to %d.", fd, events);
		return;
	}

	if (!daemon_poll_pfdrealloc(pfdcount+1)) {
		fprintf(stderr, "daemon-poll: can't realloc pfd storage.\n");
		return;
	}

	pfd[pfdcount].fd = fd;
	pfd[pfdcount].events = events;
	pfd[pfdcount].revents = 0;
	pfdcount++;
	printf("daemon-poll: added fd %d (events: %d) to watched set.\n", events, fd);
}

// Wrapper for daemon_poll_addfd().
void daemon_poll_addfd_read(int fd) {
	daemon_poll_addfd(fd, POLLIN);
}

// Wrapper for daemon_poll_addfd().
void daemon_poll_addfd_write(int fd) {
	daemon_poll_addfd(fd, POLLOUT);
}

// Wrapper for daemon_poll_addfd().
void daemon_poll_addfd_readwrite(int fd) {
	daemon_poll_addfd(fd, POLLIN | POLLOUT);
}

// This function removes the given file descriptor from the watched file descriptor list.
void daemon_poll_removefd(int fd) {
	int i = 0, found = 0;

	if (pfdcount <= 0)
		return;

	// Searching for the fd
	for (i = 0; i < pfdcount-1; i++) {
		if (pfd[i].fd == fd)
			found = 1;
		// If the item was found, we shift the other ones to the left
		if (found)
			pfd[i] = pfd[i+1];
	}

	if (pfd[pfdcount-1].fd == fd)
		found = 1;

	if (found) {
		pfdcount--;
		daemon_poll_pfdrealloc(pfdcount);
		printf("daemon-poll: removed fd %d from watched set.\n", fd);
	} else
		fprintf(stderr, "daemon-poll: can't remove fd %d from watched set.\n", fd);
}

// This function sets the maximum timeout the poll() call will wait if no events happen on
// the watched file descriptors. The timeout will be reseted to a default value after the
// poll() call.
void daemon_poll_setmaxtimeout(int timeout) {
	if (timeout < polltimeout) {
		printf("daemon-poll: setting max. poll timeout to %d ms.\n", timeout);
		polltimeout = timeout;
	}
}

// Queries the result of the poll() call for the given file descriptor.
flag_t daemon_poll_isfdreadable(int fd) {
	int i = daemon_poll_getpfdindex(fd);

	if (i < 0)
		return 0;

	return ((pfd[i].revents & POLLIN) > 0);
}

// Queries the result of the poll() call for the given file descriptor.
flag_t daemon_poll_isfdwritable(int fd) {
	int i = daemon_poll_getpfdindex(fd);

	if (i < 0)
		return 0;

	return ((pfd[i].revents & POLLOUT) > 0);
}

flag_t daemon_poll_process(void) {
//	printf("daemon-poll: calling poll() on %d fds, timeout: %d ms.\n", pfdcount, polltimeout);
	if (poll(pfd, pfdcount, polltimeout) < 0) {
		fprintf(stderr, "daemon-poll: poll() error.\n");
		return 0;
	}
//	printf("daemon-poll: poll() exited, continuing execution.\n");

	// Setting a default poll timeout, this can be overridden once at a time by daemon_set_max_polltimeout()
	polltimeout = 1000;
	return 1;
}

void daemon_poll_init(void) {
	if (pfd) {
		free(pfd);
		pfd = NULL;
	}
	pfdcount = 0;
}

void daemon_poll_deinit(void) {
	if (pfd) {
		free(pfd);
		pfd = NULL;
	}
	pfdcount = 0;
}
