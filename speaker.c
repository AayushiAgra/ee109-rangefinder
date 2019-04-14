/*
	speaker.c - Routines for outputting sound via the speaker.
*/

void speaker_init(void)
{
	DDRC |= (1 << PC2);
	TCCR0A |= (1 << WGM01);
	OCR0A = 142;

	TIMSK0 |= (1 << OCIE0A);
}

void speaker_enable(void)
{
	TCCR0B |= (1 << CS02);
	speaker_timer_cycles_enabled = 0;
}

ISR(TIMER0_COMPA_vect)
{

	if (!(PORTB & (1 << PC2))) {
		PORTB |= (1 << PC2);
	}
	else {
		PORTB &= ~(1 << PC2);
	}

	speaker_timer_cycles_enabled++;

	if (speaker_timer_cycles_enabled == 2200) {
		TCCR0B &= ~(1 << CS02);
		TCNT0 = 0;
		PORTB &= ~(1 << PC2);
	}
}