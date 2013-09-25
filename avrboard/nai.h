#ifndef NAI_H_
#define NAI_H_

#include "types.h"

void nai_send_eepromcounterresponse(void);
void nai_send_getstatusbyteresponse(void);
void nai_usbpacket_received(nai_usbpacket_t *cmd);
void nai_process(void);
void nai_init(void);

#endif
