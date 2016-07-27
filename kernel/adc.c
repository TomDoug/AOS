#include <avr/io.h>

/*
 * This module will be for accessing
 * the ADCs by the tasks
 *  
   */



//TODO:
//		-make this aware of which adcs have been allocated
//		-allow multiple different adcs to be allocated

void
adc_startup (void)
{
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	ADMUX |= (1 << REFS0);

	ADCSRA |= (1 << 5);

	ADMUX |= (1 << ADLAR);

	ADCSRA |= (1 << ADEN);

	ADCSRA |= (1 << ADSC);
}

uint8_t
adc_get (void)
{
	uint8_t value;
	value = ADCH;
	return value;
}