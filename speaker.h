/*
	speaker.h - Header file to declare functions and macros used in speaker output
*/

void speaker_init(void);
void speaker_enable(void);

extern volatile unsigned char speaker_state;
extern volatile int speaker_timer_cycles_enabled;