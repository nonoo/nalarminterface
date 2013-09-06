#ifndef NAIBOARD_H_
#define NAIBOARD_H_

#include "types.h"

typedef struct {
	uint16_t usb_blinkrxtxledpulsecount;
	uint8_t usb_connected : 1;
	uint8_t usb_vendor_enabled : 1;
	uint8_t usb_ledstate : 1;
} naiboard_state_t;

void naiboard_uart_init(void);

void naiboard_delay_ms(uint16_t ms);
void naiboard_delay_us(uint16_t us);

void naiboard_led1_on(void);
void naiboard_led1_off(void);
void naiboard_led_init(void);

void naiboard_sleep(void);

#define USBBUFSIZE sizeof(nai_usbpacket_t)
typedef uint8_t usb_buf_t[USBBUFSIZE];

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

void naiboard_init(void);

#endif
