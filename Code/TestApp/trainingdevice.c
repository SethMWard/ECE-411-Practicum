/*
 * trainingdevice.c
 *
 * Created: 10/21/2014 1:25:33 PM
 *  Author: T11
 * 
 */ 
#include "trainingdevice.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>


volatile uint8_t activated;
volatile uint8_t armed;

int main(void)
{
	setbit(DDRB, PINB0);	// Set pin B0 to an output
	setbit(DDRD, PIND7);	// Set pin D7 to an output
	clearbit(DDRB, PINB1);	// Set pin B1 to an input
	setbit(PORTB, PINB1);	// Pull up pin B1
	setbit(PCICR, PCIE0);	// Enable interrupts for PCIE0
	setbit(PCMSK0, PCINT1);	// Enable interrupt from PCINT1
	setbit(ADCSRA, ADPS2);	// Set prescaler to 64 (1)
	setbit(ADCSRA, ADPS1);	// Set prescaler to 64 (2)
	clearbit(ADMUX, ADLAR);	// Right align ADC register
	setbit(ADMUX, REFS0);	// Set external ADC voltage reference
	setbit(ADCSRA, ADIE);	// Enable ADC interrupts
	setbit(ADCSRA, ADEN);	// Enable ADC
	sei();					// Enable interrupts
	
	
    while(1)
    {
	
    }
}

ISR(PCINT0_vect)
{
	if (!bit_is_clear(PINB, PINB1))
	{
		
		if (!armed)
			armDevice();
		else
		{
			clearbit(PORTB, PINB0);
			clearbit(PORTD, PIND7);
			activated = 0;
			armed = 0;
			_delay_ms(50);
		}
	}
}

ISR(ADC_vect)
{	
	uint8_t low_bits = ADCL;					// Low end bits
	uint16_t level = (ADCH << 8) | low_bits;	// Put registers together
	
	if(level > 0x200)
	{
		setbit(PORTD, PIND7); //Turn on light
		activated = 1;
	}
	
	if (armed)
		setbit(ADCSRA, ADSC); // Start next conversion
	
}

void armDevice(void)
{
	int i;
	clearbit(PORTB, PINB0);
	for (i = 0; i < 15; i++)
	{
		_delay_ms(500);
		togglebit(PORTB, PINB0);
	}
	armed = 1;
	setbit(ADCSRA, ADSC);	// Start first conversion
}