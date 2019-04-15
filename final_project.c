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

volatile unsigned char final_project_FLAG_update_state = 1; // Flag if state has changed
volatile unsigned char final_project_FLAG_fire_rangefinder = 0; // Flag to fire rangefinder
volatile unsigned char final_project_FLAG_update_count = 1; // Flag if count needs updating

int main(void) {

	// Initialize LCD
	lcd_init();

	// Initialize LED output
	led_init();

	// Initialize serial communications & declare variables used
	serial_init(SERIAL_MYUBRR);
	short remote_result;
	unsigned char remote_buffer_count;

	// Initialize rotary encoder & retrieve stored count values
	encoder_init();
	short local_distance_count = eeprom_read_word((void *) FINAL_PROJECT_LOCAL_DISTANCE_STORAGE);
	short remote_distance_count = eeprom_read_word((void *) FINAL_PROJECT_REMOTE_DISTANCE_STORAGE);

	// Initialize button input & set the inital state
	button_init();
	unsigned char current_state = 1;

	// Initialize rangefinder and declare variables used
	rangefinder_init();
	short local_result;

	// Initialize speaker
	speaker_init();

	// Enables interupts globally
	sei();

	// Shows the splash screen
	lcd_splash_screen();

	while (1) {
		/*
			If the update count flag is on, update the current state's count 
			by the quantity contained in encoder_count, update the LCD, 
			and update the flash memory 
		*/
		if (final_project_FLAG_update_count) {
			if (!current_state) {
				local_distance_count += encoder_count;
				if (local_distance_count > 400) {
					local_distance_count = 1;
				}
				if (local_distance_count < 1) {
					local_distance_count = 400;
				}
				encoder_count = 0;
				lcd_count_update(local_distance_count);
				eeprom_update_word((void *) FINAL_PROJECT_LOCAL_DISTANCE_STORAGE , local_distance_count);
			}
			else {
				remote_distance_count += encoder_count;
				if (remote_distance_count > 400) {
					remote_distance_count = 1;
				}
				if (remote_distance_count < 1) {
					remote_distance_count = 400;
				}
				encoder_count = 0;
				lcd_count_update(remote_distance_count);
				eeprom_update_word((void *) FINAL_PROJECT_REMOTE_DISTANCE_STORAGE , remote_distance_count);
			}
			final_project_FLAG_update_count = 0;
		}
		/*
			If the update state flag is on, change the current state and update the LCD
		*/
		if (final_project_FLAG_update_state) {
			if (current_state) {
				lcd_state_update(0, local_distance_count);
				current_state = 0;
			}
			else {
				lcd_state_update(1, remote_distance_count);
				current_state = 1;
			}
			final_project_FLAG_update_state = 0;
		}
		/*
			If the fire rangefinder flag is on, trigger the rangefinder
		*/
		if (final_project_FLAG_fire_rangefinder) {
			rangefinder_trigger();
			final_project_FLAG_fire_rangefinder = 0;
		}
		/*
			If the needs calculation flag is on, calculate the distance in millimeters,
			update the LCD, enable the appropriate LED, and transmit the result
		*/
		if (rangefinder_FLAG_needs_calculation) {
			led_disable(LED_RED);
			led_disable(LED_GREEN);

			local_result = rangefinder_calculate_distance_millimeters();

			lcd_local_distance_update(local_result);
			
			if ((local_result / 10) < local_distance_count) {
				led_enable(LED_RED);
			}
			else {
				led_enable(LED_GREEN);
			}

			serial_transmit(local_result);
			
			rangefinder_FLAG_needs_calculation = 0;
		}
		/*
			If the incoming message is complete, update the LCD and decide
			whether to enable the speaker
		*/
		if (serial_FLAG_incoming_message_complete) {
			sscanf(serial_incoming_buffer, "%hd", &remote_result);
			remote_buffer_count = serial_incoming_buffer_count;

			lcd_remote_distance_update(&remote_result, remote_buffer_count);

			if ((remote_result / 10) < remote_distance_count) {
				speaker_enable();
			}

			serial_FLAG_incoming_message_complete = 0;
		}
	}
}		