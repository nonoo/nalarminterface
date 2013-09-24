#ifndef NAI_H_
#define NAI_H_

#include "types.h"
#include "../common/usbprotocol.h"

void nai_usb_packet_received_cb(nai_usbpacket_t *usbpacket);
void nai_usb_connected_cb(void);
flag_t nai_process(void);

#endif
