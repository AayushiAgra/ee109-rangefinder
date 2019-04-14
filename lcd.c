/*
	lcd.c - Routines for sending data and commands to the LCD shield
*/

#include "lcd.h"    // Declarations of the LCD functions

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

/*
	This function not declared in lcd.h since
	should only be used by the routines in this file.
*/
void lcd_writenibble(unsigned char);

/* Define a couple of masks for the bits in Port B and Port D */
#define DATA_BITS ((1 << PD7)|(1 << PD6)|(1 << PD5)|(1 << PD4))
#define CTRL_BITS ((1 << PB1)|(1 << PB0))

/*
	lcd_init - Do various things to initialize the LCD display
*/
void lcd_init(void)
{
	// Set the DDR register bits as inputs.
	DDRB |= CTRL_BITS;
	DDRD |= DATA_BITS;				    

	// Delay at least 15ms
	_delay_ms(15);

	lcd_writenibble(0b00110000);  // Use lcd_writenibble to send 0b0011
	_delay_ms(5);                 // Delay at least 4msec

	lcd_writenibble(0b00110000);  // Use lcd_writenibble to send 0b0011
	_delay_us(120);               // Delay at least 100usec

	lcd_writenibble(0b00110000);  // Use lcd_writenibble to send 0b0011, no delay needed

	lcd_writenibble(0b00100000);  // Use lcd_writenibble to send 0b0010
	_delay_ms(2);                 // Delay at least 2ms
    
	lcd_writecommand(0x28);       // Function Set: 4-bit interface, 2 lines

	lcd_writecommand(0x0f);       // Display and cursor on
}

/*
	lcd_moveto - Move the cursor to the row and column given by the arguments.
	Row is 0 or 1, column is 0 - 15.
*/
void lcd_moveto(unsigned char row, unsigned char col)
{
	unsigned char pos;
	if(row == 0) {
		pos = 0x80 + col;       // 1st row locations start at 0x80
	}
	else {
		pos = 0xc0 + col;       // 2nd row locations start at 0xc0
	}

	lcd_writecommand(pos);      // Send command
}

/*
	lcd_stringout - Print the contents of the character string "str"
	at the current cursor position.
*/
void lcd_stringout(char *str)
{
	int i = 0;
	while (str[i] != '\0') {     // Loop until next charater is NULL byte
		lcd_writedata(str[i]);  // Send the character
		i++;
	}
}

/*
	lcd_writecommand - Output a byte to the LCD command register.
*/
void lcd_writecommand(unsigned char cmd)
{
	PORTB &= ~(1 << PB0);       // Clear RS for command write
	lcd_writenibble(cmd);       // Send upper 4 bits
	lcd_writenibble(cmd << 4);  // Send lower 4 bits
	_delay_ms(2);               // Delay 2ms
}

/*
	lcd_writedata - Output a byte to the LCD data register
*/
void lcd_writedata(unsigned char dat)
{
	PORTB |= (1 << PB0);        // Set RS for data write
	lcd_writenibble(dat);       // Send upper 4 bits
	lcd_writenibble(dat << 4);  // Send lower 4 bits
	_delay_ms(2);               // Delay 2ms
}

/*
	lcd_writenibble - Output the UPPER four bits of "lcdbits" to the LCD
*/
void lcd_writenibble(unsigned char lcdbits)
{
	/* Load PORTD, bits 7-4 with bits 7-4 of "lcdbits" */
	PORTD &= ~DATA_BITS;
	PORTD |= (lcdbits & DATA_BITS);

	/* Make E signal (PB1) go to 1 and back to 0 */
	PORTB |= (1 << PB1);
	asm("nop"::);
	asm("nop"::);
	PORTB &= ~(1 << PB1);
}

/*
    lcd_splash_screen() - Displays the splashscreen for 1 second
*/
void lcd_splash_screen(void)
{
	lcd_writecommand(1); // Clear LCD
	lcd_moveto(0,0); // Move to first row, first column
	lcd_stringout("Henry J. Kroeger"); // Output name
	
	// Create string to output date
	char date[17];
	unsigned char month, day, year;
	month = 12;
	day = 15;
	year = 99;
	snprintf(date, 17, "%02d/%02d/%02d", month, day, year);
	
	lcd_moveto(1,3); // Move to second row, fourth column
	
	lcd_stringout(date); // Output date
	
	_delay_ms(1000); // Wait 1 second

	lcd_writecommand(1); // Clear LCD
}

void lcd_state_update(unsigned char state, short count)
{
	lcd_moveto(0,0);
	if (!state) {
		lcd_stringout("Local   ");
	}
	else {
		lcd_stringout("Remote  ");
	}
	lcd_count_update(count);
}

void lcd_count_update(short count)
{
	char count_display[9];
	snprintf(count_display, 9, "%3d", count);
	lcd_moveto(1,0);
	lcd_stringout(count_display);
}

void lcd_remote_distance_update(short distance, unsigned char buffer_count)
{
	unsigned char hundreds, tens, ones, tenths;

	if (buffer_count == 4) {
		hundreds = distance / 1000;
		tens = (distance % 1000) / 100;
		ones = (distance % 100) / 10;
		tenths = distance % 10;
	}
	else if (buffer_count == 3) {
		hundreds = 0;
		tens = distance / 1000;
		ones = (distance % 1000) / 100;
		tenths = (distance % 100) / 10;
	}
	else if (buffer_count == 2) {
		hundreds = 0;
		tens = 0;
		ones = distance / 1000;
		tenths = (distance % 1000) / 100;
	}
	else if (buffer_count == 1) {
		hundreds = 0;
		tens = 0;
		ones = 0;
		tenths = distance / 1000;
	}

	char distance_display[9];
	snprintf(distance_display, 9, "%1d%1d%1d.%01.1d", hundreds, tens, ones, tenths);

	lcd_moveto(1,8);
	lcd_stringout(distance_display);
}

void lcd_local_distance_update(short distance)
{
	short integer_part = distance / 10;
	unsigned char decimal_part = (distance % 10);
	char distance_display[9];
	snprintf(distance_display, 9, "%3d.%01.1d", integer_part, decimal_part);
	lcd_moveto(0, 8);
	lcd_stringout(distance_display);
}