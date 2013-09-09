#include "boardconfig.h"

#include "naiboard-eeprom.h"

#include <avr/eeprom_driver.h>

uint16_t naiboard_eeprom_readcounter(void) {
	return 0;
}

void naiboard_eeprom_init(void) {
	EEPROM_FlushBuffer();
	EEPROM_DisableMapping();
}
