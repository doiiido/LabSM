/*
 * LCD.h
 *
 *  Created on: 16 de nov de 2017
 *      Author: lincoln
 */
#ifndef LCD_H_
#define LCD_H_

void lcdBacklightON();

void lcdBacklightOFF();

void pulsa_e(void);

void Instruction_mode(void);

void Data_mode(void);

void lcd_pins(void);

void lcd_nib(char nibble);

void lcd_Byte(char byte);

void lcd_cmd(char byte);

void lcd_char(char byte);

void lcd_init(void);

void lcd_cursor(int c, int l);

void lcd_clr(void);

void lcd_home(void);

void lcd_str(char *pt);

#endif

