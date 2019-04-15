/*
	led.c - Contains routines used in the control of LEDs
*/

// Include necessary library + header files
#include "led.h"

#include <avr/io.h>

/*
	led_init() - Sets the appropriate pins to output
*/
void led_init(void)
{
	DDRB |= (1 << PB5); // Green
	DDRC |= (1 << PC1); // Red
}

/*
	led_enable(color) - Enables the appropriate LED
*/
void led_enable(char color)
{
	if (color == LED_GREEN) {
		PORTB |= (1 << PB5);
	}
	else if (color == LED_RED) {
		PORTC |= (1 << PC1);
	}
}

/*
	led_disable(color) - Disables the appropriate LED
*/
void led_disable(char color)
{
	if (color == LED_GREEN) {
		PORTB &= ~(1 << PB5);
	}
	else if (color == LED_RED) {
		PORTC &= ~(1 << PC1);
	}
}