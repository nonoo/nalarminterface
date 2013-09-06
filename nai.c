#include "boardconfig.h"

#include <avr/avr_compiler.h>

#include "nai.h"
#include "naiboard.h"
#include "types.h"

#include <avr/port_driver.h>

#include <string.h>
#include <stdio.h>

extern usb_buf_t naiboard_usb_sendbuf;

static void nai_usbpacket_send(nai_usbpacket_t *cmd) {
	memcpy(naiboard_usb_sendbuf, cmd, sizeof(nai_usbpacket_t));
	naiboard_usb_int_send(naiboard_usb_sendbuf, sizeof(nai_usbpacket_t));
}

// This gets called when a USB packet is received
void nai_usbpacket_received(nai_usbpacket_t *cmd) {
	nai_usbpacket_t response;

	printf_P(PSTR("nai_usbpacket_received(): "));

	memset(&response, 0, sizeof(nai_usbpacket_t));
	switch (*(uint8_t *)cmd) {
		// TODO
	}
}
