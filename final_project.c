/********************************************
 *
 *  Name: Henry Kroeger
 *  Email: hkroeger@usc.edu
 *  Section: Wed 2pm
 *  Assignment: FINAL PROJECT
 *
 ********************************************/

// Necessary library and header files
#include "final_project.h"

#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "rangefinder.h"
#include "lcd.h"
#include "serial.h"
#include "encoder.h"
#include "button.h"
#include "speaker.h"
#include "led.h"

volatile unsigned char final_project_current_state = 1; // State 0: Local
volatile unsigned char final_project_next_state = 0; // State 1: Remote
volatile unsigned char final_project_FLAG_update_state = 1; // Flag if state has changed

volatile unsigned char final_project_FLAG_fire_rangefinder = 0; // Flag to fire rangefinder

volatile unsigned char final_project_FLAG_update_count = 1;

int main(void) {

	// Initialize LCD
	lcd_init();

	// Initialize LED output
	led_init();

	// Initialize serial communications & interupts
	serial_init(SERIAL_MYUBRR);
	serial_enable_interupts();

	// Initialize rotary encoder & interupts
	encoder_init();
	encoder_enable_interupts();
	short final_project_local_distance_count = eeprom_read_word((void *) FINAL_PROJECT_LOCAL_DISTANCE_STORAGE);
	short final_project_remote_distance_count = eeprom_read_word((void *) FINAL_PROJECT_REMOTE_DISTANCE_STORAGE);

	// Initialize button input & interupts
	button_init();
	button_enable_interupts();

	// Initialize rangefinder and echo interupts
	rangefinder_init();
	rangefinder_enable_interupts();

	// Enables interupts globally
	sei();

	// Shows the splash screen
	lcd_splash_screen();

	while (1) {             // Loop forever
		if (!final_project_current_state) {
			if (final_project_FLAG_update_state) {
				if (final_project_next_state) {
					lcd_state_update(1, final_project_remote_distance_count);
				}
				final_project_current_state = final_project_next_state;
				final_project_FLAG_update_state = 0;
			}
			if (final_project_FLAG_update_count) {
				final_project_local_distance_count += encoder_count;
				if (final_project_local_distance_count > 400) {
					final_project_local_distance_count = 0;
				}
				if (final_project_local_distance_count < 0) {
					final_project_local_distance_count = 400;
				}
				encoder_count = 0;
				lcd_count_update(final_project_local_distance_count);
				final_project_FLAG_update_count = 0;
			}
		}
		else {
			if (final_project_FLAG_update_state) {
				if (!final_project_next_state) {
					lcd_state_update(0, final_project_local_distance_count);
				}
				final_project_current_state = final_project_next_state;
				final_project_FLAG_update_state = 0;
			}
			if (final_project_FLAG_update_count) {
				final_project_remote_distance_count += encoder_count;
				if (final_project_remote_distance_count > 400) {
					final_project_remote_distance_count = 0;
				}
				if (final_project_remote_distance_count < 0) {
					final_project_remote_distance_count = 400;
				}
				encoder_count = 0;
				lcd_count_update(final_project_remote_distance_count);
				final_project_FLAG_update_count = 0;
			}
		}
		if (final_project_FLAG_fire_rangefinder) {
			rangefinder_trigger();
			final_project_FLAG_fire_rangefinder = 0;
		}
		if (rangefinder_FLAG_needs_calculation) {
			short result = rangefinder_calculate_distance_milimeters();
			//  TODO
			// lcd_stringout result
			if (result < final_project_local_distance_count) {
				// TODO turn on LED shorter OR turn on LED longer
			}
			if (result < final_project_remote_distance_count) {
				// TODO turn on speaker
			}
			// serial output to other arduino
			
			rangefinder_FLAG_needs_calculation = 0;
		}
	}
}		