/*
 * t11_main.c
 *
 * T11 - Suspicious Package Training Device
 * Edward Sayers, Seth Ward, Devin Lorenzen, Jeremiah Franke
 * 
 */ 

/*
 * Pin Definitions
 * PC0 - Vibration Sensor 1 - PCINT8
 * PC1 - Vibration Sensor 2 - PCINT9
 * PC5 - Activation Switch  - PCINT13
 * PD0 - LED 1
 * PD1 - LED 2
 * PD2 - LED 3
 * PD4 - Buzzer
 * PC2 - RF Detector Enable
 * PC3 - RF Detector Output	- ADC3
 */

#include "t11_main.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#define THRESHOLD 0x0

int main(void)
{
	
	int i; // Counter index
	
	/* Set pin directions */
	clearbit(DDRC, PINC0);		// Vibration Sensor 1 as input
	clearbit(DDRC, PINC1);		// Vibration Sensor 2 as input
	clearbit(DDRC, PINC5);		// Activation Switch as input
	setbit(PORTC, PINC0);		// Enable pull-up for Vibration Sensor 1
	setbit(PORTC, PINC1);		// Enable pull-up for Vibration Sensor 2
	setbit(PORTC, PINC5);		// Enable pull-up for Activation Switch
	setbit(PORTD, PIND0);		// Turn LED 1 off
	setbit(PORTD, PIND1);		// Turn LED 2 off
	setbit(PORTD, PIND2);		// Turn LED 3 off
	setbit(DDRD, PIND0);		// LED 1 as output
	setbit(DDRD, PIND1);		// LED 2 as output
	setbit(DDRD, PIND2);		// LED 3 as output

	setbit(DDRC, PINC2);		// RF Detector Enable as output
	

	
	/* Setup ADC */
	
	/* Prescaler must be set so that ADC has a clock frequency between 50 kHz and 200 kHz
	 * Clock speed is 1MHz so a prescaler of 64 provides an appropriate frequency
	 * Setting ADPS2...0 to 110 sets the prescaler to 64 
	 */
	setbit(ADCSRA, ADPS2);
	setbit(ADCSRA, ADPS1);
	clearbit(ADCSRA, ADPS0);
	
	clearbit(ADMUX, ADLAR); // Right align ADC register
	
	/* Select ADC3 by setting MUX3...0 to 0011 in ADMUX */
	clearbit(ADMUX, MUX3);
	clearbit(ADMUX, MUX2);
	setbit(ADMUX, MUX1);
	setbit(ADMUX, MUX0);
	
	/* Select external voltage reference by setting REFS1...0 to 00 in ADMUX */
	clearbit(ADMUX, REFS1);
	clearbit(ADMUX, REFS0);
	
	setbit(ADCSRA, ADIE); // Enable ADC interrupts
	setbit(ADCSRA, ADEN); // Enable ADC
	
	for (i = 0; i < 15; ++i)
	{
		_delay_ms(500);
		togglebit(PORTD, PIND0);
	}
	/* Setup Interrupts */
	setbit(PCICR, PCIE1);		// Enable interrupts from PCINT[14:8]
	setbit(PCMSK1, PCINT8);		// Enable interrupt from vibration sensor 1
	setbit(PCMSK1, PCINT9);		// Enable interrupt from vibration sensor 2
	setbit(PCMSK1, PCINT13);	// Enable interrupt from activation switch
	sei(); // Enable interrupts
	
	/* Loop while waiting for interrupts */
    while(1)
    {
    }
}

/* Interrupt handler for ADC interrupts */
ISR(ADC_vect)
{
	uint8_t low_bits = ADCL;					// Get lower 8 bits
	uint16_t level = (ADCH << 8) | low_bits;	// Add upper 2 bits to lower 8 bits
	
	if (level > THRESHOLD)
	{
		clearbit(PORTD, PIND2);
	}
	else
	{
		setbit(ADCSRA, ADSC); // Start another conversion
	}
	
		
}

/* Interrupt handler for PCINT1 interrupts */
ISR(PCINT1_vect)
{
	clearbit(PORTD, PIND1);
}
