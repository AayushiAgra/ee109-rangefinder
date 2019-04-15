/*
	speaker.c - Routines for outputting sound via the speaker.
*/

// Include necessary library + header files
#include "speaker.h"

#include <avr/io.h>
#include <avr/interrupt.h>

// Declare globals used in ISR
volatile unsigned char speaker_state = 0;
volatile int speaker_timer_cycles_enabled = 0;

/*
	speaker_init(void) - Initializes the output and interrupts for the speaker
*/
void speaker_init(void)
{
	DDRC |= (1 << PC2);
	TCCR0A |= (1 << WGM01);
	TIMSK0 |= (1 << OCIE0A);
	OCR0A = 71;
}

/*
	speaker_enable() - Enables the speaker; see ISR below for additional detail
*/
void speaker_enable(void)
{
	TCCR0B |= (1 << CS02);
	speaker_timer_cycles_enabled = 0;
}

/*
	Runs every 1/880 of a second to create a square wave at approximately A4 (440 Hz).
	After approximately 2.5 seconds, automatically shuts itself off.
*/
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