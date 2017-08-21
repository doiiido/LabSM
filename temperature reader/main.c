/*Alunos: Lincoln Abreu Barbosa 140045023
          Bruno Freitas Feitosa Nunes 120112388 */

#include <msp430.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define D4  BIT0
#define D5  BIT1
#define D6  BIT2
#define D7  BIT3
#define RS BIT6
#define EN BIT5
#define TST(x, y)    (x & (y))
#define SET(x, y)    (x |= (y))
#define CLR(x, y)    (x &= ~(y))
#define TOG(x, y)    (x ^= (y))

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
void pin_config(void);
void clock_config(void);
void timer_config(void);
void adc_config(void);
void itoa(int num, char to[]);
void word_send(const char *str);
void UART_setup(void);
void muda_faixa(char *input);
void listen(void);
void screen_update(int value);

volatile int high_temp = 55, low_temp = 45;
int values[20];
int * pointer = values;
unsigned char volatile number_of_readings = 0;
int volatile raw_value;
unsigned char volatile five_seconds = 0;
unsigned char volatile seconds_activated = 0;
char input[100];
unsigned int RXByteCtr = 0;
int cnt = 0;
volatile int previous = 0;
volatile int  on = 0;




int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    UART_setup();

    int temp;
    int med = 0;
    int out = 0;
    char buff[6];

    pin_config();
    //clock_config();
    lcd_pins();
    lcd_init();
    timer_config();
    adc_config();
    _enable_interrupt();                        // Enable global interrupt

    word_send("Se quiser mudar a temperatura envie high ou low para mudar os limites \n \r");
    word_send("Limiares: \n \r");
    itoa(low_temp, buff);
    word_send(buff);
    word_send("<T<");
    itoa(high_temp, buff);
    word_send(buff);
    word_send("\n \r");

    while(1) {
        if (cnt == 1) {
            //Check if cnt is 1
            cnt = 0;
            //Reset cnt
            RXByteCtr = 0;
            muda_faixa(input);
        }
        while ( (ADC12IFG & BIT0) == 0);
        number_of_readings++;
        raw_value = ADC12MEM0;
        raw_value = raw_value;
        *pointer = raw_value;
        if(med==0 || out == 3 || *pointer >= med -10 && *pointer <= med +10){
            med = (med+*pointer)/2;
            out = 0;    //resfriamento ou aquecimento muito acelerado (+ de 10 graus)
        }
        else{
            out++;
        }
        if (pointer == values + 10) {
            pointer = values;
            temp = (int)(77-(12*med/458));
            //itoa((med/37)-32, value);
            //itoa(med, value);
            if (temp != previous){
                previous = temp;
                lcd_init();
                screen_update(temp);
            }
            if (!on && temp <= low_temp){
                P2OUT |= BIT4;
                on = 1;
            }
            if ( temp >= high_temp){
                P2OUT &= ~BIT4;
                on = 0;
            }
            /*if(on == 1){
                if(count == 3 ){
                    if (cut == 1){
                        P2OUT |= BIT4;
                        cut = 0;
                        count =0;

                    }else if (cut == 0){
                        P2OUT &= ~BIT4;
                        cut = 1;
                        count = -2;
                    }
                }
                count ++;
            }*/
        }
        pointer++;
    }
}

void screen_update(int value){
    char buff[6];
    itoa(value, buff);
    lcd_cursor(0,0);
    lcd_str("  Temperatura:");
    lcd_cursor(0,1);
    lcd_str("   ");
    lcd_str(buff);
    lcd_str(" C");
    lcd_str(" ");
    itoa(low_temp, buff);
    lcd_str(buff);
    lcd_str("<T<");
    itoa(high_temp, buff);
    lcd_str(buff);
    word_send("Temperatura: \n \r");
    itoa(value, buff);
    word_send(buff);
    word_send(" C \n \r");
}

void listen(void){
    if (cnt == 1) {
        //Check if cnt is 1
        cnt = 0;
        //Reset cnt
        RXByteCtr = 0;
    }
}

void muda_faixa(char *input){
    int integer;
    int i;
    char buff[6];
    P2OUT &= ~BIT4; // previne sobreaquecimento por erro no bluetooth
    on = 0;
    if(strcmp("high", input)==0){
        word_send("Qual o limiar superior? \n \r");
        while(cnt == 0);
        listen();
        for(i = 0; i< (int)(sizeof(input[i])/sizeof(input[0]));i++){
            integer = (input[i]-'0');
            if(integer<0 || integer >9 ){
                word_send("Valor invalido! Operacao cancelada \n \r");
                return;
            }
        }
        integer = atoi(input);
        high_temp = integer;
        screen_update(previous);
    }else if(strcmp("low", input)==0){
        word_send("Qual o limiar inferior? \n \r");
        while(cnt == 0);
        listen();
        for(i = 0; i< (int)(sizeof(input[i])/sizeof(input[0]));i++){
            integer = (input[i]-'0');
            if(integer<0 || integer >9 ){
                word_send("Valor invalido! Operacao cancelada \n \r");
                return;
            }
        }
        integer = atoi(input);
        low_temp = integer;
        screen_update(previous);
    }else
        word_send("Nao entendi, mande high ou low pra mudar os limiares \n \r");

    word_send("Limiares: \n \r");
    itoa(low_temp, buff);
    word_send(buff);
    word_send("<T<");
    itoa(high_temp, buff);
    word_send(buff);
    word_send("\n \r");
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
  Instruction_mode();  // escrevendo uma instrucao: RS=0
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
  Instruction_mode();  // escrevendo uma instrucao: RS=0
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



void word_send(const char *str){
    while (*str != 0) { //Do this during current element is not
           //equal to null character
           while (!(UCTXIFG & UCA0IFG))
               ;
           //Ensure that transmit interrupt flag is set
           UCA0TXBUF = *str++;
           //Load UCA0TXBUF with current string element
       }       //then go to the next element
}

void UART_setup(void){
    P3SEL = BIT3 + BIT4;                        // P3.3,4 = USCI_A0 TXD/RXD
       UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
       UCA0CTL1 |= UCSSEL_2;                     // SMCLK
       UCA0BR0 = 6;                              // 1MHz 9600 (see User's Guide)
       UCA0BR1 = 0;                              // 1MHz 9600
       UCA0MCTL = UCBRS_0 + UCBRF_13 + UCOS16;   // Modln UCBRSx=0, UCBRFx=0,
   // over sampling
       UCA0CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**
       UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
}

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void){   //Check if the UCA0RXBUF is different from 0x0A
    //(Enter key from keyboard)
        if(UCA0RXBUF != 0x0A) input[RXByteCtr++] = UCA0RXBUF;
    //If it is, load received character
    //to current input string element
        else {cnt = 1;
            //If it is not, set cnt
            input[RXByteCtr-1] = 0;
        }   //Add null character at the end of input string (on the /r)
    }

//----------------------------------------------------------------------------------------

/*#pragma vector = ADC12_VECTOR
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
    SET(P6SEL, BIT4);                           // Seleciona P6.4 com funcao I/O.

    //Rele
    P2DIR |= BIT4;              // Marca o pino P2.4 como saida.
    P2DS |= BIT4;               // Drive Strength high
    P2OUT &= ~BIT4;             // Marca o pino como desligado.
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
    TA0CCR0 = 25000;                            // 5Hz, 5 batidas por segundo
    TA0CCTL1 = OUTMOD_6;                        // Modo toogle/set
    TA0CCR1 = 12500;
}

void adc_config(void) {
    CLR(ADC12CTL0, ADC12ENC);                   // Desabilitar para configurar

//    SET(ADC12IE, BIT1);

    ADC12CTL0 = (ADC12SHT0_3 |                  // ADC tempo amostragem ADCMEM[0-7]
                ADC12ON);                       // Ligar ADC

    ADC12CTL1 = (ADC12CSTARTADD_0 |             // Armazenar na Posicao 0
                ADC12SHS_1        |             // Usar TA0.1 (Selecionar (ADC12SC bit))
                ADC12SHP          |             // S/H usar timer
                ADC12DIV_0        |             // Divisor = 1
                ADC12SSEL_3       |             // SMCLK
                ADC12CONSEQ_2);                 // Modo Único Canal

    ADC12CTL2  = (ADC12TCOFF |                  // Desligar sensor temperatura
                 ADC12RES_2);                   // Resolucao 12-bit

    ADC12MCTL0 = (ADC12EOS   |                  // Fim
                 ADC12SREF_0 |                  // VR+ = AVCC e VR- = AVSS
                 ADC12INCH_4);                  // A4 = Canal 4

    SET(ADC12CTL0, ADC12ENC);                   // Habilitar ADC12
}
