#include "boardconfig.h"

#include "naiboard-eeprom.h"

#include <avr/eeprom_driver.h>

#define EE_COUNTER_PAGE		0
#define EE_COUNTER_PAGEVAL	0
#define EE_COUNTER_ADDRVAL	1

uint8_t naiboard_eepromcounter_page;
uint8_t naiboard_eepromcounter_addr;

void naiboard_eeprom_readcounter(void) {
	naiboard_eepromcounter_page = EEPROM_ReadByte(EE_COUNTER_PAGE, EE_COUNTER_PAGEVAL);
	naiboard_eepromcounter_addr = EEPROM_ReadByte(EE_COUNTER_PAGE, EE_COUNTER_ADDRVAL);
}

static void naiboard_eeprom_writecounter(void) {
	EEPROM_WriteByte(EE_COUNTER_PAGE, EE_COUNTER_PAGEVAL, naiboard_eepromcounter_page);
	EEPROM_WriteByte(EE_COUNTER_PAGE, EE_COUNTER_ADDRVAL, naiboard_eepromcounter_addr);
}

static void naiboard_eeprom_increasecounter(void) {
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
	return EEPROM_ReadByte(naiboard_eepromcounter_page, naiboard_eepromcounter_addr);
}

void naiboard_eeprom_writestatusbyte(uint8_t statusbyte) {
	EEPROM_WriteByte(naiboard_eepromcounter_page, naiboard_eepromcounter_addr, statusbyte);

	if (naiboard_eeprom_readstatusbyte() != statusbyte) { // If the read back value is not correct
		naiboard_eeprom_increasecounter();
		// Trying writing again to a different EEPROM byte
		EEPROM_WriteByte(naiboard_eepromcounter_page, naiboard_eepromcounter_addr, statusbyte);
	}
}

void naiboard_eeprom_init(void) {
	EEPROM_FlushBuffer();
	EEPROM_DisableMapping();

	naiboard_eeprom_readcounter();
}
