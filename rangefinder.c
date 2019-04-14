/*
	rangefinder.c - Routines for controling the rangefinder.
*/

#include "rangefinder.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

volatile unsigned char rangefinder_FLAG_timer_enabled = 0;
volatile unsigned char rangefinder_FLAG_needs_calculation = 0;

void rangefinder_init(void)
{
	DDRB |= (1 << PB4);

	//Timer
	TCCR1B |= (1 << WGM12);
	OCR1A = RANGEFINDER_TIMER_MAX;

}

void rangefinder_enable_interupts(void)
{
	TIMSK1 |= (1 << OCIE1A);

	PCICR |= (1 << PCIE0);
	PCMSK0 |= (1 << PCINT3);
}

void rangefinder_trigger(void)
{
	PORTB |= (1 << PB4);
	_delay_us(RANGEFINDER_TRIGGER_DELAY);
	PORTB &= ~(1 << PB4);
}

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

ISR(TIMER1_COMPA_vect)
{
	TCCR1B &= ~(1 << CS11);
	rangefinder_FLAG_timer_enabled = 0;
}