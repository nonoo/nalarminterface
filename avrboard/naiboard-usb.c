#include "boardconfig.h"

#include "naiboard-usb.h"
#include "nai.h"

#include <stdio.h>

#define USB_BLINKRXTXLED() naiboard_state.usb_blinkrxtxledpulsecount = 5

usb_buf_t naiboard_usb_sendbuf;
static usb_buf_t naiboard_usb_recvbuf;
extern volatile naiboard_state_t naiboard_state;

// This function sends USB interrupt data
void naiboard_usb_int_send(uint8_t *data, uint16_t length) {
	uint16_t i = 0;

	if (length) { // If length == 0 then this function is just used for data receive acknowledgement
		printf_P(PSTR("naiboard-usb: sending %d bytes: "), length);
		for (i = 0; i < length; i++)
			printf_P(PSTR("%.2x"), data[i]);
		printf_P(PSTR("\n"));
	}

	udi_vendor_interrupt_in_run(data, length, naiboard_usb_vendor_int_in);
}

// This gets called when interrupt data is sent
void naiboard_usb_vendor_int_in(udd_ep_status_t status, iram_size_t nb_transfered, udd_ep_id_t ep) {
	USB_BLINKRXTXLED();
	printf_P(PSTR("naiboard-usb: sending interrupt data: "));
	if (UDD_EP_TRANSFER_OK != status) {
		printf_P(PSTR("aborted\n"));
		return;
	}

	if (nb_transfered)
		printf_P(PSTR("%d bytes\n"), nb_transfered);
	else
		printf_P(PSTR("ok\n"));

	// Wait a full buffer
	udi_vendor_interrupt_out_run(naiboard_usb_recvbuf, USBBUFSIZE, naiboard_usb_vendor_int_out);
}

// This gets called when interrupt data is received
void naiboard_usb_vendor_int_out(udd_ep_status_t status, iram_size_t nb_transfered, udd_ep_id_t ep) {
	USB_BLINKRXTXLED();
	uint16_t i = 0;

	printf_P(PSTR("naiboard-usb: received interrupt data: "));
	if (UDD_EP_TRANSFER_OK != status) {
		printf_P(PSTR("aborted\n"));
		return;
	}

	if (nb_transfered == 0) {
		printf_P(PSTR("no data received\n"));
		return;
	}

	printf_P(PSTR("%d bytes: "), nb_transfered);
	for (i = 0; i < nb_transfered; i++)
		printf_P(PSTR("%.2x"), naiboard_usb_recvbuf[i]);
	printf_P(PSTR("\n"));

	nai_usbpacket_received((nai_usbpacket_t *)naiboard_usb_recvbuf);
	// Initializing USB RX again
	naiboard_usb_init_int_recv();
}

void naiboard_usb_init_int_recv() {
	naiboard_usb_vendor_int_in(UDD_EP_TRANSFER_OK, 0, 0);
}

bool naiboard_usb_vendor_enable(void) {
	naiboard_state.usb_vendor_enabled = 1;
	printf_P(PSTR("naiboard-usb: usb_vendor_enable();\n"));
	naiboard_usb_init_int_recv();
	return true;
}

void naiboard_usb_vendor_disable(void) {
	naiboard_state.usb_vendor_enabled = naiboard_state.usb_connected = 0;
	printf_P(PSTR("naiboard-usb: usb_vendor_disable();\n"));
}

bool naiboard_usb_setup_out_received(void) {
	printf_P(PSTR("naiboard-usb: usb_setup_out_received();\n"));
	return true;
}

bool naiboard_usb_setup_in_received(void) {
	printf_P(PSTR("naiboard-usb: usb_setup_in_received();\n"));
	return true;
}

void naiboard_usb_vbus_action(bool b_high) {
	printf_P(PSTR("naiboard-usb: usb_vbus_action(%d);\n"), b_high);

	if (b_high) {
		// Attach USB Device
		udc_attach();
	} else {
		// VBUS not present
		udc_detach();
	}
}

void naiboard_usb_suspend_action(void) {
	printf_P(PSTR("naiboard-usb: usb_suspend_action();\n"));
}

void naiboard_usb_resume_action(void) {
	printf_P(PSTR("naiboard-usb: usb_resume_action();\n"));
}

void naiboard_usb_sof_action(void) {
//	printf_P(PSTR("naiboard-usb: usb_sof_action();\n"));
}

void naiboard_usb_init(void) {
	udc_start();

	if (!udc_include_vbus_monitoring()) {
		// VBUS monitoring is not available on this product
		// thereby VBUS has to be considered as present
		naiboard_usb_vbus_action(true);
	}
}
