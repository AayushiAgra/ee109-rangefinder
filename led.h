/*
	led.h - Header file to declare functions and macros used in LED usage
*/

#define LED_GREEN 0
#define LED_RED 1

void led_init(void);
void led_enable(char);
void led_disable(char);