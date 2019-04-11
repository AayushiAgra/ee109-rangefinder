/*
	led.c - Contains routines used in the control of LEDs
*/

#include "led.h"

#include <avr/io.h>

void led_init(void)
{
	DDRB |= (1 << PB5); // Green
	DDRC |= (1 << PC1); // Red
}

void led_enable(char color)
{
	if (color == LED_GREEN) {
		PORTB |= (1 << PB5);
	}
	else if (color == LED_RED) {
		PORTC |= (1 << PC1);
	}
}

void led_disable(char color)
{
	if (color == LED_GREEN) {
		PORTB &= ~(1 << PB5);
	}
	else if (color == LED_RED) {
		PORTC &= ~(1 << PC1);
	}
}