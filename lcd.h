/*
	lcd.h - Declares functions used in LCD control/output.
*/

void lcd_init(void);
void lcd_moveto(unsigned char, unsigned char);
void lcd_stringout(char *);
void lcd_writecommand(unsigned char);
void lcd_writedata(unsigned char);
void lcd_splash_screen(void);
void lcd_state_update(unsigned char, short);
void lcd_count_update(short);
void lcd_remote_distance_update(short*, unsigned char);
void lcd_local_distance_update(short);