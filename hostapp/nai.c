#include "nai.h"
#include "usb.h"
#include "config.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define CHECKBOARDTIMEOUTINSEC		3
#define CHECKBOARDSENDPERIODINSEC	15

static time_t nai_checkboardsentat = 0;
static time_t nai_gotcheckboardat = 0;
static flag_t nai_connected = 0;

void nai_getstatusbyte(void) {
	nai_usbpacket_t usbpacket = {0};

	printf("nai: sending getstatusbyte packet.\n");

	usbpacket.type = NAI_USBPACKET_TYPE_GETSTATUSBYTE;
	usb_send_naipacket(&usbpacket);
}

void nai_geteepromcounter(void) {
	nai_usbpacket_t usbpacket = {0};

	printf("nai: sending get eeprom counter packet.\n");

	usbpacket.type = NAI_USBPACKET_TYPE_GETEEPROMCOUNTER;
	usb_send_naipacket(&usbpacket);
}

void nai_resetinterrupts(void) {
	nai_usbpacket_t usbpacket = {0};

	printf("nai: sending reset interrupts packet.\n");

	usbpacket.type = NAI_USBPACKET_TYPE_RESETINTERRUPTS;
	usb_send_naipacket(&usbpacket);
}

void nai_checkboard(void) {
	nai_usbpacket_t usbpacket = {0};

	printf("nai: sending checkboard packet.\n");

	usbpacket.type = NAI_USBPACKET_TYPE_CHECKBOARD;
	usb_send_naipacket(&usbpacket);
	nai_checkboardsentat = time(NULL);
}

static void nai_gotinterrupt(nai_statusbyte_t statusbyte) {
	char *runonalarm = NULL;
	char cmd[255] = {0};

	printf("nai: got interrupt.\n");
	runonalarm = config_get_runonalarm();
	snprintf(cmd, sizeof(cmd), "%s %d %d %d %d %d %d %d %d", runonalarm,
		statusbyte.p1state,
		statusbyte.p1int,
		statusbyte.p2state,
		statusbyte.p2int,
		statusbyte.p3state,
		statusbyte.p3int,
		statusbyte.p4state,
		statusbyte.p4int);
	printf("nai: executing: %s\n", cmd);
	if (system(cmd) < 0)
		fprintf(stderr, "nai error: error on exec.\n");
	free(runonalarm);
}

static void nai_eepromcounterhasincreased(int newpage, int newaddress) {
	char *runoneepromcounterincrease = NULL;
	char cmd[255] = {0};

	printf("nai: eeprom counter has increased.\n");
	runoneepromcounterincrease = config_get_runoneepromcounterincrease();
	snprintf(cmd, sizeof(cmd), "%s %d %d %d %d", runoneepromcounterincrease,
		newpage,
		newaddress,
		config_get_eepromcounter_page(),
		config_get_eepromcounter_address());
	printf("nai: executing: %s\n", cmd);
	if (system(cmd) < 0)
		fprintf(stderr, "nai error: error on exec.\n");
	free(runoneepromcounterincrease);
}

void nai_usb_packet_received_cb(nai_usbpacket_t *usbpacket) {
	nai_statusbyte_t *statusbyte = NULL;

	switch (usbpacket->type) {
		case NAI_USBPACKET_TYPE_CHECKBOARD | NAI_USBPACKET_TYPE_RESPONSE:
			printf("nai: received checkboard response.\n");
			nai_checkboardsentat = 0;
			nai_gotcheckboardat = time(NULL);
			break;
		case NAI_USBPACKET_TYPE_RESETINTERRUPTS | NAI_USBPACKET_TYPE_RESPONSE:
			printf("nai: received reset interrupts response.\n");
			break;
		case NAI_USBPACKET_TYPE_GETEEPROMCOUNTER | NAI_USBPACKET_TYPE_RESPONSE:
			printf("nai: received get eeprom counter response: page %d address %d.\n",
				usbpacket->payload[0], usbpacket->payload[1]);
			if (usbpacket->payload[0] != config_get_eepromcounter_page() ||
				usbpacket->payload[1] != config_get_eepromcounter_address()) {
					nai_eepromcounterhasincreased(usbpacket->payload[0], usbpacket->payload[1]);
					config_set_eepromcounter_page(usbpacket->payload[0]);
					config_set_eepromcounter_address(usbpacket->payload[1]);
				}
			break;
		case NAI_USBPACKET_TYPE_GETSTATUSBYTE | NAI_USBPACKET_TYPE_RESPONSE:
			printf("nai: received get status byte response.\n");
			statusbyte = (nai_statusbyte_t *)&usbpacket->payload[0];
			if (statusbyte->p1int || statusbyte->p2int || statusbyte->p3int || statusbyte->p4int)
				nai_gotinterrupt(*statusbyte);
			break;
	}
}

void nai_usb_connected_cb(void) {
	printf("nai: usb interface connected.\n");
	nai_connected = 1;
	nai_geteepromcounter();
	nai_getstatusbyte();
}

flag_t nai_process(void) {
	if (nai_connected) {
		if (nai_checkboardsentat) { // Checkboard already sent but no response received yet
			if (time(NULL) - nai_checkboardsentat > CHECKBOARDTIMEOUTINSEC) {
				fprintf(stderr, "nai error: checkboard timeout.\n");
				return 0;
			}
		} else { // Checkboard not sent yet
			if (time(NULL) - nai_gotcheckboardat > CHECKBOARDSENDPERIODINSEC) {
				printf("nai: periodic board check timeout.\n");
				nai_checkboard();
			}
		}
	}

	return 1;
}
