/*
	speaker.c - Routines for outputting sound via the speaker.
*/

/*
    speaker_play_note(freq) - Plays a tone at the frequency specified for one second
*/
void speaker_play_note(unsigned short freq)
{
	unsigned long period;

	// Period of note in microseconds
	period = 1000000 / freq;

}