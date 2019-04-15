/*
	serial.c - Routines for sending data using serial communications.
*/

// Include necessary library + header files
#include "serial.h"	

#include <avr/io.h>
#include <avr/interrupt.h>

/*
	Not declared in serial.h because it should only be used by
	functions in this file.
*/
void serial_txchar(char);

// Declare globals used in serial comms ISRs
volatile unsigned char serial_FLAG_incoming_message = 0;
volatile unsigned char serial_FLAG_incoming_message_complete = 0;
volatile unsigned char serial_incoming_buffer_count = 0;
volatile char serial_incoming_buffer[5] = {'0','0','0','0','\0'};

/*
	serial_init(ubrr_value) - Initializes the serial communications
	for the given ubrr_value and enables interrupts
*/
void serial_init(unsigned short ubrr_value)
{
	// Sets tri-state 
	DDRC |= (1 << PC3);
	PORTC &= ~(1 << PC3);

	// Set up USART0 registers
	UBRR0 = ubrr_value;			// Set baud rate
	UCSR0C = (3 << UCSZ00);			// Async., no parity, 1 stop bit, 8 data bits
	UCSR0B |= ((1 << TXEN0) | (1 << RXEN0));	// Enable RX and TX
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
	serial_transmit(result) - Transmits the result of the rangefinder in millimeters 
	according to the convention established in the project handout
*/
void serial_transmit(short result)
{
	unsigned char digit;
	serial_txchar('@');

	if (result >= 1000) {
		digit = result / 1000;
		serial_txchar('0' + digit);
	}
	if (result >= 100) {
		digit = (result % 1000) / 100;
		serial_txchar('0' + digit);
	}
	if (result >= 10) {
		digit = (result % 100) / 10;
		serial_txchar('0' + digit);
	}
	if (result >= 0) {
		digit = result % 10;
		serial_txchar('0' + digit);
	}

	serial_txchar('$');
}

/*
	Runs when a character is received. Creates a buffer of numbers if the appropriate 
	symbols have been received according to the convention established in the project 
	handout. Sets a flag to display the message in main()
*/
ISR(USART_RX_vect)
{
	char ch = UDR0;

	if (ch == '@') {
		serial_FLAG_incoming_message = 1;
		serial_incoming_buffer_count = 0;
		int i;
		for (i = 0; i < 4; i++) {
			serial_incoming_buffer[i] = '0';
		}
	}
	else if (ch == '$' && serial_FLAG_incoming_message && serial_incoming_buffer_count > 0) {
		serial_FLAG_incoming_message = 0;
		serial_FLAG_incoming_message_complete = 1;
	}
	else if (ch == '$' && serial_FLAG_incoming_message) {
		serial_FLAG_incoming_message = 0;
		serial_incoming_buffer_count = 0;
		int i;
		for (i = 0; i < 4; i++) {
			serial_incoming_buffer[i] = '0';
		}
	}
	else if (serial_FLAG_incoming_message) {
		serial_incoming_buffer[serial_incoming_buffer_count] = ch;
		serial_incoming_buffer_count++;
	}
	else {
		serial_FLAG_incoming_message = 0;
		serial_incoming_buffer_count = 0;
		int i;
		for (i = 0; i < 4; i++) {
			serial_incoming_buffer[i] = 0;
		}
	}
}