#ifndef NAIBOARD_EEPROM_H_
#define NAIBOARD_EEPROM_H_

#include <stdint.h>

void naiboard_eeprom_readcounter(void);
uint8_t naiboard_eeprom_readstatusbyte(void);
void naiboard_eeprom_writestatusbyte(uint8_t statusbyte);
void naiboard_eeprom_init(void);

#endif
