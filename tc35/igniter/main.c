// The TC35's IGN pin needs to be pulled low for at least 100ms after power is
// on. The module is only starting normal working mode when the IGN pin is
// released. The igniter circuit does this IGN pulldown periodically.

#define F_CPU 1000000UL

#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define IGNITEON() (PORTB |= 1 << PB3)
#define IGNITEOFF() (PORTB &= ~(1 << PB3))

int main() {
	DDRB |= (1 << PB3); // Pin 2 output

	_delay_ms(200);
	while (1) {
		IGNITEON();
		_delay_ms(200);
		IGNITEOFF();
		_delay_ms(1000);
	}

/*	while (1) {
		// Enabling sleep (this is just enabling, not switching to sleep state)
		sleep_enable();
		// Turning off all modules.
		PRR |= 0b1111;
		// Setting sleep mode to powerdown.
		MCUCR |= 0b1000;
		// Turning on BOD sleep.
		MCUCR |= 0b10000100;
		MCUCR |= 0b10000000;
		MCUCR &= ~0b00000100;
		// BOD sleep is active for only 3 clock cycles so we have to go to sleep now.
		sleep_cpu();
	}*/

	return 0;
}
