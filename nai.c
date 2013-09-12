#include "boardconfig.h"

#include <avr/avr_compiler.h>

#include "nai.h"
#include "naiboard-usb.h"
#include "naiboard-adc.h"
#include "naiboard-eeprom.h"
#include "naiboard.h"
#include "types.h"

#include <avr/port_driver.h>

#include <string.h>
#include <stdio.h>

extern usb_buf_t naiboard_usb_sendbuf;

volatile nai_statusbyte_t nai_statusbyte;

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
			naiboard_readstatus();
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

void nai_printvcc(void) {
	printf_P(PSTR("vcc: %fV\n"), naiboard_get_vcc());
}


void nai_processconsolecommand(char *buffer) {
	char *tok;

	tok = strtok(buffer, " "); // Getting the command
	if (tok == NULL)
		return;

	if (strcmp(tok, "help") == 0 || strcmp(tok, "h") == 0) {
		printf_P(PSTR("  rst     - reset\n"));
		printf_P(PSTR("  vcp     - uc vcc print\n"));
		printf_P(PSTR("  stp     - status byte print\n"));
		printf_P(PSTR("  ecp     - eeprom counter print\n"));
		return;
	}
	if (strcmp(tok, "rst") == 0) {
		naiboard_reset();
		return;
	}
	if (strcmp(tok, "vcp") == 0) {
		nai_printvcc();
		return;
	}
	if (strcmp(tok, "stp") == 0) {
		printf_P(PSTR("statusbyte: P1state - %d\n"), nai_statusbyte.p1state);
		printf_P(PSTR("            P1int   - %d\n"), nai_statusbyte.p1int);
		printf_P(PSTR("            P2state - %d\n"), nai_statusbyte.p2state);
		printf_P(PSTR("            P2int   - %d\n"), nai_statusbyte.p2int);
		printf_P(PSTR("            P3state - %d\n"), nai_statusbyte.p3state);
		printf_P(PSTR("            P3int   - %d\n"), nai_statusbyte.p3int);
		printf_P(PSTR("            P4state - %d\n"), nai_statusbyte.p4state);
		printf_P(PSTR("            P4int   - %d\n"), nai_statusbyte.p4int);
		return;
	}
	if (strcmp(tok, "ecp") == 0) {
		printf_P(PSTR("eeprom counter: %d\n"), naiboard_eeprom_readcounter());
		return;
	}
}
