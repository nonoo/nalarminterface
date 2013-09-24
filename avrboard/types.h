#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>

typedef uint8_t flag_t;
typedef uint8_t timestamp_t;

typedef struct {
	uint8_t eepromupdated : 1;
} nai_flags_t;

typedef struct {
	uint16_t usb_blinkrxtxledpulsecount;
	uint8_t usb_connected : 1;
	uint8_t usb_vendor_enabled : 1;
	uint8_t usb_ledstate : 1;
} naiboard_state_t;

#include "../common/usbprotocol.h"

#endif
