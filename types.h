#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>

typedef uint8_t flag_t;

typedef struct {
	uint8_t p1state : 1;
	uint8_t p1int 	: 1;
	uint8_t p2state : 1;
	uint8_t p2int 	: 1;
	uint8_t p3state : 1;
	uint8_t p3int 	: 1;
	uint8_t p4state : 1;
	uint8_t p4int 	: 1;
} nai_usbpacket_t;

#endif
