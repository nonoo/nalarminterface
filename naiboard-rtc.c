#include "boardconfig.h"

#include "naiboard-rtc.h"
#include "naiboard-led.h"
#include "types.h"

#include <avr/sysclk.h>
#include <avr/rtc_driver.h>

// Period == 1024 -> 1 sec resolution
void naiboard_rtc_init(void) {
	sysclk_enable_peripheral_clock(&RTC);

	cli();
    /* Turn on internal 32kHz. */
    OSC.CTRL |= OSC_RC32KEN_bm;

    do {
        /* Wait for the 32kHz oscillator to stabilize. */
    } while ((OSC.STATUS & OSC_RC32KRDY_bm) == 0);

    /* Set internal 32kHz oscillator as clock source for RTC. */
    CLK.RTCCTRL = CLK_RTCSRC_RCOSC32_gc | CLK_RTCEN_bm;

    do {
		/* Wait until RTC is not busy. */
    } while (RTC_Busy());

    RTC_Initialize((uint16_t) 1024 / RTCCALLPERSEC, 0, 0, RTC_PRESCALER_DIV1_gc);

    /* Enable overflow interrupt. */
    RTC_SetIntLevels(RTC_OVFINTLVL_LO_gc, RTC_COMPINTLVL_OFF_gc);

    sei();
}

ISR(RTC_OVF_vect) {
	extern volatile naiboard_state_t naiboard_state;

	if (naiboard_state.usb_blinkrxtxledpulsecount) {
		if (naiboard_state.usb_ledstate)
			naiboard_led1_off();
		else
			naiboard_led1_on();
		naiboard_state.usb_ledstate = !naiboard_state.usb_ledstate;
		naiboard_state.usb_blinkrxtxledpulsecount--;
	} else {
		if (naiboard_state.usb_connected && naiboard_state.usb_ledstate == 0)
			naiboard_led1_on();
		if (!naiboard_state.usb_connected && naiboard_state.usb_ledstate)
			naiboard_led1_off();
	}
}
