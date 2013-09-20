#include "boardconfig.h"

#include <avr/avr_compiler.h>

#include "nai.h"
#include "nai-console.h"
#include "naiboard-usb.h"
#include "naiboard-adc.h"
#include "naiboard-eeprom.h"
#include "naiboard-ports.h"
#include "naiboard.h"
#include "types.h"

#include <string.h>
#include <stdio.h>

extern usb_buf_t naiboard_usb_sendbuf;
extern uint8_t naiboard_eepromcounter_page;
extern uint8_t naiboard_eepromcounter_addr;
extern volatile timestamp_t naiboard_time;
extern volatile naiboard_state_t naiboard_state;

volatile nai_statusbyte_t nai_statusbyte;
volatile nai_flags_t nai_flags;

static void nai_usbpacket_send(nai_usbpacket_t *cmd) {
	memcpy(naiboard_usb_sendbuf, cmd, sizeof(nai_usbpacket_t));
	naiboard_usb_int_send(naiboard_usb_sendbuf, sizeof(nai_usbpacket_t));
}

// This gets called when a USB packet is received
void nai_usbpacket_received(nai_usbpacket_t *cmd) {
	nai_usbpacket_t response;

	printf_P(PSTR("nai_usbpacket_received(): "));

	memset(&response, 0, sizeof(nai_usbpacket_t));
	response.type = cmd->type | NAI_USBPACKET_TYPE_RESPONSE;
	switch (cmd->type) {
		case NAI_USBPACKET_TYPE_RESETINTERRUPTS:
			printf("nai: host requested interrupt reset.\n");
			nai_statusbyte.p1int = nai_statusbyte.p2int =
				nai_statusbyte.p3int = nai_statusbyte.p4int = 0;
			naiboard_ports_readstatus();
			nai_flags.eepromupdated = 0; // This will trigger writing the status byte to the EEPROM
			response.payload[0] = *(uint8_t*)&nai_statusbyte;
			nai_usbpacket_send(&response);
			break;
		case NAI_USBPACKET_TYPE_GETSTATUSBYTE:
			naiboard_ports_readstatus();
			nai_flags.eepromupdated = 0;
			response.payload[0] = *(uint8_t*)&nai_statusbyte;
			nai_usbpacket_send(&response);
			break;
		case NAI_USBPACKET_TYPE_GETEEPROMCOUNTER:
			response.payload[0] = naiboard_eepromcounter_page;
			response.payload[1] = naiboard_eepromcounter_addr;
			nai_usbpacket_send(&response);
			break;
		case NAI_USBPACKET_TYPE_CHECKBOARD:
			nai_usbpacket_send(&response);
			// After this command we consider the host to be connected.
			naiboard_state.usb_connected = 1;
			break;
	}
}

static timestamp_t nai_calctimediff(timestamp_t t1, timestamp_t t2) {
	if (t1 >= t2)
   	    return t1 - t2;
    else
   	    return t1 + (256 - t2);
}

void nai_process(void) {
	static timestamp_t laststatussendat;
	timestamp_t diff;
	nai_usbpacket_t usbpacket;

	if (!nai_flags.eepromupdated) {
		printf_P(PSTR("nai: status byte needs updating in the eeprom.\n"));
		naiboard_eeprom_writestatusbyte(*(uint8_t*)&nai_statusbyte);
		nai_flags.eepromupdated = 1;
	}

	// There was an interrupt?
	if (nai_statusbyte.p1int || nai_statusbyte.p2int || nai_statusbyte.p3int || nai_statusbyte.p4int) {
		diff = nai_calctimediff(naiboard_time, laststatussendat);
		if (diff > STATUSBYTESENDINTERVALINTICKS) {
			// Sending the status byte to the host periodically
			printf_P(PSTR("nai: sending status byte to the host...\n"));
			memset(&usbpacket, 0, sizeof(nai_usbpacket_t));
			usbpacket.type = NAI_USBPACKET_TYPE_GETSTATUSBYTE | NAI_USBPACKET_TYPE_RESPONSE;
			usbpacket.payload[0] = *(uint8_t*)&nai_statusbyte;
			nai_usbpacket_send(&usbpacket);

			laststatussendat = naiboard_time;
		}
	}
}

void nai_init(void) {
	*(uint8_t*)&nai_statusbyte = naiboard_eeprom_readstatusbyte();
	nai_flags.eepromupdated = 1;
	nai_console_printeepromcounter();
	nai_console_printstatusbyte();
}
