/*
	encoder.h - Header file to declare functions and globals used in rotary encoder.
*/

void encoder_init(void);

extern volatile unsigned char encoder_current_state;
extern volatile unsigned char encoder_next_state;
extern volatile short encoder_count;