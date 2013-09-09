#include "boardconfig.h"

#include <avr/avr_compiler.h>

#include "nai.h"
#include "naiboard-usb.h"
#include "naiboard-eeprom.h"
#include "naiboard.h"
#include "types.h"

#include <avr/port_driver.h>

#include <string.h>
#include <stdio.h>

extern usb_buf_t naiboard_usb_sendbuf;

static volatile nai_statusbyte_t nai_statusbyte;

static void nai_usbpacket_send(nai_usbpacket_t *cmd) {
	memcpy(naiboard_usb_sendbuf, cmd, sizeof(nai_usbpacket_t));
	naiboard_usb_int_send(naiboard_usb_sendbuf, sizeof(nai_usbpacket_t));
}

// This gets called when a USB packet is received
void nai_usbpacket_received(nai_usbpacket_t *cmd) {
	nai_usbpacket_t response;
	uint16_t counter;

	printf_P(PSTR("nai_usbpacket_received(): "));

	memset(&response, 0, sizeof(nai_usbpacket_t));
	response.type = cmd->type | NAI_USBPACKET_TYPE_RESPONSE;
	switch (cmd->type) {
		case NAI_USBPACKET_TYPE_RESETINTERRUPTS:
			nai_statusbyte.p1int = nai_statusbyte.p2int =
				nai_statusbyte.p3int = nai_statusbyte.p4int = 0;
			naiboard_readstatus(&nai_statusbyte);
			response.payload[0] = *(uint8_t*)&nai_statusbyte;
			nai_usbpacket_send(&response);
			break;
		case NAI_USBPACKET_TYPE_GETSTATUSBYTE:
			response.payload[0] = *(uint8_t*)&nai_statusbyte;
			nai_usbpacket_send(&response);
			break;
		case NAI_USBPACKET_TYPE_GETEEPROMCOUNTER:
			counter = naiboard_eeprom_readcounter();
			memcpy(response.payload, &counter, 2);
			nai_usbpacket_send(&response);
			break;
		case NAI_USBPACKET_TYPE_CHECKBOARD:
			nai_usbpacket_send(&response);
			break;
	}
}
