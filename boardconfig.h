#ifndef BOARDCONFIG_H_
#define BOARDCONFIG_H_

#undef F_CPU
#define F_CPU					48000000UL
#define RTCCALLPERSEC			10

#define USB_VID					0x4566
#define USB_PID					0x0000

#define USARTMODULE				USARTE0
#define USARTPORT				PORTE
#define USARTRXPIN				PIN2_bm
#define USARTTXPIN				PIN3_bm

#define LED1PORT				PORTA
#define LED1PIN					PIN0_bm

#define P1PORT					PORTF
#define P1PIN					PIN6_bm
#define P1INTLVL				PORT_INT0LVL_MED_gc
#define P1INTLVL_OFF			PORT_INT0LVL_OFF_gc
#define P1INTVECT				PORTF_INT0_vect
#define P1INTCONFIG(a, b, c)	PORT_ConfigureInterrupt0(a, b, c)

#define P2PORT					PORTF
#define P2PIN					PIN5_bm
#define P2INTLVL				PORT_INT1LVL_MED_gc
#define P2INTLVL_OFF			PORT_INT1LVL_OFF_gc
#define P2INTVECT				PORTF_INT1_vect
#define P2INTCONFIG(a, b, c)	PORT_ConfigureInterrupt1(a, b, c)

#define P3PORT					PORTE
#define P3PIN					PIN6_bm
#define P3INTLVL				PORT_INT0LVL_MED_gc
#define P3INTLVL_OFF			PORT_INT0LVL_OFF_gc
#define P3INTVECT				PORTF_INT0_vect
#define P3INTCONFIG(a, b, c)	PORT_ConfigureInterrupt0(a, b, c)

#define P4PORT					PORTE
#define P4PIN					PIN5_bm
#define P4INTLVL				PORT_INT1LVL_MED_gc
#define P4INTLVL_OFF			PORT_INT1LVL_OFF_gc
#define P4INTVECT				PORTF_INT1_vect
#define P4INTCONFIG(a, b, c)	PORT_ConfigureInterrupt1(a, b, c)

#endif
