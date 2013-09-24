#include "boardconfig.h"

#include "naiboard-eeprom.h"

#include <avr/eeprom_driver.h>

#include <stdio.h>

// Note: every value written and read to/from the EEPROM is negated,
// because every byte is 0xff by factory default.

#define EE_COUNTER_PAGE			0
#define EE_COUNTER_PAGEVAL		0
#define EE_COUNTER_ADDRVAL		1

// These will be the default init values
#define EE_COUNTER_INIT_PAGE	0
#define EE_COUNTER_INIT_ADDR	2

uint8_t naiboard_eepromcounter_page;
uint8_t naiboard_eepromcounter_addr;

void naiboard_eeprom_readcounter(void) {
	naiboard_eepromcounter_page = ~EEPROM_ReadByte(EE_COUNTER_PAGE, EE_COUNTER_PAGEVAL);
	naiboard_eepromcounter_addr = ~EEPROM_ReadByte(EE_COUNTER_PAGE, EE_COUNTER_ADDRVAL);
}

static void naiboard_eeprom_writecounter(void) {
	EEPROM_WriteByte(EE_COUNTER_PAGE, EE_COUNTER_PAGEVAL, ~naiboard_eepromcounter_page);
	EEPROM_WriteByte(EE_COUNTER_PAGE, EE_COUNTER_ADDRVAL, ~naiboard_eepromcounter_addr);
}

static void naiboard_eeprom_increasecounter(void) {
	printf_P(PSTR("naiboard-eeprom: increasing eeprom counter.\n"));

	naiboard_eepromcounter_addr++;
	if (naiboard_eepromcounter_addr >= EEPROMBYTESPERPAGE) { // Max. this many bytes per page
		naiboard_eepromcounter_addr = 0;
		naiboard_eepromcounter_page++;
		if (naiboard_eepromcounter_page >= EEPROMPAGECOUNT) // Max. this many pages on this MCU
			naiboard_eepromcounter_page = 0;
	}

	naiboard_eeprom_writecounter();
}

uint8_t naiboard_eeprom_readstatusbyte(void) {
	return ~EEPROM_ReadByte(naiboard_eepromcounter_page, naiboard_eepromcounter_addr);
}

void naiboard_eeprom_writestatusbyte(uint8_t statusbyte) {
	if (naiboard_eeprom_readstatusbyte() == statusbyte) // Not updating if not necessary
		return;

	EEPROM_WriteByte(naiboard_eepromcounter_page, naiboard_eepromcounter_addr, ~statusbyte);

	if (naiboard_eeprom_readstatusbyte() != statusbyte) { // If the read back value is not correct
		naiboard_eeprom_increasecounter();
		// Trying writing again to a different EEPROM byte
		EEPROM_WriteByte(naiboard_eepromcounter_page, naiboard_eepromcounter_addr, ~statusbyte);
	}
}

void naiboard_eeprom_init(void) {
	EEPROM_FlushBuffer();
	EEPROM_DisableMapping();

	naiboard_eeprom_readcounter();

	if (naiboard_eepromcounter_page == EE_COUNTER_PAGE &&
		(naiboard_eepromcounter_addr == EE_COUNTER_PAGEVAL ||
		naiboard_eepromcounter_addr == EE_COUNTER_ADDRVAL)) {
			naiboard_eepromcounter_page = EE_COUNTER_INIT_PAGE;
			naiboard_eepromcounter_addr = EE_COUNTER_INIT_ADDR;
			naiboard_eeprom_writecounter();
	}
}
