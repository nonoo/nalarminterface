#ifndef USB_H_
#define USB_H_

#include "types.h"

int usb_send_int(uint8_t *data, int length);
flag_t usb_init();
void usb_process(void);

#endif
