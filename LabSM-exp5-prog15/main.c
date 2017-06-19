/*Alunos: Lincoln Abreu Barbosa 140045023
          Bruno Freitas Feitosa Nunes 120112388 */

#include <msp430.h>
#include <stdlib.h>

#define D4  BIT0
#define D5  BIT1
#define D6  BIT2
#define D7  BIT3

//void pulsa_e(void);
void lcd_pins(void);
void lcd_nib(char nibble);
void lcd_cmd(char byte);
void lcd_char(char byte);
void lcd_inic(void);
void lcd_cursor(int c, int l);
void lcd_clr(void);
void lcd_home(void);
void lcd_str(char *pt);

/*void pulsa_e(void){
  P3OUT |= BIT5;
  __delay_cycles(5000);
  P3OUT &= ~BIT5;
}*/

void lcd_pins(void){  //Configura como saída todos os pinos utilizados pelo LCD
  P3OUT &= ~BIT5;
  P3DIR |= BIT5;

  P3OUT &= ~BIT6;
  P3DIR |= BIT6;

  P6OUT &= ~D4;
  P6DIR |= D4;

  P6OUT &= ~D5;
  P6DIR |= D5;

  P6OUT &= ~D6;
  P6DIR |= D6;

  P6OUT &= ~D7;
  P6DIR |= D7;
}

void lcd_nib(char nibble){  //recebe um nibble 0000 xxxx, escreve no bus e pulsa E
  P6OUT = nibble;
  P3OUT = P3OUT | BIT5;
  __delay_cycles(5000);
  P3OUT = P3OUT & ~BIT5; //Pulsa enable
}

void lcd_cmd(char byte){  //RS = 0, recebe um byte, quebra em 2 nibbles e escreve com lcd_nib()
  char nibble1;
  char nibble2;
  P3OUT &= ~BIT6;  // escrevendo uma instrução: RS=0
  nibble1 = (byte & 0xF0);
  nibble2 = (nibble1>>4)&(0xF0);
  lcd_nib(nibble1);
  nibble2 = (byte & 0x0F);
  lcd_nib(nibble2);
}

void lcd_char(char byte){ //RS = 1, recebe um byte, quebra em 2 nibbles e escreve com lcd_nib()
  char nibble1;
  char nibble2;
  P3OUT |= BIT6;  // escrevendo uma instrução: RS=1
  nibble1 = (byte & 0xF0);
  nibble2 = (nibble1>>4)&(0xF0);
  lcd_nib(nibble1);
  nibble2 = (byte & 0x0F);
  lcd_nib(nibble2);
}

void lcd_inic(void){
  P3OUT &= ~BIT6;  // escrevendo uma instrução: RS=0

  __delay_cycles(20000);
  lcd_nib(0x3);

  __delay_cycles(10000);
  lcd_nib(0x3);

  __delay_cycles(2000);
  lcd_nib(0x3);

  __delay_cycles(2000);
  lcd_nib(0x2);  //ativa modo 4 bits

  lcd_cmd(0x28); //define 4 bits, 2 linhas, display 5x8
  lcd_cmd(0x8);  //display desativado, cursor desabilitado, cursor estático
  lcd_clr();     //limpa tudo
  lcd_cmd(0x6);  //modo entrada: cursor p/ direita, display parado
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
}

void lcd_home(void){ // posiciona o cursor na 1a linha e 1a coluna
  lcd_cmd(0x02);
}

void lcd_str(char* pt)
{
    while (*pt != '\0') {
        lcd_char(*pt);
        pt++;
    }
}


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    int t, p, c;

    lcd_pins();
    lcd_inic();
    for(t=0;t<4;t++){
        lcd_cursor(0,0);
        for(p=0;p<16;p++){
          c++;
          lcd_char(c);
          __delay_cycles(300000);
        }
        lcd_cursor(0,1);
        for(p=0;p<16;p++){
          c++;
          lcd_char(c);
          __delay_cycles(300000);
        }
        __delay_cycles(500000);
        lcd_home();
        __delay_cycles(100000);
        lcd_clr();
    }
    return(0);
}

