#ifndef USB_H_
#define USB_H_

#include "types.h"
#include "../common/usbprotocol.h"

void usb_send_naipacket(nai_usbpacket_t *usbpacket);
flag_t usb_init();
void usb_deinit();
flag_t usb_process(void);

#endif
