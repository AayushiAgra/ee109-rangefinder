/*
	rangefinder.h - Header file to declare functions and macros used in rangefinder usage
*/

#define RANGEFINDER_TIMER_MAX 46400
#define RANGEFINDER_PRESCALAR 8
#define RANGEFINDER_CLOCK 16000000
#define RANGEFINDER_SHIFT_AMOUNT 16
#define RANGEFINDER_SHIFT_MASK ((1UL << RANGEFINDER_SHIFT_AMOUNT) - 1)
#define RANGEFINDER_TRIGGER_DELAY 10

void rangefinder_init(void);
void rangefinder_enable_interupts(void);
void rangefinder_trigger(void);
short rangefinder_calculate_distance_millimeters(void);

extern volatile unsigned char rangefinder_FLAG_timer_enabled;
extern volatile unsigned char rangefinder_FLAG_needs_calculation;