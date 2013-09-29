#include "boardconfig.h"

#include "types.h"
#include "../common/usbprotocol.h"

#include <avr/port_driver.h>

extern volatile nai_statusbyte_t nai_statusbyte;
extern volatile nai_flags_t nai_flags;

void naiboard_ports_readstatus(void) {
	// Inputs are inverted because of the pullup.
	// Of course we could set the pins as inverted using PORT_ConfigurePins() but
	// I think this was it's more straightforward :)
	nai_statusbyte.p1state = (PORT_GetPortValue(&P1PORT) & P1PIN) ? 0 : 1;
	nai_statusbyte.p2state = (PORT_GetPortValue(&P2PORT) & P2PIN) ? 0 : 1;
	nai_statusbyte.p3state = (PORT_GetPortValue(&P3PORT) & P3PIN) ? 0 : 1;
	nai_statusbyte.p4state = (PORT_GetPortValue(&P4PORT) & P4PIN) ? 0 : 1;
}

void naiboard_ports_init(void) {
	// Setting pins P1-P4 as inputs with interrupts happening on transitions on both edges.
	PORT_SetPinsAsInput(&P1PORT, P1PIN);
	PORT_ConfigurePins(&P1PORT, P1PIN, false, false, PORT_OPC_PULLUP_gc, PORT_ISC_BOTHEDGES_gc);
	PORT_SetPinsAsInput(&P2PORT, P2PIN);
	PORT_ConfigurePins(&P2PORT, P2PIN, false, false, PORT_OPC_PULLUP_gc, PORT_ISC_BOTHEDGES_gc);
	PORT_SetPinsAsInput(&P3PORT, P3PIN);
	PORT_ConfigurePins(&P3PORT, P3PIN, false, false, PORT_OPC_PULLUP_gc, PORT_ISC_BOTHEDGES_gc);
	PORT_SetPinsAsInput(&P4PORT, P4PIN);
	PORT_ConfigurePins(&P4PORT, P4PIN, false, false, PORT_OPC_PULLUP_gc, PORT_ISC_BOTHEDGES_gc);

	// Setting up interrupts for the pins
	P1INTCONFIG(&P1PORT, P1INTLVL, P1PIN);
	P2INTCONFIG(&P2PORT, P2INTLVL, P2PIN);
	P3INTCONFIG(&P3PORT, P3INTLVL, P3PIN);
	P4INTCONFIG(&P4PORT, P4INTLVL, P4PIN);
}

// These are the interrupt handlers for the watched pins
ISR(P1INTVECT) {
	nai_statusbyte.p1int = 1;
	nai_flags.eepromupdated = 0; // This will trigger EEPROM value update.
}
ISR(P2INTVECT) {
	nai_statusbyte.p2int = 1;
	nai_flags.eepromupdated = 0;
}
ISR(P3INTVECT) {
	nai_statusbyte.p3int = 1;
	nai_flags.eepromupdated = 0;
}
ISR(P4INTVECT) {
	nai_statusbyte.p4int = 1;
	nai_flags.eepromupdated = 0;
}
