#include "LCD.h"
#include "I2C.h"
#include <msp430.h>


#define RS  BIT0
#define RW  BIT1
#define EN  BIT2
#define BL  BIT3
#define D4  BIT4
#define D5  BIT5
#define D6  BIT6
#define D7  BIT7

/*
 * LCD.c
 *
 *  Created on: 16 de nov de 2017
 *      Author: lincoln
 */

void lcdBacklightON(){
    volatile char buff = UCB0TXBUF;
    buff |= BL;
    write_I2C(buff);
}

void lcdBacklightOFF(){
    volatile char buff = UCB0TXBUF;
    buff &= ~BL;
    write_I2C(buff);
}

void pulsa_e(void){
    volatile char buff = UCB0TXBUF;
    buff |= EN;
    write_I2C(buff);
    delay_100us(50);
    buff &= ~EN;
    write_I2C(buff);
}

void Instruction_mode(void){
    //RS = 0, RW = 0
    volatile char buff = UCB0TXBUF;
    buff &= ~RS;
    write_I2C(buff);
}

void Data_mode(void){
    //RS = 1, RW = 0
    volatile char buff = UCB0TXBUF;
    buff |= RS;
    write_I2C(buff);
}

void lcd_pins(void){  //Configura como saída todos os pinos utilizados pelo LCD
    I2C_config();
  /*
   * UCB0TXBUF:
   * BIT0->RS
   * BIT1->R/W
   * BIT2->EN
   * BIT3->Backlight
   * BIT4->D4
   * BIT5->D5
   * BIT6->D6
   * BIT7->D7
   */
}

void lcd_nib(char nibble){  //recebe um nibble 0000 xxxx, escreve no bus e pulsa E//zera os dados
    volatile char buff = UCB0TXBUF;
    nibble &= 0xf;
    buff &= ~(D4 | D5 | D6 |D7);    //zera os dados
    buff |= nibble<<4;
    write_I2C(buff);
    pulsa_e();
}

void lcd_cmd(char byte){  //RS = 0, recebe um byte, quebra em 2 nibbles e escreve com lcd_nib()
  Instruction_mode();  // escrevendo uma instrução: RS=0
  lcd_Byte(byte);
}

void lcd_char(char byte){ //RS = 1, recebe um byte, quebra em 2 nibbles e escreve com lcd_nib()
  Data_mode();  // escrevendo dados: RS=1
  lcd_Byte(byte);
}

void lcd_Byte(char byte){ //recebe um byte, quebra em 2 nibbles e escreve com lcd_nib()
  char nibble1;
  char nibble2;
  nibble1 = (byte >> 4);
  lcd_nib(nibble1);
  nibble2 = (byte);
  lcd_nib(nibble2);
}

void lcd_init(void){
  lcdBacklightON();
  Instruction_mode();  // escrevendo uma instrução: RS=0
  lcd_nib(0x3);
  delay_100us(150);
  lcd_nib(0x3);
  delay_100us(1);
  lcd_nib(0x3);
  delay_100us(1);
  lcd_nib(0x2);  //ativa modo 4 bits
  delay_100us(1);
  lcd_cmd(0x28); //define 4 bits, 2 linhas, display 5x8
  delay_100us(20);
  lcd_cmd(0x8);  //display desativado, cursor desabilitado, cursor estático
  delay_100us(20);
  lcd_clr();     //limpa tudo
  lcd_cmd(0x6);  //modo entrada: cursor p/ direita, display parado
  delay_100us(20);
  lcd_cmd(0xF);  //display ativado, cursor habilitado, cursor piscante
}

void lcd_cursor(int x, int y)
{
    if (x < 16)
    {
        x |= 0x80;     // Set LCD for first line write
        switch (y)
        {
        case 1:
            x |= 0x40; // Set LCD for second line write
            break;
        case 2:
            x |= 0x60; // Set LCD for first line write reverse
            break;
        case 3:
            x |= 0x20; // Set LCD for second line write reverse
            break;
        }
        lcd_cmd(x);
     }
}

void lcd_clr(void){ // apaga display e posiciona o cursor na 1a linha e 1a coluna
  lcd_cmd(0x1);
  delay_100us(20);
}

void lcd_home(void){ // posiciona o cursor na 1a linha e 1a coluna
  lcd_cmd(0x02);
  delay_100us(20);
}

void lcd_str(char *pt){
  do{
  lcd_char(*pt);
  *pt++;
  }
  while (*pt != '\0');
}
