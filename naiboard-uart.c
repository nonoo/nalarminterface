#include "boardconfig.h"

#include "naiboard-uart.h"
#include "nai.h"

#include <avr/usart_driver.h>
#include <avr/port_driver.h>
#include <avr/sysclk.h>
#include <stdio.h>

static int naiboard_uart_putchar(char c, FILE *stream);
static int naiboard_uart_getchar(FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(naiboard_uart_putchar, naiboard_uart_getchar, _FDEV_SETUP_RW);

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

flag_t naiboard_stdin_data_waiting(void) {
	return USART_IsRXComplete(&USARTMODULE);
}

void naiboard_process_stdin(void) {
	char stdin_in;
	static char stdin_buffer[STDINBUFFERSIZE];
	static uint8_t stdin_bufferpos;

	while (naiboard_stdin_data_waiting()) {
		stdin_in = fgetc(stdin);

		if (stdin_in == '\n' || (stdin_in == '\r' && stdin_bufferpos == 0)) {
			printf_P(PSTR("\n"));
			continue;
		}

		if (stdin_bufferpos+1 == STDINBUFFERSIZE && stdin_in != '\b' && stdin_in != 0x7f && stdin_in != '\r')
			continue;

		if (stdin_in == '\b' || stdin_in == 0x7f) { // 0x7f - backspace
			printf_P(PSTR("%c %c"), stdin_in, stdin_in);
			if (stdin_bufferpos > 0)
				stdin_bufferpos--;
			continue;
		}

		printf_P(PSTR("%c"), stdin_in);

		if (stdin_in == '\r') {
			printf_P(PSTR("\n"));
			stdin_buffer[stdin_bufferpos] = 0;
			stdin_bufferpos = 0;
			nai_processconsolecommand(stdin_buffer);
		} else
			stdin_buffer[stdin_bufferpos++] = stdin_in;
	}
}

void naiboard_uart_init(void) {
	sysclk_enable_peripheral_clock(&USARTMODULE);
    stdout = &mystdout;
    stdin = &mystdout;

	PORT_SetPinsAsOutput(&USARTPORT, USARTTXPIN);
	PORT_ConfigurePins(&USARTPORT, USARTTXPIN, false, false, PORT_OPC_WIREDANDPULL_gc, PORT_ISC_INPUT_DISABLE_gc);
	PORT_SetPinsAsInput(&USARTPORT, USARTRXPIN);
	PORT_ConfigurePins(&USARTPORT, USARTRXPIN, false, false, PORT_OPC_PULLDOWN_gc, PORT_ISC_FALLING_gc);
	USART_Format_Set(&USARTMODULE, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, false);
	USART_Baudrate_Set(&USARTMODULE, 1603, -6); // 48MHz, 115200bps
	USART_Rx_Enable(&USARTMODULE);
	USART_Tx_Enable(&USARTMODULE);
}
