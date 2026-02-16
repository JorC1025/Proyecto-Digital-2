
/*
 * LCD.h
 *
 * Created: 25/1/2026 12:53:05
 *  Author: Jorge Cruz
 */ 


#ifndef LCD_H_
#define LCD_H_
#define F_CPU 16000000

#include <util/delay.h>
#include <avr/io.h>

void initLCD8bits(void);
void LCD_CMD(char a);
void LCD_Port(char a);
void LCD_Write_Char(char c);
void LCD_Write_String(char *a);
void LCD_Shift_Right(void);
void LCD_Shift_Left(void);
void LCD_Set_Cursor(char c, char f);

#endif /* LCD_H_ */
