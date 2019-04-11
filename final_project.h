/*
	final_project.h - Header file to declare functions, macros, and globals
	used in the final project.
*/

#define FINAL_PROJECT_LOCAL_DISTANCE_STORAGE 100
#define FINAL_PROJECT_REMOTE_DISTANCE_STORAGE 200

extern volatile unsigned char final_project_current_state; // State 0: Local
extern volatile unsigned char final_project_next_state; // State 1: Remote
extern volatile unsigned char final_project_FLAG_update_state;

extern volatile unsigned char final_project_FLAG_fire_rangefinder;

extern volatile unsigned char final_project_FLAG_update_count;