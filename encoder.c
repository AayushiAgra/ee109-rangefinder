/*
  encoder.c - Routines for using/interpreting rotary encoder
*/

#include "encoder.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "final_project.h"

volatile unsigned char encoder_current_state = 0;
volatile unsigned char encoder_next_state = 0;
volatile short encoder_count = 0;

void encoder_init(void)
{
	PORTC |= ((1 << PC5) | (1 << PC4));

	unsigned char a, b;
	encoder_determine_a_b(&a, &b);
	if (!a && !b)
		encoder_current_state = 0;
	else if (a && !b)
		encoder_current_state = 1;
	else if (a && b)
		encoder_current_state = 2;
	else
		encoder_current_state = 3;
	encoder_next_state = encoder_current_state;
}

/*
	encoder_enable_interupts() - Enables PIN change interupts for
	the encoder.
*/
void encoder_enable_interupts(void)
{
	PCICR |= (1 << PCIE1);
	PCMSK1 |= ((1 << PCINT13) | (1 << PCINT12));
}

void encoder_determine_a_b(unsigned char *a, unsigned char *b)
{
	unsigned char read_in = PINC;
	*a = (read_in & (1 << PC4));
	*b = (read_in & (1 << PC5));
}

ISR(PCINT1_vect)
{
	unsigned char a, b;

	encoder_determine_a_b(&a, &b);

	if (encoder_current_state == 0) {
		// Handle A and B inputs for state 0
		if (a) {
			encoder_next_state = 1;
			encoder_count++;
		}
		else if (b) {
			encoder_next_state = 3;
			encoder_count--;
		}
	}
	else if (encoder_current_state == 1) {
		// Handle A and B inputs for state 1
		if (!a) {
			encoder_next_state = 0;
			encoder_count--;
		}
		else if (b) {
			encoder_next_state = 2;
			encoder_count++;
		}
	}
	else if (encoder_current_state == 2) {
		// Handle A and B inputs for state 2
		if (!a) {
			encoder_next_state = 3;
			encoder_count++;
		}
		else if (!b) {
			encoder_next_state = 1;
			encoder_count--;
		}
	}
	else {   // encoder_current_state = 3
		// Handle A and B inputs for state 3
		if (a) {
			encoder_next_state = 2;
			encoder_count--;    
		}
		else if (!b) {
			encoder_next_state = 0;
			encoder_count++;
		}
	}

	if (encoder_next_state != encoder_current_state){
		encoder_current_state = encoder_next_state;
		final_project_FLAG_update_count = 1;
	}
}