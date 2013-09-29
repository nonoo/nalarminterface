#ifndef USBPROTOCOL_H_
#define USBPROTOCOL_H_

// This is the status byte which holds the pins state. This gets sent to the
// host application and stored in the EEPROM as well.
typedef struct {
	uint8_t p1state : 1; // This is 1 if P1 is 1
	uint8_t p1int 	: 1; // This is 1 if there was an interrupt on P1
	uint8_t p2state : 1;
	uint8_t p2int 	: 1;
	uint8_t p3state : 1;
	uint8_t p3int 	: 1;
	uint8_t p4state : 1;
	uint8_t p4int 	: 1;
} nai_statusbyte_t;

#define NAI_USBPACKET_TYPE_RESETINTERRUPTS	0
#define NAI_USBPACKET_TYPE_GETSTATUSBYTE	1
#define NAI_USBPACKET_TYPE_GETEEPROMCOUNTER	2
#define NAI_USBPACKET_TYPE_CHECKBOARD		3
#define NAI_USBPACKET_TYPE_RESPONSE			128

typedef struct {
	uint8_t type;
	uint8_t payload[2];
} nai_usbpacket_t;

#define USBBUFSIZE sizeof(nai_usbpacket_t)
typedef uint8_t usb_buf_t[USBBUFSIZE];

#endif
