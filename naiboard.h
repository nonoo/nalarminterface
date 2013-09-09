#ifndef NAIBOARD_H_
#define NAIBOARD_H_

#include "types.h"

void naiboard_readstatus(volatile nai_statusbyte_t *statusbyte);

void naiboard_delay_ms(uint16_t ms);
void naiboard_delay_us(uint16_t us);

void naiboard_sleep(void);

void naiboard_init(void);

#endif
