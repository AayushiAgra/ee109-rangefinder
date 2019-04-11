/*
	button.c - Routines for usage of buttons as input.
*/

#include "button.h"

#include <avr/interrupt.h>

#include "final_project.h"

void button_init(void)
{
	PORTD |= ((1 << PD3) | (1 << PD2));
}

void button_enable_interupts(void)
{
	PCICR |= (1 << PCIE2);
	PCMSK2 |= ((1 << PCINT19) | (1 << PCINT18));
}

char button_get_value(char pin)
{
	if ((PIND & (1 << pin)) != 0) {
		return 1;
	}
	else {
		return 0;
	}
}

ISR(PCINT2_vect)
{
	if (button_get_value(PD3) == 0) {
		final_project_FLAG_fire_rangefinder = 1;
	}
	else if (button_get_value(PD2) == 0) {
		if (final_project_current_state) {
			final_project_next_state = 0;
		}
		else {
			final_project_next_state = 1;
		}
		final_project_FLAG_update_state = 1;
	}
}