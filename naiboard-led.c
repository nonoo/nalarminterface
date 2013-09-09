#include "boardconfig.h"

#include "naiboard-led.h"

#include <avr/port_driver.h>

static void naiboard_led1_init(void) {
    LED1PORT.DIRSET = LED1PIN; // TODO
    LED1PORT.OUTCLR = LED1PIN;
}

void naiboard_led1_on(void) {
    LED1PORT.OUTCLR = LED1PIN;
}

void naiboard_led1_off(void) {
    LED1PORT.OUTSET = LED1PIN;
}

void naiboard_led_init(void) {
	naiboard_led1_init();
	naiboard_led1_off();
}
