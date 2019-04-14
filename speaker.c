/*
	speaker.c - Routines for outputting sound via the speaker.
*/

#include "speaker.h"

#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char speaker_state = 0;
volatile int speaker_timer_cycles_enabled = 0;

void speaker_init(void)
{
	DDRC |= (1 << PC2);
	TCCR0A |= (1 << WGM01);
	TIMSK0 |= (1 << OCIE0A);
	OCR0A = 71;
}

void speaker_enable(void)
{
	TCCR0B |= (1 << CS02);
	speaker_timer_cycles_enabled = 0;
}

ISR(TIMER0_COMPA_vect)
{

	if (!speaker_state) {
		PORTC |= (1 << PC2);
		speaker_state = 1;
	}
	else {
		PORTC &= ~(1 << PC2);
		speaker_state = 0;
	}

	speaker_timer_cycles_enabled++;

	if (speaker_timer_cycles_enabled == 2200) {
		TCCR0B &= ~(1 << CS02);
		TCNT0 = 0;
		PORTC &= ~(1 << PC2);
	}
}