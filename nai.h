#ifndef NAI_H_
#define NAI_H_

#include "types.h"

void nai_usbpacket_received(nai_usbpacket_t *cmd);
void nai_processconsolecommand(char *cmd);

#endif
