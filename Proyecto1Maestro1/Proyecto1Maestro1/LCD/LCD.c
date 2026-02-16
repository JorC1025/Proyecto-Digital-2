/*
 * LCD.c
 *
 * Created: 25/1/2026 12:52:54
 *  Author: Jorge Cruz
 */ 

#include "LCD.h"

void initLCD8bits(void){

	DDRD |= (1 << DDD2) | (1 << DDD3) | (1<<DDD4) | (1<<DDD5) | (1<<DDD6) | (1<<DDD7);
	DDRB |= (1<<DDB0) | (1<<DDB1) | (1<<DDB2) | (1<<DDB3);
	
	LCD_Port(0x00);
	_delay_ms(20);
	LCD_CMD(0x03);
	_delay_ms(5);
	LCD_CMD(0x03);
	_delay_ms(11);
	LCD_CMD(0x03);
	LCD_CMD(0x02);
	
	//Function Set
	LCD_CMD(0x38);
	
	//Display On/Off
	LCD_CMD(0x00);
	LCD_CMD(0x0C);
	
	//Entry Mode
	LCD_CMD(0x00);
	LCD_CMD(0x06);
	
	//Clear Display
	LCD_CMD(0x00);
	LCD_CMD(0x01);
}

void LCD_CMD(char a){
	PORTD &= ~(1 << PORTD2); //RS
	LCD_Port(a);
	PORTD |= (1<<PORTD3); //EN
	_delay_ms(4);
	PORTD &= ~(1 << PORTD3);
	
	
}

void LCD_Port(char a){
	//D0 = 1;
	if (a & 1)
	{
		PORTD |= (1 << PORTD4);
		
	} 
	else
	{
		PORTD &= ~(1 << PORTD4);
	}
	
	//D1 = 1;
	if (a & 2)
	{
		PORTD |= (1 << PORTD5);
		
	}
	else
	{
		PORTD &= ~(1 << PORTD5);
	}
	//D2 = 1;
	if (a & 4)
	{
		PORTD |= (1 << PORTD6);
		
	}
	else
	{
		PORTD &= ~(1 << PORTD6);
	}
	
	//D3 = 1
		if (a & 8)
		{
			PORTD |= (1 << PORTD7);
			
		}
		else
		{
			PORTD &= ~(1 << PORTD7);
		}
		
		//D4 = 1
		if (a & 16)
		{
			PORTB |= (1 << PORTB0);
			
		}
		else
		{
			PORTB &= ~(1 << PORTB0);
		}
		
		//D5 = 1
		if (a & 32)
		{
			PORTB |= (1 << PORTB1);
			
		}
		else
		{
			PORTB &= ~(1 << PORTB1);
		}
		
		//D6 = 1
		if (a & 64)
		{
			PORTB |= (1 << PORTB2);
			
		}
		else
		{
			PORTB &= ~(1 << PORTB2);
		}		
		
		//D7 = 1
		if (a & 128)
		{
			PORTB |= (1 << PORTB3);
			
		}
		else
		{
			PORTB &= ~(1 << PORTB3);
		}		
}

void LCD_Write_Char(char c){
	
	PORTD |= (1 << PORTD2);
	LCD_Port(c);
	PORTD |= (1 << PORTD3);
	_delay_ms(4);
	PORTD &= ~(1 << PORTD3);

}

void LCD_Write_String(char *a){
	int i;
	for (i = 0;  a[i] != '\0'; i++ )
	LCD_Write_Char(a[i]);		
	
}

void LCD_Shift_Right(void){
	LCD_CMD(0x01);
	LCD_CMD(0x0C);
	
	
}

void LCD_Shift_Left(void){
	LCD_CMD(0x01);
	LCD_CMD(0x08);
		
}

void LCD_Set_Cursor(char c, char f){
	char temp;
	if (f == 1)
	{
		temp = 0x80 + c - 1;
		LCD_CMD(temp);
	} else if (f == 2){
		
		temp = 0xC0 + c - 1;
		LCD_CMD(temp);
		
	}
	
}