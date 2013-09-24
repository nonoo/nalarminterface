#include "usb.h"
#include "daemon-poll.h"
#include "nai.h"

#include "../avrboard/boardconfig.h"

#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define USBTIMEOUTINMS 1000

#define EP_INT_IN			(1 | LIBUSB_ENDPOINT_IN)
#define EP_INT_OUT			(2 | LIBUSB_ENDPOINT_OUT)
#define EP_INT_LENGTH		64

static struct {
	int claimed : 1;
	int initfinished : 1;
	int checkboardsent : 1;
	int connected : 1;
	int error : 1;
} usb_state = {0};

static libusb_context *usb_ctx = NULL;
static struct libusb_device_handle *usb_devh = NULL;
static struct libusb_transfer *usb_int_transfer = NULL;

static void LIBUSB_CALL usb_send_int_cb(struct libusb_transfer *transfer) {
	if (!transfer)
		return;

	if (transfer->status == LIBUSB_TRANSFER_CANCELLED) {
		// This happens when we cancel the transfer in usb_deinit(), so this case is not an error.
		printf("usb: transfer cancelled\n");
		return;
	}

	if (transfer->status != LIBUSB_TRANSFER_COMPLETED) {
		fprintf(stderr, "usb error: transfer error.\n");
		usb_state.error = 1;
		return;
	}

	printf("usb: sending int transfer completed.\n");
}

static void usb_send_int(uint8_t *data, int length) {
	struct libusb_transfer *transfer = NULL;
	uint8_t *intbuf = NULL;
	int i = 0;

	printf("usb: sending int (length: %d): ", length);
	for (i = 0; i < length; i++)
		printf("%.2x", data[i]);
	printf("\n");

	intbuf = (uint8_t *)malloc(sizeof(nai_usbpacket_t));
	if (!intbuf) {
		fprintf(stderr, "usb error: can't send int packet, out of memory.\n");
		usb_state.error = 1;
		return;
	}
	memcpy(intbuf, data, length);

	transfer = libusb_alloc_transfer(0);

	if (!transfer) {
		fprintf(stderr, "usb error: can't allocate transfer for int sending.\n");
		usb_state.error = 1;
		free(intbuf);
		return;
	}

	libusb_fill_interrupt_transfer(transfer, usb_devh, EP_INT_OUT, intbuf, sizeof(nai_usbpacket_t), usb_send_int_cb, NULL, 0);
	transfer->flags |= LIBUSB_TRANSFER_FREE_BUFFER | LIBUSB_TRANSFER_FREE_TRANSFER; // Free the buffer when the transfer is freed and free the transfer after the callback returns

	if (libusb_submit_transfer(transfer) < 0) {
		fprintf(stderr, "usb error: int transfer error.\n");
		usb_state.error = 1;
	} else
		printf("usb: int transfer submitted.\n");
}

void usb_send_naipacket(nai_usbpacket_t *usbpacket) {
	if (!usb_state.connected) {
		printf("usb: can't send nai packet, interface not connected.\n");
		return;
	}
	usb_send_int((uint8_t *)usbpacket, sizeof(nai_usbpacket_t));
}

static void usb_packet_received_cb(nai_usbpacket_t *usbpacket) {
	switch (usbpacket->type) {
		case NAI_USBPACKET_TYPE_CHECKBOARD | NAI_USBPACKET_TYPE_RESPONSE:
			printf("usb: received checkboard response.\n");
			if (!usb_state.connected) {
				printf("usb: interface is now connected.\n");
				usb_state.connected = 1;
				nai_usb_connected_cb();
			}
			break;
	}
}

static void LIBUSB_CALL usb_receive_int_cb(struct libusb_transfer *transfer) {
	printf("usb: int received, transfer status: ");
	switch (transfer->status) {
		case LIBUSB_TRANSFER_ERROR: printf("error\n"); break;
		case LIBUSB_TRANSFER_TIMED_OUT: printf("timed out\n"); break;
		case LIBUSB_TRANSFER_STALL: printf("stall\n"); break;
		case LIBUSB_TRANSFER_NO_DEVICE: printf("no device\n"); break;
		case LIBUSB_TRANSFER_OVERFLOW: printf("overflow\n"); break;
		case LIBUSB_TRANSFER_COMPLETED: printf("completed\n"); break;
		case LIBUSB_TRANSFER_CANCELLED: printf("cancelled\n"); break;
	}

	if (transfer->status == LIBUSB_TRANSFER_CANCELLED) {
		// This happens when we cancel the transfer in usb_deinit(), so this case is not an error.
		printf("usb: transfer cancelled\n");
		return;
	}

	if (transfer->status != LIBUSB_TRANSFER_COMPLETED) {
		usb_state.error = 1;
		return;
	}

	if (transfer->actual_length == sizeof(nai_usbpacket_t)) {
		// Handling connection-specific commands
		usb_packet_received_cb((nai_usbpacket_t *)transfer->buffer);
		// Passing other commands to the upper layer
		nai_usb_packet_received_cb((nai_usbpacket_t *)transfer->buffer);
	}

	// Reinitializing callback
	if (libusb_submit_transfer(usb_int_transfer) < 0) {
		fprintf(stderr, "usb: can't initialize rx int transfer.\n");
		usb_state.error = 1;
		libusb_free_transfer(usb_int_transfer);
		usb_int_transfer = NULL;
	}
}

static libusb_device_handle* usb_open(int vid, int pid) {
	libusb_device **usbdevlist = NULL;
	int i = 0, r = 0;
	struct libusb_device_descriptor desc = {0};
	int usbdevcount = 0;
	struct libusb_device_handle *probe_usbdevh = NULL;
	libusb_device *probe_usbdev = NULL;

	usbdevcount = libusb_get_device_list(usb_ctx, &usbdevlist);

	printf("usb: searching for device vid=%.4x pid=%.4x...\n", vid, pid);
	for (i = 0; i < usbdevcount; i++) {
		probe_usbdev = usbdevlist[i];
		r = libusb_get_device_descriptor(probe_usbdev, &desc);
		if (r >= 0 && desc.idVendor == vid && desc.idProduct == pid) {
			r = libusb_open(probe_usbdev, &probe_usbdevh);
			if (r == 0) {
				printf("usb: device found and opened.\n");
				libusb_free_device_list(usbdevlist, 1);
				return probe_usbdevh;
			}
			libusb_close(probe_usbdevh);
		}
	}
	fprintf(stderr, "usb error: can't find device.\n");

	if (usbdevlist)
		libusb_free_device_list(usbdevlist, 1);
	return NULL;
}

static flag_t usb_initreceivecallback(void) {
	static uint8_t usb_intinbuf[sizeof(nai_usbpacket_t)] = {0};

	usb_int_transfer = libusb_alloc_transfer(0);
	if (!usb_int_transfer)
		return 0;
	libusb_fill_interrupt_transfer(usb_int_transfer, usb_devh, EP_INT_IN, usb_intinbuf, sizeof(usb_intinbuf), usb_receive_int_cb, NULL, 0);
	// Initializing callback
	if (libusb_submit_transfer(usb_int_transfer) < 0) {
		libusb_free_transfer(usb_int_transfer);
		usb_int_transfer = NULL;
		return 0;
	}

	return 1;
}

static void usb_pollfd_added_cb(int fd, short events, void *user_data) {
	daemon_poll_addfd(fd, events);
}
static void usb_pollfd_removed_cb(int fd, void *user_data) {
	daemon_poll_removefd(fd);
}

flag_t usb_init() {
	struct libusb_config_descriptor *config = NULL;

	memset(&usb_state, 0, sizeof(usb_state));

	printf("usb: initializing libusb.\n");
	int r = libusb_init(&usb_ctx);
	if (r < 0) {
		fprintf(stderr, "usb error: libusb init error.\n");
		usb_ctx = NULL;
		return 0;
	}

	libusb_set_debug(usb_ctx, 3);
	printf("usb: setting poll fd notifiers.\n");
	libusb_set_pollfd_notifiers(usb_ctx, usb_pollfd_added_cb, usb_pollfd_removed_cb, NULL);

	printf("usb: opening device.\n");
	if ((usb_devh = usb_open(USB_VID, USB_PID)) == NULL) {
		fprintf(stderr, "usb error: can't open device.\n");
		usb_deinit();
		return 0;
	}

	printf("usb: resetting device.\n");
	if (libusb_reset_device(usb_devh) == LIBUSB_ERROR_NOT_FOUND) {
		printf("usb: reenumeration required.\n");
		libusb_close(usb_devh);
		sleep(1);
		if ((usb_devh = usb_open(USB_VID, USB_PID)) == NULL) {
			fprintf(stderr, "usb error: can't open device after reenumeration.\n");
			usb_deinit();
			return 0;
		}
	}

	if (libusb_kernel_driver_active(usb_devh, 0)) {
		printf("usb: detaching kernel driver.\n");
		libusb_detach_kernel_driver(usb_devh, 0);
	}
	printf("usb: setting configuration 1.\n");
	r = libusb_set_configuration(usb_devh, 1);
	if (r < 0) {
		fprintf(stderr, "usb error: set configuration error.\n");
		usb_deinit();
		return 0;
	}

	printf("usb: claiming interface.\n");
	r = libusb_claim_interface(usb_devh, 0);
	if (r < 0) {
		fprintf(stderr, "usb error: claim interface error.\n");
		usb_deinit();
		return 0;
	}
	usb_state.claimed = 1;

	printf("usb: getting active config descriptor.\n");
	r = libusb_get_active_config_descriptor(libusb_get_device(usb_devh), &config);
	if (r < 0) {
		if (config) {
			libusb_free_config_descriptor(config);
			config = NULL;
		}
		fprintf(stderr, "usb error: error getting active config descriptor.\n");
		usb_deinit();
		return 0;
	}

	printf("usb: setting interface alt setting.\n");
	if (config->interface->num_altsetting != 1) {
		r = libusb_set_interface_alt_setting(usb_devh, 0, 1);
		if (r < 0) {
			if (config) {
				libusb_free_config_descriptor(config);
				config = NULL;
			}
			fprintf(stderr, "usb error: can't set interface alt setting.\n");
			usb_deinit();
			return 0;
		}
	}
	if (config) {
		libusb_free_config_descriptor(config);
		config = NULL;
	}

	printf("usb: initializing int receive transfer and callback.\n");
	if (!usb_initreceivecallback()) {
		fprintf(stderr, "usb error: can't initialize int receive callback.\n");
		usb_deinit();
		return 0;
	}

	usb_state.initfinished = 1;
	printf("usb: interface opened and configured.\n");
	return 1;
}

void usb_deinit() {
	struct timeval tv = {0};

	printf("usb: deinit.\n");

	usb_state.initfinished = 0;

	if (usb_int_transfer && usb_int_transfer->status != LIBUSB_TRANSFER_CANCELLED) {
		printf("usb: int transfer pending, cancelling.\n");
		if (libusb_cancel_transfer(usb_int_transfer) == 0) {
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			libusb_handle_events_timeout(usb_ctx, &tv);
		}
	}

	if (usb_int_transfer) {
		printf("usb: freeing int transfer.\n");
		libusb_free_transfer(usb_int_transfer);
		usb_int_transfer = NULL;
	}

	if (usb_state.claimed) {
		printf("usb: releasing interface.\n");
		libusb_release_interface(usb_devh, 0);
		usb_state.claimed = 0;
	}
	if (usb_devh > 0) {
		printf("usb: closing device.\n");
		libusb_close(usb_devh);
		usb_devh = NULL;
	}
	if (usb_ctx) {
		printf("usb: freeing usb context.\n");
		libusb_exit(usb_ctx);
		usb_ctx = NULL;
	}
	memset(&usb_state, 0, sizeof(usb_state));
	printf("usb: deinit finished.\n");
}

flag_t usb_process(void) {
	nai_usbpacket_t usbpacket = {0};
	struct timeval tv = {0};

	if (usb_state.error)
		return 0;

	if (!usb_state.connected && !usb_state.checkboardsent && usb_state.initfinished) {
		printf("usb: interface initialized, sending checkboard command.\n");
		usbpacket.type = NAI_USBPACKET_TYPE_CHECKBOARD;
		usb_send_int((uint8_t *)&usbpacket, sizeof(nai_usbpacket_t));
		usb_state.checkboardsent = 1;
	}

	if (usb_state.initfinished) {
		tv.tv_sec = tv.tv_usec = 0;
		int r = libusb_handle_events_timeout(usb_ctx, &tv);
		if (r < 0) {
			fprintf(stderr, "usb error: libusb handle events error.\n");
			usb_state.error = 1;
			return 0;
		}
		if (libusb_get_next_timeout(usb_ctx, &tv))
			daemon_poll_setmaxtimeout(tv.tv_sec * 1000 + tv.tv_usec / 1000.0);
	}

	return 1;
}
