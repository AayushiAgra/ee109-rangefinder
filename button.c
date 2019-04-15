/*
	button.c - Routines for usage of buttons as input.
*/

// Include necessary header + library files
#include "button.h"

#include <avr/interrupt.h>

#include "final_project.h"

// Declared here since only used in this file
char button_get_value(char);

/*
	button_init() - Enables pull-up resistors and interupts for the buttons
*/
void button_init(void)
{
	PORTD |= ((1 << PD3) | (1 << PD2));
	PCICR |= (1 << PCIE2);
	PCMSK2 |= ((1 << PCINT19) | (1 << PCINT18));
}

/*
	button_get_value(pin) - Determines whether a button connected to a pin has been pressed
*/
char button_get_value(char pin)
{
	if ((PIND & (1 << pin)) != 0) {
		return 1;
	}
	else {
		return 0;
	}
}

/*
	Runs when a button has been pressed. If the trigger button is pressed,
	the flag to fire the rangefinder is enabled. If the state change button is
	pressed, the flag to change state is enabled
*/
ISR(PCINT2_vect)
{
	if (button_get_value(PD3) == 0) {
		final_project_FLAG_fire_rangefinder = 1;
	}
	else if (button_get_value(PD2) == 0) {
		final_project_FLAG_update_state = 1;
	}
}