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

// This function copies the given nai packet to the fixed-size USB send buffer
// and starts the transfer.
static void nai_usbpacket_send(nai_usbpacket_t *cmd) {
	memcpy(naiboard_usb_sendbuf, cmd, sizeof(nai_usbpacket_t));
	naiboard_usb_int_send(naiboard_usb_sendbuf, sizeof(nai_usbpacket_t));
}

// Sends the EEPROM counter response packet to the host.
void nai_send_eepromcounterresponse(void) {
	nai_usbpacket_t response = {0};

	printf_P(PSTR("nai: sending eeprom counter response to the host.\n"));

	response.type = NAI_USBPACKET_TYPE_GETEEPROMCOUNTER | NAI_USBPACKET_TYPE_RESPONSE;
	response.payload[0] = naiboard_eepromcounter_page;
	response.payload[1] = naiboard_eepromcounter_addr;

	nai_usbpacket_send(&response);
}

// Sends the getstatusbyte response to the host.
void nai_send_getstatusbyteresponse(void) {
	nai_usbpacket_t response = {0};

	printf_P(PSTR("nai: sending status byte to the host.\n"));

	naiboard_ports_readstatus();
	nai_flags.eepromupdated = 0; // This will trigger writing the status byte to the EEPROM

	response.type = NAI_USBPACKET_TYPE_GETSTATUSBYTE | NAI_USBPACKET_TYPE_RESPONSE;
	response.payload[0] = *(uint8_t*)&nai_statusbyte;

	nai_usbpacket_send(&response);
}

// This gets called when a USB packet is received
void nai_usbpacket_received(nai_usbpacket_t *cmd) {
	nai_usbpacket_t response = {0};

	printf_P(PSTR("nai: received usb packet: "));

	memset(&response, 0, sizeof(nai_usbpacket_t));
	response.type = cmd->type | NAI_USBPACKET_TYPE_RESPONSE;
	switch (cmd->type) {
		case NAI_USBPACKET_TYPE_RESETINTERRUPTS:
			printf_P(PSTR("resetinterrupts\n"));
			nai_statusbyte.p1int = nai_statusbyte.p2int =
				nai_statusbyte.p3int = nai_statusbyte.p4int = 0;

			nai_send_getstatusbyteresponse();
			break;
		case NAI_USBPACKET_TYPE_GETSTATUSBYTE:
			printf_P(PSTR("getstatusbyte\n"));
			nai_send_getstatusbyteresponse();
			break;
		case NAI_USBPACKET_TYPE_GETEEPROMCOUNTER:
			printf_P(PSTR("geteepromcounter\n"));
			nai_send_eepromcounterresponse();
			break;
		case NAI_USBPACKET_TYPE_CHECKBOARD:
			printf_P(PSTR("checkboard\n"));
			printf_P(PSTR("nai: sending checkboard response.\n"));
			nai_usbpacket_send(&response);
			if (!naiboard_state.usb_connected) {
				// After this command we consider the host to be connected.
				printf_P(PSTR("nai: host is now connected.\n"));
				naiboard_state.usb_connected = 1;
			}
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
	timestamp_t diff = 0;

	if (!nai_flags.eepromupdated) {
		printf_P(PSTR("nai: status byte needs updating in the eeprom.\n"));
		naiboard_eeprom_writestatusbyte(*(uint8_t*)&nai_statusbyte);
		nai_flags.eepromupdated = 1;
	}

	// There was an interrupt?
	if (nai_statusbyte.p1int || nai_statusbyte.p2int || nai_statusbyte.p3int || nai_statusbyte.p4int) {
		diff = nai_calctimediff(naiboard_time, laststatussendat);
		if (diff > STATUSBYTESENDINTERVALINTICKS) {
			printf_P(PSTR("nai: interrupt active.\n"));
			// Sending the status byte to the host periodically
			nai_send_getstatusbyteresponse();
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
