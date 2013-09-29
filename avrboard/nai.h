#ifndef NAI_H_
#define NAI_H_

#include "types.h"
#include "../common/usbprotocol.h"

#define ISNAIINTERRUPTACTIVE() (nai_statusbyte.p1int || nai_statusbyte.p2int || nai_statusbyte.p3int || nai_statusbyte.p4int)

void nai_send_eepromcounterresponse(void);
void nai_send_getstatusbyteresponse(void);
void nai_usbpacket_received(nai_usbpacket_t *cmd);
void nai_process(void);
void nai_init(void);

#endif
