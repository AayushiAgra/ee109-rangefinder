/*
	encoder.h - Header file to declare functions and macros used in rotary encoder.
*/

void encoder_init(void);
void encoder_determine_a_b(unsigned char *, unsigned char *);

extern volatile unsigned char encoder_current_state;
extern volatile unsigned char encoder_next_state;
extern volatile short encoder_count;