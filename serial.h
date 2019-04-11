/*
	serial.h - Header file to declare functions and macros used in serial comms
*/

#define SERIAL_FOSC 16000000           // Clock frequency
#define SERIAL_BAUD 9600               // Baud rate used
#define SERIAL_MYUBRR (SERIAL_FOSC/16/SERIAL_BAUD-1) // Value for UBRR0 register

void serial_init(unsigned short);
void serial_stringout(char *);
void serial_enable_interupts(void);