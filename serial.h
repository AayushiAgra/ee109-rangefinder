/*
	serial.h - Header file to declare functions and macros used in serial comms
*/

#define SERIAL_FOSC 16000000           // Clock frequency
#define SERIAL_BAUD 9600               // Baud rate used
#define SERIAL_MYUBRR (SERIAL_FOSC/16/SERIAL_BAUD-1) // Value for UBRR0 register

void serial_init(unsigned short);
void serial_transmit(short);

extern volatile unsigned char serial_FLAG_incoming_message;
extern volatile unsigned char serial_FLAG_incoming_message_complete;
extern volatile unsigned char serial_incoming_buffer_count;
extern volatile char serial_incoming_buffer[5];