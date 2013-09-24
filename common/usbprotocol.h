#ifndef USBPROTOCOL_H_
#define USBPROTOCOL_H_

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
