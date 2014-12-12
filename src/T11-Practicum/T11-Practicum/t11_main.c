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

//#define THRESHOLD 425
#define THRESHOLD 300
//#define SILENT

volatile uint16_t level;

int main(void)
{
	
	int i; // Counter index
	
	/* Set pin directions and initial values */
	clearbit(DDRC, PINC0);		// Vibration Sensor 1 as input
	clearbit(DDRC, PINC1);		// Vibration Sensor 2 as input
	clearbit(DDRC, PINC5);		// Activation Switch as input
	setbit(PORTC, PINC0);		// Enable pull-up for Vibration Sensor 1
	setbit(PORTC, PINC1);		// Enable pull-up for Vibration Sensor 2
	setbit(PORTD, PIND0);		// Turn LED 1 off
	setbit(PORTD, PIND1);		// Turn LED 2 off
	setbit(PORTD, PIND2);		// Turn LED 3 off
	setbit(DDRD, PIND0);		// LED 1 as output
	setbit(DDRD, PIND1);		// LED 2 as output
	setbit(DDRD, PIND2);		// LED 3 as output
	setbit(DDRD, PIND4);		// Buzzer as output
	setbit(DDRC, PINC2);		// RF Detector Enable as output
	setbit(PORTC, PINC2);		// Enable RF Detector
	

	
	/* Setup ADC */
	
	/* Prescaler determines the clock rate of the ADC, 50 - 200 kHz is recommended.
	 *
	 * Clock speed is 1MHz so a prescaler of 64 provides an appropriate frequency
	 * Setting ADPS2...0 to 110 sets the prescaler to 64 
	 */
	setbit(ADCSRA, ADPS2);
	setbit(ADCSRA, ADPS1);
	clearbit(ADCSRA, ADPS0);
	
	clearbit(ADMUX, ADLAR);		// Right align ADC register
	
	/* Select ADC3 by setting MUX3...0 to 0011 in ADMUX */
	clearbit(ADMUX, MUX3);
	clearbit(ADMUX, MUX2);
	setbit(ADMUX, MUX1);
	setbit(ADMUX, MUX0);
	
	/* Select external voltage reference by setting REFS1...0 to 00 in ADMUX */
	clearbit(ADMUX, REFS1);
	setbit(ADMUX, REFS0);
	
	setbit(ADCSRA, ADIE);		// Enable ADC interrupts
	setbit(ADCSRA, ADEN);		// Enable ADC
	
	/* Toggle Activation LED with half second delay
	 * Delay allows time for device to be placed
	 */
	for (i = 0; i < 15; ++i)
	{
		_delay_ms(500);
		togglebit(PORTD, PIND0);
	}
	/* Setup Interrupts */
	setbit(PCICR, PCIE1);		// Enable interrupts from PCINT[14:8]
	setbit(PCMSK1, PCINT8);		// Enable interrupt from vibration sensor 1
	setbit(PCMSK1, PCINT9);		// Enable interrupt from vibration sensor 2
	sei();						// Enable interrupts
	setbit(ADCSRA, ADSC);		// Start conversion
	
	/* Loop while waiting for interrupts */
    while(1)
    {
		// wait for interrupt
    }
}

/* Interrupt handler for ADC interrupts */
ISR(ADC_vect)
{
	uint8_t low_bits = ADCL;					// Get lower 8 bits
	level = (ADCH << 8) | low_bits;	// Add upper 2 bits to lower 8 bits
	
	if (level > THRESHOLD)
	{
		triggerDevice(0);
	}
	else
	{
		setbit(ADCSRA, ADSC); // Start another conversion
	}
	
		
}

/* Interrupt handler for PCINT1 interrupts */
ISR(PCINT1_vect)
{
	triggerDevice(1);
}

/* Triggers device
 * 0 = RF
 * 1 = Movement
 */
void triggerDevice(char source)
{
	cli();
	switch (source)
	{
	case 0:
		clearbit(PORTD, PIND2); // Turn on RF LED
		break;
	case 1:
	default:
		clearbit(PORTD, PIND1); // Turn on Movement LED
		break;		
	}
	soundBuzzer();	
}

// Turns buzzer on
void soundBuzzer(void)
{
#ifndef SILENT
		while (1)
		{
			togglebit(PORTD, PIND4);
			_delay_us(100);
		}
#endif
}


