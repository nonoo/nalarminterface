#include <avr/avr_compiler.h>

#include "boardconfig.h"
#include "conf_usb.h"
#include "naiboard.h"
#include "naiboard-uart.h"
#include "nai.h"
#include "nai-console.h"

#include <avr/wdt_driver.h>

#include <stdio.h>
#include <stdint.h>

int main() {
	naiboard_init();

	naiboard_delay_ms(10);
   	printf_P(PSTR("\n\n\nnonoo's alarm interface v%d.%d\n" \
   		"built: " __TIME__ " " __DATE__ "\nusb vid=%.4x pid=%.4x sn=" USB_DEVICE_SERIAL_NAME "\n"), USB_DEVICE_MAJOR_VERSION, USB_DEVICE_MINOR_VERSION, USB_DEVICE_VENDOR_ID, USB_DEVICE_PRODUCT_ID);
	nai_console_printvcc();
	nai_init();
	printf_P(PSTR("ready.\n"));

	while (1) {
		WDT_Reset();
		naiboard_sleep();
		naiboard_process_stdin();
		nai_process();
	}

    return 0;
}
