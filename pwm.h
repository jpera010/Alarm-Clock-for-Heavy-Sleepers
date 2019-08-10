#include <avr/io.h>
#include <avr/interrupt.h>
void set_PWM(double frequency) {
	static double current_frequency = -1; // Keeps track of the currently set frequency
	if (frequency != current_frequency) {
		if (!frequency) { TCCR0B &= 0x08; } //stops timer/counter
		else { TCCR0B |= 0x03; } // resumes/continues timer/counter			

		if (frequency < 0.954) { OCR0A = 0xFFFF; }
	
		// prevents OCR0A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow	
		else if (frequency > 31250) { OCR0A = 0x0000; }
			
		else { OCR0A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT0 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR0A = (1 << COM0A0);
		// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR0B = (1 << WGM02) | (1 << CS01) | (1 << CS00);	
		// WGM02: When counter (TCNT0) matches OCR0A, reset counter
		// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR0A = 0x00;
	TCCR0B = 0x00;
}