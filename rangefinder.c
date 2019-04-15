/*
	rangefinder.c - Routines for controling the rangefinder.
*/

// Include necessary library + header files
#include "rangefinder.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

// Declare globals used in rangefinder ISRs
volatile unsigned char rangefinder_FLAG_timer_enabled = 0;
volatile unsigned char rangefinder_FLAG_needs_calculation = 0;

/*
	rangefinder_init() - Sets the appropriate pin to output, enables the relevant 
	interrupts, and sets the parameters for the timer
*/
void rangefinder_init(void)
{
	DDRB |= (1 << PB4);

	//Timer
	TCCR1B |= (1 << WGM12);
	OCR1A = RANGEFINDER_TIMER_MAX;

	TIMSK1 |= (1 << OCIE1A);

	PCICR |= (1 << PCIE0);
	PCMSK0 |= (1 << PCINT3);
}

/*
	rangefinder_trigger() - Triggers the rangefinder
*/
void rangefinder_trigger(void)
{
	PORTB |= (1 << PB4);
	_delay_us(RANGEFINDER_TRIGGER_DELAY);
	PORTB &= ~(1 << PB4);
}

/*
	rangefinder_calculate_distance_millimeters() - Calculates the distance in millimeters 
	based on the time length of the received echo. Uses only integer calculations
*/
short rangefinder_calculate_distance_millimeters(void)
{
	unsigned long long int last_timer = TCNT1;
	unsigned long long int temp = (last_timer << RANGEFINDER_SHIFT_AMOUNT);
	temp *= RANGEFINDER_PRESCALAR;
	temp /= RANGEFINDER_CLOCK;
	temp *= 1000000; // us in 1s
	temp /= 58; // Rangefinder constant for us --> cm
	temp *= 10; // mm in 1cm
	short millimeters = (temp >> RANGEFINDER_SHIFT_AMOUNT);
	if ((temp & RANGEFINDER_SHIFT_MASK) >= 32768) {
		millimeters++;
	}
	return millimeters;
}

/*
	Runs when the echo becomes high or low on the relevant pin.
	Sets a flag to tell main that the echo has been received and needs to be
	interpreted
*/
ISR(PCINT0_vect)
{
	if (!rangefinder_FLAG_timer_enabled) {
		//START TIMER
		TCNT1 = 0;
		TCCR1B |= (1 << CS11);
		rangefinder_FLAG_timer_enabled = 1;
	}
	else if (rangefinder_FLAG_timer_enabled) {
		//END TIMER
		TCCR1B &= ~(1 << CS11);
		rangefinder_FLAG_timer_enabled = 0;
		rangefinder_FLAG_needs_calculation = 1;
	}
}

/*
	Runs if the timer reaches the equivalent of a 400cm measurement. Prevents
	the echo from running forever due to the echo input pin being triggered by noise
	or other interference
*/
ISR(TIMER1_COMPA_vect)
{
	TCCR1B &= ~(1 << CS11);
	rangefinder_FLAG_timer_enabled = 0;
}