/*Alunos: Lincoln Abreu Barbosa 140045023
          Bruno Freitas Feitosa Nunes 120112388 */

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




//***************************************************************************************
//  Laboratório de Arquitetura de Processadores Digitais - Turma B
//
//  Alunos/Autores:
//                  Cassio Fabius Cambraia Ribeiro  - 16/0025567
//                  Matheus Cordeiro Lima           - 16/0137837
//
//  Projeto Final -  Conversor A/D para amostrar a tensão de entrada de apenas um eixo do
//                   joystick (ou de um potênciômetro de 10kOhm).
//
//  Data: 29 de junho de 2017
//
//  Feito no Code Composer Studio 7.0
//  Modelo utilizado: MSP430F5529 LaunchPad
//***************************************************************************************



#include <msp430.h>

void pin_config(void);
void clock_config(void);
void timer_config(void);
void adc_config(void);

#define TST(x, y)    (x & (y))
#define SET(x, y)    (x |= (y))
#define CLR(x, y)    (x &= ~(y))
#define TOG(x, y)    (x ^= (y))

// A amostragem deve ser feita a cada 200ms e uma estrutura de dados deve
// armazenar os dados dos últimos 20 segundos de amostragem.
// Então, em 20 000ms haverá 100 leituras. É necessário uma estrutura com
// 100 posições. Utilizaremos um array e um ponteiro para percorrê-lo.
int values[100];
int * pointer = values;
unsigned char volatile number_of_readings = 0;
int volatile raw_value;

// É necessário uma variável para verificar o último estado em que ficou úmido.
// Por exemplo, se estava úmido (last_state = 0), e aí ficou seco (last_state = 1),
// a bomba será acionada por alguns segundos. A bomba será desligada quando ficar
// úmido novamente (last_state = 0) ou se passar muito tempo com ela ligada.
// Isso serve para evitar que a bomba queime, caso o pote fique vazio,
// ou caso o sensor tenha queimado/saído da terra e acabe inundando o vaso.
unsigned char volatile five_seconds = 0;
unsigned char volatile seconds_activated = 0;

// Para um resistor de 2,5kOhm como resistor de entrada externa do conversor AD,
// o tempo de amostragem deve ser:
// tsample > (2,5k + 1,8k) x ln(2^13).25p + 800ns
// tsample > 1,77us


void main(void) {
    char value [6];

    pin_config();
    clock_config();
    lcd_pins();
    lcd_init();
    timer_config();
    adc_config();
//    _enable_interrupt();                        // Enable global interrupt

    while(1) {
        while ( (ADC12IFG & BIT0) == 0);
        number_of_readings++;
        raw_value = ADC12MEM0;
        raw_value = raw_value;
        *pointer = raw_value;

        if (pointer == values + 99) {
            pointer = values;
        }

//        CLR(P1OUT, BIT2);

        itoa(*pointer, value);
        lcd_cursor(0,0);
        lcd_str(value);

        pointer++;
    }

}

//----------------------------------------------------------------------------------------
/*
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void){
    switch(__even_in_range(ADC12IV, 34)) {
        case 6:
            number_of_readings++;
            *pointer = ADC12MEM0;
            pointer++;
            break;
        default:
            break;
    }


    if (pointer == values + 99) {
        pointer = values;
    }
}*/

//----------------------------------------------------------------------------------------

void pin_config(void) {
    WDTCTL = WDTPW | WDTHOLD;                   // Stop watchdog timer

    // Entrada analógica A1 (P6.1)
    SET(P6SEL, BIT4);                           // Seleciona P6.4 com função I/O.

/*
    // Saída para o Relé = P1.2
    P1SEL &= ~BIT2;                             // Seleciona P1.0 com função I/O.
    P1DIR |= BIT2;                              // P1.0, que é o LED1, será saída.
    P1OUT &= ~BIT2;                             // Define LED1 como apagado.
    */
}
//----------------------------------------------------------------------------------------

void clock_config(void) {
    // Crystals
    P5SEL |= (BIT2 | BIT3);                     // Port select XT2.
    P5SEL |= (BIT4 | BIT5);                     // Port select XT1.

    UCSCTL6 &= ~(XT1OFF | XT1DRIVE_0 | XT1BYPASS);
    UCSCTL6 &= ~(XT2OFF | XT2DRIVE_0 | XT2BYPASS);

    UCSCTL6 &= ~(SMCLKOFF | XTS);               // SMCLK ligado e modo de baixa frequência
    UCSCTL6 = XCAP_3;                           // Internal load cap

    // Loop until XT1 fault flag is cleared
    do {
        UCSCTL7 = 0;                            // Clear XT1 fault flags
    } while (UCSCTL7);                          // Test XT1 fault flag

    UCSCTL8 = (SMCLKREQEN | MCLKREQEN | ACLKREQEN);

    UCSCTL4 = (SELA_0 | SELS_5);                // ACLK = XT1CLK = 32768Hz
                                                // SMCLK = XT2CLK = 4Mhz

    UCSCTL5 = DIVS_2;                           // SMCLK = (SMCLK)/4 = 1Mhz

    do {                                        // Limpa as flags de falta XT2, XT1 e DCO
      UCSCTL7 &= ~(XT2OFFG | XT1LFOFFG | DCOFFG);

      SFRIFG1 &= ~OFIFG;                        // Limpa a flag de falta
     } while (SFRIFG1 & OFIFG);                 // Testa se a flag de falta estabilizou

}

//----------------------------------------------------------------------------------------

void timer_config(void) {
    // Timer A, que controlará o funcionamento do sensor de umidade de solo:
    TA0CTL = (TASSEL__SMCLK |                   // SMCLK = 1MHz
              ID__4         |                   // Dividir por 4: 1MHz/4 = 250kHz
              MC__UP);                          // Modo UP
    TA0CCR0 = 50000;                            // 5Hz, 5 batidas por segundo
    TA0CCTL1 = OUTMOD_6;                        // Modo toogle/set
    TA0CCR1 = 25000;
}

void adc_config(void) {
    CLR(ADC12CTL0, ADC12ENC);                   // Desabilitar para configurar

//    SET(ADC12IE, BIT1);

    ADC12CTL0 = (ADC12SHT0_3 |                  // ADC tempo amostragem ADCMEM[0-7]
                ADC12ON);                       // Ligar ADC

    ADC12CTL1 = (ADC12CSTARTADD_0 |             // Armazenar na Posição 0
                ADC12SHS_1        |             // Usar TA0.1 (Selecionar (ADC12SC bit))
                ADC12SHP          |             // S/H usar timer
                ADC12DIV_0        |             // Divisor = 1
                ADC12SSEL_3       |             // SMCLK
                ADC12CONSEQ_2);                 // Modo Único Canal

    ADC12CTL2  = (ADC12TCOFF |                  // Desligar sensor temperatura
                 ADC12RES_2);                   // Resolução 12-bit

    ADC12MCTL0 = (ADC12EOS   |                  // Fim
                 ADC12SREF_0 |                  // VR+ = AVCC e VR- = AVSS
                 ADC12INCH_4);                  // A4 = Canal 4

    SET(ADC12CTL0, ADC12ENC);                   // Habilitar ADC12
}
