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
#include "naiboard-ports.h"
#include "powersaving.h"

volatile naiboard_state_t naiboard_state;

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
	naiboard_delay_ms(50);
	naiboard_led1_on();
	naiboard_delay_ms(50);
	naiboard_led1_off();
	naiboard_delay_ms(50);
	naiboard_led1_on();
	naiboard_delay_ms(50);
	naiboard_led1_off();

	naiboard_rtc_init();
}
