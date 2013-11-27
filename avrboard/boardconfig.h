#ifndef BOARDCONFIG_H_
#define BOARDCONFIG_H_

// This is the MCIU clock define needed by the sleep functions.
// We're running at 48MHz because of the USB interface.
#undef F_CPU
#define F_CPU							48000000UL
// The MCU has this much EEPROM pages and bytes per page.
#define EEPROMBYTESPERPAGE				16
#define EEPROMPAGECOUNT					128
// How many times the RTC interrupt will be called in a second.
#define RTCCALLPERSEC					10
// If an interrupt is active, send the status byte to the host on every nth
// RTC interrupt call.
#define STATUSBYTESENDINTERVALINTICKS	3
// This many characters can be entered to the command line.
#define STDINBUFFERSIZE					25

// The interface's self-assigned dummy VID and PID.
#define USB_VID							0x4566
#define USB_PID							0x0000

// The console's USART module's location and pins.
#define USARTMODULE						USARTE0
#define USARTMODULERXCINTVECT			USARTE0_RXC_vect
#define USARTMODULEDREINTVECT			USARTE0_DRE_vect
#define USARTPORT						PORTE
#define USARTRXPIN						PIN2_bm
#define USARTTXPIN						PIN3_bm

// We only have 1 LED on the board.
#define LED1PORT						PORTA
#define LED1PIN							PIN0_bm

// The following defines are the settings for the 4 inputs (P1-P4).
#define P1PORT							PORTF
#define P1PIN							PIN6_bm
#define P1INTLVL						PORT_INT0LVL_MED_gc
#define P1INTLVL_OFF					PORT_INT0LVL_OFF_gc
#define P1INTVECT						PORTF_INT0_vect
#define P1INTCONFIG(a, b, c)			PORT_ConfigureInterrupt0(a, b, c)

#define P2PORT							PORTF
#define P2PIN							PIN5_bm
#define P2INTLVL						PORT_INT1LVL_MED_gc
#define P2INTLVL_OFF					PORT_INT1LVL_OFF_gc
#define P2INTVECT						PORTF_INT1_vect
#define P2INTCONFIG(a, b, c)			PORT_ConfigureInterrupt1(a, b, c)

#define P3PORT							PORTE
#define P3PIN							PIN5_bm
#define P3INTLVL						PORT_INT0LVL_MED_gc
#define P3INTLVL_OFF					PORT_INT0LVL_OFF_gc
#define P3INTVECT						PORTE_INT0_vect
#define P3INTCONFIG(a, b, c)			PORT_ConfigureInterrupt0(a, b, c)

#define P4PORT							PORTB
#define P4PIN							PIN6_bm
#define P4INTLVL						PORT_INT1LVL_MED_gc
#define P4INTLVL_OFF					PORT_INT1LVL_OFF_gc
#define P4INTVECT						PORTB_INT1_vect
#define P4INTCONFIG(a, b, c)			PORT_ConfigureInterrupt1(a, b, c)

#endif
