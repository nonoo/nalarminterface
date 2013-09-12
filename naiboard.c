#include "boardconfig.h"

#include <avr/sysclk.h>
#include <avr/wdt_driver.h>
#include <avr/port_driver.h>

#include "naiboard.h"
#include "naiboard-led.h"
#include "naiboard-usb.h"
#include "naiboard-rtc.h"
#include "naiboard-uart.h"
#include "naiboard-eeprom.h"
#include "powersaving.h"

volatile naiboard_state_t naiboard_state;
extern volatile nai_statusbyte_t nai_statusbyte;

void naiboard_readstatus(void) {
	nai_statusbyte.p1state = (PORT_GetPortValue(&P1PORT) & P1PIN) ? 1 : 0;
	nai_statusbyte.p2state = (PORT_GetPortValue(&P2PORT) & P2PIN) ? 1 : 0;
	nai_statusbyte.p3state = (PORT_GetPortValue(&P3PORT) & P3PIN) ? 1 : 0;
	nai_statusbyte.p4state = (PORT_GetPortValue(&P4PORT) & P4PIN) ? 1 : 0;
}

void naiboard_ports_init(void) {
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
}
ISR(P2INTVECT) {
	nai_statusbyte.p2int = 1;
}
ISR(P3INTVECT) {
	nai_statusbyte.p3int = 1;
}
ISR(P4INTVECT) {
	nai_statusbyte.p4int = 1;
}

void naiboard_reset(void) {
	cli();
	wdt_enable(WDTO_15MS);
	while (1)
		;
}

void naiboard_delay_ms(uint16_t ms) {
	uint16_t i;
	for (i = 0; i < ms; i++) {
		_delay_ms(1);
		WDT_Reset(); // Watchdog reset
	}
}

void naiboard_delay_us(uint16_t us) {
	uint16_t i;
	for (i = 0; i < us; i++) {
		_delay_us(1);
		WDT_Reset(); // Watchdog reset
	}
}

void naiboard_sleep(void) {
	if (naiboard_state.usb_vendor_enabled)
		return;
	SLEEP.CTRL = SLEEP_SMODE_IDLE_gc;

	sysclk_disable_peripheral_clock(&RTC);
	//printf_P(PSTR("sleep\n"));

	WDT_Disable();
	sleep_enable();
	cpu_sleep();

	// Waking up
	sleep_disable();
	WDT_Enable();
	sysclk_enable_peripheral_clock(&RTC);
	//printf_P(PSTR("wake\n"));
}

static void naiboard_enable_interrupts(void) {
	// Enabling all interrupt levels
	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
	// Enabling global interrupts
	sei();
}

void naiboard_init(void) {
	WDT_EnableAndSetTimeout(WDT_PER_2KCLK_gc);

	POWERSAVING_DISABLE_ALL_MODULES();
	DISABLE_JTAG();
	POWERSAVING_ENABLE_RTC();
	POWERSAVING_ENABLE_USART();

	naiboard_enable_interrupts();
	sysclk_init();

	naiboard_led_init();
	naiboard_uart_init();
	naiboard_usb_init();
	naiboard_eeprom_init();
	naiboard_ports_init();

	naiboard_led1_on();
	naiboard_delay_ms(50);
	naiboard_led1_off();

	naiboard_rtc_init();
}
