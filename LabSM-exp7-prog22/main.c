#include <msp430.h>
#include <stdlib.h>
#include <string.h>

#define D4  BIT0
#define D5  BIT1
#define D6  BIT2
#define D7  BIT3
#define RS BIT6
#define EN BIT5

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
void time_delay(int mult);
void I2C_config(void);
void itoa(int num, char to[]);


/*Alunos: Lincoln Abreu Barbosa 140045023
          Bruno Freitas Feitosa Nunes 120112388 */

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    char addr[4];
    int i, address;
    I2C_config();
    lcd_pins();
    lcd_init();
    for(i = 0; i <= 127; i++){
        UCB0I2CSA = i;

        UCB0CTL1 |= UCTR | UCTXSTT | UCTXSTP;

        while((UCB0CTL1 & UCTXSTP) == UCTXSTP);
        if((UCB0IFG & UCNACKIFG) == UCNACKIFG){
            UCB0IFG &= ~UCNACKIFG;
        }else{
            address = i;
            break;
        }
    }

    while(1);
    itoa(address, addr);
    lcd_cursor(0,0);
    lcd_char(addr);
    while(1);

}

void I2C_config(void){

    P3SEL |= BIT0 | BIT1;   //p3.0 SDA, 3.1 SCL
    P3REN |= BIT0 | BIT1;
    P3OUT |= BIT0 | BIT1;

    UCB0CTL1 = UCSWRST;

    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;
    UCB0BRW = 10;
    UCB0CTL1 = UCSSEL_2;
}



void itoa(int num, char to[]){
    char* p = to;
    int aux, div = 1;
    int j, k = 2;
    while(div*10 < num){
        div *= 10;
        k++;
    }
    for(j = 0; j < k; j++) {
        aux = (num/div) % 10;
        div /= 10;
        *p=aux + '0';
        ++p;
    }
    to[k-1] = '\0';
}

void pulsa_e(void){
  P3OUT |= EN;
  time_delay(50);
  P3OUT &= ~EN;
}

void Instruction_mode(void){
    //RS = 0, RW = 0
    P3OUT &= ~RS;
}

void Data_mode(void){
    //RS = 1, RW = 0
    P3OUT |= RS;
}
void lcd_pins(void){  //Configura como saída todos os pinos utilizados pelo LCD
  P3DIR |= EN;
  P3DIR |= RS;

  P6DIR |= D4;
  P6DIR |= D5;
  P6DIR |= D6;
  P6DIR |= D7;
  time_delay(150);
}

void lcd_nib(char nibble){  //recebe um nibble 0000 xxxx, escreve no bus e pulsa E//zera os dados
  P6OUT &= ~(D4 | D5 | D6 |D7);    //zera os dados
  P6OUT = nibble;
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
  nibble1 = (byte & 0xF0);// Apagando os dados diferentes de xxxx0000
  nibble1 = (byte >> 4);
  lcd_nib(nibble1);
  nibble2 = (byte & 0x0F);// Apagando os dados diferentes de 0000xxxx
  lcd_nib(nibble2);
}

void lcd_init(void){
  Instruction_mode();  // escrevendo uma instrução: RS=0
  lcd_nib(0x3);
  time_delay(150);
  lcd_nib(0x3);
  time_delay(1);
  lcd_nib(0x3);
  time_delay(1);
  lcd_nib(0x2);  //ativa modo 4 bits
  time_delay(1);
  lcd_cmd(0x28); //define 4 bits, 2 linhas, display 5x8
  time_delay(20);
  lcd_cmd(0x8);  //display desativado, cursor desabilitado, cursor estático
  time_delay(20);
  lcd_clr();     //limpa tudo
  lcd_cmd(0x6);  //modo entrada: cursor p/ direita, display parado
  time_delay(20);
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
  time_delay(20);
}

void lcd_home(void){ // posiciona o cursor na 1a linha e 1a coluna
  lcd_cmd(0x02);
  time_delay(20);
}

void lcd_str(char *pt){
  do{
  lcd_char(*pt);
  *pt++;
  }
  while (*pt != '\0');
}

void time_delay (int mult){
    int i = 0;
    int ccr0 = 2 * mult;   //ccr0 ~=0,1 ms
    for(i = 0; i<ccr0; i++){
        __no_operation();
    }
}
