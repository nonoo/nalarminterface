#include <avr/avr_compiler.h>

#include "boardconfig.h"
#include "naiboard.h"
#include "nai.h"

#include <avr/wdt_driver.h>

#include <stdio.h>
#include <stdint.h>

int main() {
	naiboard_init();

	naiboard_delay_ms(10);
   	printf_P(PSTR("\n\n\nnonoo's alarm interface v%d.%d\n" \
   		"built: " __TIME__ " " __DATE__ "\nusb vid=%.4x pid=%.4x sn=" USB_DEVICE_SERIAL_NAME "\n\n"), USB_DEVICE_MAJOR_VERSION, USB_DEVICE_MINOR_VERSION, USB_DEVICE_VENDOR_ID, USB_DEVICE_PRODUCT_ID);
	printf_P(PSTR("ready.\n"));

	while (1) {
		WDT_Reset();
		naiboard_sleep();
	}

    return 0;
}
