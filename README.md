# EE 109 FINAL PROJECT - RANGEFINDER

## ARDUINO I/O CONFIG
	IN		NAME	USAGE
	DO		PD0		RX
	D1		PD1		TX
	D2		PD2		Local/Remote Control Button Input
	D3		PD3		Rangefinder Enable Button Input

	D11		PB3		Echo Input
	D12		PB4		Trigger Output
	D13		PB5		Green LED Output

	A1		PC1		Red LED Output
	A2		PC2		Speaker Output
	A3		PC3		Tri-State Control Output
	A4		PC4		Rotary Encoder Input 1
	A5		PC5		Rotary Encoder Input 2

## STATES
- State 0: Local Threshold Distance Display
- State 1: Remote Threshold Distance Display

## EEPROM DATA LOCATIONS
- Local Threshold Distance: 100
- Remote Threshold Distance: 200

## NOTES
- main() exists in final_project.c
- Code for each component/module has its own .c file
   - i.e. led.c contains routines for controlling LEDs
- Project-wide globals are declared in final_project.h
- Individual module/component globals declared in relevant header files
- All globals/macros are named: location_detailedname
   - i.e. final_project_next_state or encoder_count
- All flags have _FLAG_ following their declared location in their name
