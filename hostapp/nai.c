#include "nai.h"
#include "usb.h"
#include "config.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define CHECKBOARDTIMEOUTINSEC			3
#define CHECKBOARDSENDPERIODINSEC		15
#define CHECKEEPROMCOUNTERPERIODINSEC	30

static struct {
	time_t checkboardsentat;
	time_t gotcheckboardat;
	time_t geteepromcountersentat;
	flag_t connected;
} nai_state = {0};

// Starts a get status byte transfer.
void nai_getstatusbyte(void) {
	nai_usbpacket_t usbpacket = {0};

	printf("nai: sending getstatusbyte packet.\n");

	usbpacket.type = NAI_USBPACKET_TYPE_GETSTATUSBYTE;
	usb_send_naipacket(&usbpacket);
}

// Starts a get EEPROM counter transfer.
void nai_geteepromcounter(void) {
	nai_usbpacket_t usbpacket = {0};

	printf("nai: sending get eeprom counter packet.\n");

	usbpacket.type = NAI_USBPACKET_TYPE_GETEEPROMCOUNTER;
	usb_send_naipacket(&usbpacket);
	nai_state.geteepromcountersentat = time(NULL);
}

// Starts a reset interrupts transfer.
void nai_resetinterrupts(void) {
	nai_usbpacket_t usbpacket = {0};

	printf("nai: sending reset interrupts packet.\n");

	usbpacket.type = NAI_USBPACKET_TYPE_RESETINTERRUPTS;
	usb_send_naipacket(&usbpacket);
}

// Starts a checkboard transfer.
void nai_checkboard(void) {
	nai_usbpacket_t usbpacket = {0};

	printf("nai: sending checkboard packet.\n");

	usbpacket.type = NAI_USBPACKET_TYPE_CHECKBOARD;
	usb_send_naipacket(&usbpacket);
	nai_state.checkboardsentat = time(NULL);
}

// This gets called when an alarm interrupt is received (one of the interrupt flags
// were 1 in the received status byte).
static void nai_gotinterrupt(nai_statusbyte_t statusbyte) {
	static time_t lastrunonalarm = 0;
	char *runonalarm = NULL;
	char cmd[255] = {0};

	printf("nai: got interrupt.\n");

	if (time(NULL)-lastrunonalarm > config_get_runonalarmminimumdelayinsecbetweenruns()) {
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
		lastrunonalarm = time(NULL);
	} else
		fprintf(stderr, "nai: minimum delay between onalarm runs not spent, not running command.\n");

	nai_resetinterrupts();
}

// This gets called when the EEPROM counter is increased (the received counter value
// was greater than the stored one).
static void nai_eepromcounterhasincreased(int newpage, int newaddress) {
	static time_t lastrunoneepromcounterincrease = 0;
	char *runoneepromcounterincrease = NULL;
	char cmd[255] = {0};

	printf("nai: eeprom counter has increased.\n");

	if (time(NULL)-lastrunoneepromcounterincrease > config_get_runoneepromcounterincreaseminimumdelayinsecbetweenruns()) {
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
	} else
		fprintf(stderr, "nai: minimum delay between on eeprom counter increase runs not spent, not running command.\n");

	// Storing the new EEPROM counter values.
	config_set_eepromcounter_page(newpage);
	config_set_eepromcounter_address(newaddress);
}

// This gets called when a nai packet is received.
void nai_usb_packet_received_cb(nai_usbpacket_t *usbpacket) {
	nai_statusbyte_t *statusbyte = NULL;

	switch (usbpacket->type) {
		case NAI_USBPACKET_TYPE_CHECKBOARD | NAI_USBPACKET_TYPE_RESPONSE:
			printf("nai: received checkboard response.\n");
			nai_state.checkboardsentat = 0;
			nai_state.gotcheckboardat = time(NULL);
			break;
		case NAI_USBPACKET_TYPE_RESETINTERRUPTS | NAI_USBPACKET_TYPE_RESPONSE:
			printf("nai: received reset interrupts response.\n");
			break;
		case NAI_USBPACKET_TYPE_GETEEPROMCOUNTER | NAI_USBPACKET_TYPE_RESPONSE:
			printf("nai: received get eeprom counter response: page %d address %d.\n",
				usbpacket->payload[0], usbpacket->payload[1]);

			if (usbpacket->payload[0] != config_get_eepromcounter_page() ||
				usbpacket->payload[1] != config_get_eepromcounter_address())
					nai_eepromcounterhasincreased(usbpacket->payload[0], usbpacket->payload[1]);
				else
					printf("nai: eeprom counter check ok.\n");
			break;
		case NAI_USBPACKET_TYPE_GETSTATUSBYTE | NAI_USBPACKET_TYPE_RESPONSE:
			printf("nai: received get status byte response.\n");
			statusbyte = (nai_statusbyte_t *)&usbpacket->payload[0];
			printf("nai: statusbyte: P1state - %d\n" \
				   "                 P1int   - %d\n" \
				   "                 P2state - %d\n" \
				   "                 P2int   - %d\n" \
				   "                 P3state - %d\n" \
				   "                 P3int   - %d\n" \
				   "                 P4state - %d\n" \
				   "                 P4int   - %d\n",
				   		statusbyte->p1state,
				   		statusbyte->p1int,
				   		statusbyte->p2state,
				   		statusbyte->p2int,
				   		statusbyte->p3state,
				   		statusbyte->p3int,
				   		statusbyte->p4state,
				   		statusbyte->p4int);

			if (statusbyte->p1int || statusbyte->p2int || statusbyte->p3int || statusbyte->p4int)
				nai_gotinterrupt(*statusbyte);
			else
				printf("nai: status byte ok, no interrupt happened.\n");
			break;
	}
}

// This gets called when the USB interface is successfully connected.
void nai_usb_connected_cb(void) {
	printf("nai: usb interface connected.\n");
	nai_state.connected = 1;
	nai_geteepromcounter();
	nai_getstatusbyte();
}

flag_t nai_process(void) {
	if (nai_state.connected) {
		if (nai_state.checkboardsentat) { // Checkboard already sent but no response received yet
			if (time(NULL) - nai_state.checkboardsentat > CHECKBOARDTIMEOUTINSEC) {
				// This happens when we can't reach the USB interface (it won't respond to
				// checkboard requests).
				fprintf(stderr, "nai error: checkboard timeout.\n");
				return 0;
			}
		} else { // Checkboard not sent yet
			if (time(NULL) - nai_state.gotcheckboardat > CHECKBOARDSENDPERIODINSEC) {
				// Checking board periodically.
				printf("nai: periodic board check timeout.\n");
				nai_checkboard();
			}
		}
		if (time(NULL) - nai_state.geteepromcountersentat > CHECKEEPROMCOUNTERPERIODINSEC) {
			// Checking the EEPROM counter periodically.
			printf("nai: check eeprom counter timeout.\n");
			nai_geteepromcounter();
		}
	}

	return 1;
}
