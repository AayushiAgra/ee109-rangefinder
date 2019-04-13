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
	unsigned char remote_result_hundreds, remote_result_tens, remote_result_ones, remote_result_tenths;

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
	short local_result;

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
			led_disable(LED_RED);
			led_disable(LED_GREEN);

			local_result = rangefinder_calculate_distance_milimeters();

			lcd_local_distance_update(local_result);
			
			if (local_result < final_project_local_distance_count) {
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

			if (serial_incoming_buffer_count == 4) {
				remote_result_hundreds = remote_result / 1000;
				remote_result_tens = (remote_result % 1000) / 100;
				remote_result_ones = (remote_result % 100) / 10;
				remote_result_tenths = remote_result % 10;
			}
			else if (serial_incoming_buffer_count == 3) {
				remote_result_hundreds = 0;
				remote_result_tens = remote_result / 1000;
				remote_result_ones = (remote_result % 1000) / 100;
				remote_result_tenths = (remote_result % 100) / 10;
			}
			else if (serial_incoming_buffer_count == 2) {
				remote_result_hundreds = 0;
				remote_result_tens = 0;
				remote_result_ones = remote_result / 1000;
				remote_result_tenths = (remote_result % 1000) / 100;
			}
			else if (serial_incoming_buffer_count == 1) {
				remote_result_hundreds = 0;
				remote_result_tens = 0;
				remote_result_ones = 0;
				remote_result_tenths = remote_result / 1000;
			}

			lcd_remote_distance_update(remote_result_hundreds, remote_result_tens, remote_result_ones, remote_result_tenths);

			// TODO turn on speaker
			serial_FLAG_incoming_message_complete = 0;
		}
	}
}		