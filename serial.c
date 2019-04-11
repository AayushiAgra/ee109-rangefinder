/*
	serial.c - Routines for sending data using serial communications.
*/

#include "serial.h"		// Declarations of serial comm functions

#include <avr/io.h>
#include <avr/interrupt.h>

/*
	Not declared in serial.h because it should only be used by
	functions in this file.
*/
void serial_txchar(char);

/*
	serial_init(ubrr_value) - Initializes the serial communications
	for the given ubrr_value.
*/
void serial_init(unsigned short ubrr_value)
{
	// Sets tri-state 
	DDRD |= (1 << PC3);
	PORTD &= ~(1 << PC3);

	// Set up USART0 registers
	UBRR0 = ubrr_value;			// Set baud rate
	UCSR0C = (3 << UCSZ00);			// Async., no parity, 1 stop bit, 8 data bits
	UCSR0B |= (1 << TXEN0 | 1 << RXEN0);	// Enable RX and TX
}

/*
	serial_enable_interupts() - Enable interupts for serial comms
*/
void serial_enable_interupts(void)
{
	UCSR0B |= (1 << RXCIE0); // Enable receiver interrupts
}

/*
	serial_txchar(ch) - Waits until the transmitter is ready and then
	transmits the char ch.
*/
void serial_txchar(char ch)
{
	// Wait for transmitter data register empty
	while ((UCSR0A & (1 << UDRE0)) == 0) {}
	UDR0 = ch;
}

/*
	serial_stringout(s) - Transmits a message by repeatedly calling the
	serial_txchar function above.
*/
void serial_stringout(char *s)
{
	// Call serial_txchar in loop to send a string
	int i;
	for (i = 0; i < 16; i++) {
		serial_txchar(s[i]);
	}
}

/*
	Runs when a character is received. Adds the characters to an array until
	all 16 have been received. main() then resets the count and displays the
	message.
*/
ISR(USART_RX_vect)
{
	char ch = UDR0;

}