#include "boardconfig.h"

#include "naiboard-adc.h"
#include "powersaving.h"

#include <avr/port_driver.h>

#include <stdint.h>

static uint16_t naiboard_adc_readucvcc(void) {
	POWERSAVING_ENABLE_ADC();

	ADCA.CTRLB = ADC_RESOLUTION_12BIT_gc | ADC_CONMODE_bm;
	ADCA.REFCTRL = ADC_REFSEL_INT1V_gc;
	ADCA.PRESCALER = ADC_PRESCALER_DIV512_gc; // Samplerate 115.2kHz (14745600/128)
	ADCA.CTRLA = ADC_ENABLE_bm | ADC_FLUSH_bm;

	ADCA.CH0.CTRL = ADC_CH_INPUTMODE_INTERNAL_gc;
	ADCA.CH0.INTCTRL = ADC_CH_INTMODE_COMPLETE_gc | ADC_CH_INTLVL_OFF_gc;
	ADCA.CH0.MUXCTRL = ADC_CH_MUXINT_SCALEDVCC_gc; // Vcc/10
	ADCA.CTRLA |= ADC_CH0START_bm;
	while (!(ADCA.INTFLAGS & ADC_CH0IF_bm))
		;
	ADCA.INTFLAGS = ADC_CH0IF_bm;
	uint16_t res = ADCA.CH0.RES;

	POWERSAVING_DISABLE_ADC();

	return res;
}

float naiboard_get_vcc(void) {
	return (float)naiboard_adc_readucvcc() * 10 / 2048;
}
