#include "boardconfig.h"

#include "nai-console.h"
#include "naiboard.h"
#include "naiboard-adc.h"
#include "naiboard-eeprom.h"
#include "naiboard-ports.h"
#include "types.h"

#include <avr/avr_compiler.h>

#include <stdio.h>
#include <string.h>

extern uint8_t naiboard_eepromcounter_page;
extern uint8_t naiboard_eepromcounter_addr;
extern volatile timestamp_t naiboard_time;
extern volatile nai_statusbyte_t nai_statusbyte;

void nai_console_printvcc(void) {
	printf_P(PSTR("vcc: %fV\n"), naiboard_get_vcc());
}

void nai_console_printeepromcounter(void) {
	printf_P(PSTR("eeprom counter: page %d addr %d\n"),
		naiboard_eepromcounter_page, naiboard_eepromcounter_addr);
}

void nai_console_printstatusbyte(void) {
	printf_P(PSTR("statusbyte: P1state - %d\n"), nai_statusbyte.p1state);
	printf_P(PSTR("            P1int   - %d\n"), nai_statusbyte.p1int);
	printf_P(PSTR("            P2state - %d\n"), nai_statusbyte.p2state);
	printf_P(PSTR("            P2int   - %d\n"), nai_statusbyte.p2int);
	printf_P(PSTR("            P3state - %d\n"), nai_statusbyte.p3state);
	printf_P(PSTR("            P3int   - %d\n"), nai_statusbyte.p3int);
	printf_P(PSTR("            P4state - %d\n"), nai_statusbyte.p4state);
	printf_P(PSTR("            P4int   - %d\n"), nai_statusbyte.p4int);
}

static void nai_console_printcurrenttime(void) {
	printf_P(PSTR("current time: %u\n"), naiboard_time);
}

void nai_console_processcommand(char *buffer) {
	char *tok;

	tok = strtok(buffer, " "); // Getting the command
	if (tok == NULL)
		return;

	if (strcmp(tok, "help") == 0 || strcmp(tok, "h") == 0) {
		printf_P(PSTR("  rst     - reset\n"));
		printf_P(PSTR("  vcp     - uc vcc print\n"));
		printf_P(PSTR("  stp     - status byte print\n"));
		printf_P(PSTR("  ecp     - eeprom counter print\n"));
		printf_P(PSTR("  ctp     - current time print\n"));
		return;
	}
	if (strcmp(tok, "rst") == 0) {
		naiboard_reset();
		return;
	}
	if (strcmp(tok, "vcp") == 0) {
		nai_console_printvcc();
		return;
	}
	if (strcmp(tok, "stp") == 0) {
		naiboard_ports_readstatus();
		nai_console_printstatusbyte();
		return;
	}
	if (strcmp(tok, "ecp") == 0) {
		nai_console_printeepromcounter();
		return;
	}
	if (strcmp(tok, "ctp") == 0) {
		nai_console_printcurrenttime();
		return;
	}
}
