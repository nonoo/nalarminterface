#include "boardconfig.h"

#include <avr/avr_compiler.h>

#include <avr/usart_driver.h>
#include <avr/sysclk.h>
#include <avr/rtc_driver.h>
#include <avr/wdt_driver.h>
#include <avr/port_driver.h>

#include "naiboard.h"
#include "nai.h"
#include "powersaving.h"

#include <stdio.h>

usb_buf_t naiboard_usb_sendbuf;
static usb_buf_t naiboard_usb_recvbuf;

static int naiboard_uart_putchar(char c, FILE *stream);
static int naiboard_uart_getchar(FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(naiboard_uart_putchar, naiboard_uart_getchar, _FDEV_SETUP_RW);

#define USB_BLINKRXTXLED() naiboard_state.usb_blinkrxtxledpulsecount = 5;
naiboard_state_t naiboard_state;

static int naiboard_uart_putchar(char c, FILE *stream) {
    if (c == '\n')
		naiboard_uart_putchar('\r', stream);
	while (!USART_IsTXDataRegisterEmpty(&USARTMODULE))
		;
	USART_PutChar(&USARTMODULE, c);
	return 0;
}

static int naiboard_uart_getchar(FILE *stream) {
	while (!USART_IsRXComplete(&USARTMODULE))
		;
	return USART_GetChar(&USARTMODULE);
}

void naiboard_uart_init(void) {
	sysclk_enable_peripheral_clock(&USARTMODULE);
    stdout = &mystdout;
    stdin = &mystdout;

	USARTPORT.DIRSET = USARTTXPIN;
	USARTPORT.DIRCLR = USARTRXPIN;
	USART_Format_Set(&USARTMODULE, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, false);
//	USART_Baudrate_Set(&USARTMODULE, 3317, -4); // 32MHz, 9600bps
//	USART_Baudrate_Set(&USARTMODULE, 1047, -6); // 32MHz, 115200bps
	USART_Baudrate_Set(&USARTMODULE, 1603, -6); // 48MHz, 115200bps
//	USART_Baudrate_Set(&USARTMODULE, 769, -6); // 24MHz, 115200bps
	USART_Rx_Enable(&USARTMODULE);
	USART_Tx_Enable(&USARTMODULE);
}

void naiboard_delay_ms(uint16_t ms) {
	uint16_t i;
	for (i = 0; i < ms; i++) {
		_delay_ms(1);
#ifdef ARCH_AVR
		WDT_Reset(); // Watchdog reset
#endif
	}
}

void naiboard_delay_us(uint16_t us) {
	uint16_t i;
	for (i = 0; i < us; i++) {
		_delay_us(1);
#ifdef ARCH_AVR
		WDT_Reset(); // Watchdog reset
#endif
	}
}

static void naiboard_led1_init(void) {
#ifdef LED1PORT
    LED1PORT.DIRSET = LED1PIN;
    LED1PORT.OUTCLR = LED1PIN;
#endif
}

void naiboard_led1_on(void) {
#ifdef LED1PORT
    LED1PORT.OUTSET = LED1PIN;
#endif
}

void naiboard_led1_off(void) {
#ifdef LED1PORT
    LED1PORT.OUTCLR = LED1PIN;
#endif
}

void naiboard_led_init(void) {
	naiboard_led1_init();
	naiboard_led1_off();
}

void naiboard_sleep(void) {
	if (naiboard_state.usb_vendor_enabled)
		return;
	SLEEP.CTRL = SLEEP_SMODE_IDLE_gc;

	sysclk_disable_peripheral_clock(&RTC);
#ifdef LOG
	//printf_P(PSTR("sleep\n"));
#endif

	WDT_Disable();
	sleep_enable();
	cpu_sleep();

	// Waking up
	sleep_disable();
	WDT_Enable();
	sysclk_enable_peripheral_clock(&RTC);
#ifdef LOG
	//printf_P(PSTR("wake\n"));
#endif
}

// period == 1024 -> 1 sec resolution
static void naiboard_rtc_init(void) {
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

    /* Enable interrupts. */
    PMIC.CTRL |= PMIC_LOLVLEN_bm;
    sei();
}

ISR(RTC_OVF_vect) {
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

// This function sends USB interrupt data
void naiboard_usb_int_send(uint8_t *data, uint16_t length) {
#ifdef LOG
	uint16_t i;

	if (length) { // If length == 0 then this function is just used for data receive acknowledgement
		printf_P(PSTR("sending %d bytes: "), length);
		for (i = 0; i < length; i++)
			printf_P(PSTR("%.2x"), data[i]);
		printf_P(PSTR("\n"));
	}
#endif

	udi_vendor_interrupt_in_run(data, length, naiboard_usb_vendor_int_in);
}

// This gets called when interrupt data is sent
void naiboard_usb_vendor_int_in(udd_ep_status_t status, iram_size_t nb_transfered, udd_ep_id_t ep) {
	USB_BLINKRXTXLED();
#ifdef LOG
	printf_P(PSTR("sending interrupt data: "));
#endif
	if (UDD_EP_TRANSFER_OK != status) {
#ifdef LOG
		printf_P(PSTR("aborted\n"));
#endif
		return;
	}

#ifdef LOG
	if (nb_transfered)
		printf_P(PSTR("sending %d bytes\n"), nb_transfered);
	else
		printf_P(PSTR("ok\n"));
#endif

	// Wait a full buffer
	udi_vendor_interrupt_out_run(naiboard_usb_recvbuf, USBBUFSIZE, naiboard_usb_vendor_int_out);
}

// This gets called when interrupt data is received
void naiboard_usb_vendor_int_out(udd_ep_status_t status, iram_size_t nb_transfered, udd_ep_id_t ep) {
	USB_BLINKRXTXLED();
#ifdef LOG
	uint16_t i;

	printf_P(PSTR("received interrupt data: "));
#endif
	if (UDD_EP_TRANSFER_OK != status) {
#ifdef LOG
		printf_P(PSTR("aborted\n"));
#endif
		return;
	}

	if (nb_transfered == 0) {
#ifdef LOG
		printf_P(PSTR("no data received\n"));
#endif
		return;
	}

#ifdef LOG
	printf_P(PSTR("%d bytes: "), nb_transfered);
	for (i = 0; i < nb_transfered; i++)
		printf_P(PSTR("%.2x"), naiboard_usb_recvbuf[i]);
	printf_P(PSTR("\n"));
#endif

	nai_usbpacket_received((nai_usbpacket_t *)naiboard_usb_recvbuf);
	// Initializing USB RX again
	naiboard_usb_init_int_recv();
}

void naiboard_usb_init_int_recv() {
	naiboard_usb_vendor_int_in(UDD_EP_TRANSFER_OK, 0, 0);
}

bool naiboard_usb_vendor_enable(void) {
	naiboard_state.usb_vendor_enabled = 1;
#ifdef LOG
	printf_P(PSTR("usb_vendor_enable();\n"));
#endif
	naiboard_usb_init_int_recv();
	return true;
}

void naiboard_usb_vendor_disable(void) {
	naiboard_state.usb_vendor_enabled = naiboard_state.usb_connected = 0;
#ifdef LOG
	printf_P(PSTR("usb_vendor_disable();\n"));
#endif
}

bool naiboard_usb_setup_out_received(void) {
#ifdef LOG
	printf_P(PSTR("usb_setup_out_received();\n"));
#endif
	return true;
}

bool naiboard_usb_setup_in_received(void) {
#ifdef LOG
	printf_P(PSTR("usb_setup_in_received();\n"));
#endif
	return true;
}

void naiboard_usb_vbus_action(bool b_high) {
#ifdef LOG
	printf_P(PSTR("usb_vbus_action(%d);\n"), b_high);
#endif

	if (b_high) {
		// Attach USB Device
		udc_attach();
	} else {
		// VBUS not present
		udc_detach();
	}
}

void naiboard_usb_suspend_action(void) {
#ifdef LOG
	printf_P(PSTR("usb_suspend_action();\n"));
#endif
}

void naiboard_usb_resume_action(void) {
#ifdef LOG
	printf_P(PSTR("usb_resume_action();\n"));
#endif
}

void naiboard_usb_sof_action(void) {
#ifdef LOG
//	printf_P(PSTR("usb_sof_action();\n"));
#endif
}

void naiboard_usb_init(void) {
	udc_start();

	if (!udc_include_vbus_monitoring()) {
		// VBUS monitoring is not available on this product
		// thereby VBUS has to be considered as present
		naiboard_usb_vbus_action(true);
	}
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

	naiboard_led1_on();
	naiboard_delay_ms(50);
	naiboard_led1_off();

	naiboard_rtc_init();
}
