#include "boardconfig.h"

#include "naiboard-led.h"

#include <avr/port_driver.h>

static void naiboard_led1_init(void) {
	PORT_SetPinsAsOutput(&LED1PORT, LED1PIN);
    naiboard_led1_off();
}

void naiboard_led1_on(void) {
	PORT_ClearPins(&LED1PORT, LED1PIN);
}

void naiboard_led1_off(void) {
	PORT_SetPins(&LED1PORT, LED1PIN);
}

void naiboard_led_init(void) {
	naiboard_led1_init();
}
