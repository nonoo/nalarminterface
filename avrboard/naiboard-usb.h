#ifndef NAIBOARD_USB_H_
#define NAIBOARD_USB_H_

#include "types.h"

void naiboard_usb_int_send(uint8_t *data, uint16_t length);

#include <avr/udc.h>
void naiboard_usb_vendor_int_in(udd_ep_status_t status, iram_size_t nb_transfered, udd_ep_id_t ep);
void naiboard_usb_vendor_int_out(udd_ep_status_t status, iram_size_t nb_transfered, udd_ep_id_t ep);

void naiboard_usb_init_int_recv();

bool naiboard_usb_vendor_enable(void);
void naiboard_usb_vendor_disable(void);
bool naiboard_usb_setup_out_received(void);
bool naiboard_usb_setup_in_received(void);
void naiboard_usb_vbus_action(bool b_high);
void naiboard_usb_suspend_action(void);
void naiboard_usb_resume_action(void);
void naiboard_usb_sof_action(void);
void naiboard_usb_init(void);

#endif
