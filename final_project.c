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

#include <avr/eeprom.h>
#include <stdio.h>

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
	short remote_result;
	unsigned char remote_buffer_count;

	// Initialize rotary encoder & interupts
	encoder_init();
	encoder_enable_interupts();
	short local_distance_count = eeprom_read_word((void *) FINAL_PROJECT_LOCAL_DISTANCE_STORAGE);
	short remote_distance_count = eeprom_read_word((void *) FINAL_PROJECT_REMOTE_DISTANCE_STORAGE);

	// Initialize button input & interupts
	button_init();
	button_enable_interupts();

	// Initialize rangefinder and echo interupts
	rangefinder_init();
	rangefinder_enable_interupts();
	short local_result;

	// Enables interupts globally
	sei();

	// Shows the splash screen
	lcd_splash_screen();

	while (1) {             // Loop forever
		if (!final_project_current_state) {
			if (final_project_FLAG_update_state) {
				if (final_project_next_state) {
					lcd_state_update(1, remote_distance_count);
				}
				final_project_current_state = final_project_next_state;
				final_project_FLAG_update_state = 0;
			}
			if (final_project_FLAG_update_count) {
				local_distance_count += encoder_count;
				if (local_distance_count > 400) {
					local_distance_count = 0;
				}
				if (local_distance_count < 0) {
					local_distance_count = 400;
				}
				encoder_count = 0;
				lcd_count_update(local_distance_count);
				eeprom_update_word((void *) FINAL_PROJECT_LOCAL_DISTANCE_STORAGE , local_distance_count);
				final_project_FLAG_update_count = 0;
			}
		}
		else {
			if (final_project_FLAG_update_state) {
				if (!final_project_next_state) {
					lcd_state_update(0, local_distance_count);
				}
				final_project_current_state = final_project_next_state;
				final_project_FLAG_update_state = 0;
			}
			if (final_project_FLAG_update_count) {
				remote_distance_count += encoder_count;
				if (remote_distance_count > 400) {
					remote_distance_count = 0;
				}
				if (remote_distance_count < 0) {
					remote_distance_count = 400;
				}
				encoder_count = 0;
				lcd_count_update(remote_distance_count);
				eeprom_update_word((void *) FINAL_PROJECT_REMOTE_DISTANCE_STORAGE , remote_distance_count);
				final_project_FLAG_update_count = 0;
			}
		}
		if (final_project_FLAG_fire_rangefinder) {
			rangefinder_trigger();
			final_project_FLAG_fire_rangefinder = 0;
		}
		if (rangefinder_FLAG_needs_calculation) {
			led_disable(LED_RED);
			led_disable(LED_GREEN);

			local_result = rangefinder_calculate_distance_milimeters();

			lcd_local_distance_update(local_result);
			
			if (local_result < local_distance_count) {
				led_enable(LED_RED);
			}
			else {
				led_enable(LED_GREEN);
			}

			serial_transmit(local_result);
			
			rangefinder_FLAG_needs_calculation = 0;
		}
		if (serial_FLAG_incoming_message_complete) {
			sscanf(serial_incoming_buffer, "%hd", &remote_result);
			remote_buffer_count = serial_incoming_buffer_count;

			lcd_remote_distance_update(remote_result, remote_buffer_count);

			if (remote_result < remote_distance_count) {
				speaker_enable();
			}

			serial_FLAG_incoming_message_complete = 0;
		}
	}
}		