#include <msp430.h>
#include <stdlib.h>
#include <string.h>

#define D4  BIT0
#define D5  BIT1
#define D6  BIT2
#define D7  BIT3
#define RS BIT6
#define EN BIT5
#define FLLN(x) (x-1)

/*
*   Alunos: Lincoln Abreu Barbosa 140045023
*           Bruno Freitas Feitosa Nunes 120112388
*/

float ccr0 = 3277;                 // Marca o ccr0 como 3277, resultando em uma chamada de 10Hz (frequencia base 32768Hz)
int S1 = 0, S2 = 0;
int Running = 0;
int hour = 0, min = 0, sec = 0, dec = 0;
char Hour[3], Min[3], Sec[3], Dec[3];
char Time[11] = "00:00:00,0", Lap[11] = "00:00:00,0";


void timer_setup(void);
void s1_s2_setup(void);
void format_time (void);
void Make_lap (void);
void printString(char *);
void core_initialize(void);
void screen_update(void);
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

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    s1_s2_setup();              // Configurando Botoes
    timer_setup();              // Configurando Timer
    core_initialize ();         // Mudando os clocks para melhorar sincronia
    lcd_pins();
    lcd_init();
    screen_update();

    __no_operation();
    __bis_SR_register(LPM0_bits+GIE);  //Ativa a interrupcao e vai pra baixo consumo LPM3
    __no_operation();
}
// Timer1 B0 interrupt service routine
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMERB0_CCR0_ISR(void)
{
    if (Running == 1){          // Se o cronometro esta ligado
        dec ++;
        format_time();
    }else{
        TB0CTL |= MC_0;            // Para o timer TB.
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){
    switch (__even_in_range( P1IV, P1IV_P1IFG7 )){
        case P1IV_P1IFG1:
            if (S2==0){
                if(Running == 1){    // 0<=PWM<=100
                    Make_lap();   // Aumenta em 5%
                }else{
                    timer_setup();              // Reconfigurando Timer
                    hour = 0;
                    min = 0;
                    sec = 0;
                    dec = 0;
                    strcpy(Time, "00:00:00,0");
                    strcpy(Lap, "00:00:00,0");
                    screen_update();
                }
                S2 = 1;     // Marca botao S2 como apertado
                P1IES &= ~BIT1;      // Modo de interrupcao entre edge down-up
                __delay_cycles(5000);   // Debouncing
                P1IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S2.
                break;
            }else
                S2 = 0;     // Marca botao S2 como solto
            __delay_cycles(5000);   // Debouncing
            P1IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S2.
            P1IES |= BIT1;      // Modo de interrupcao entre edge up-down
            break;
        default:
            _never_executed();
    }
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void){
    switch (__even_in_range( P2IV, P2IV_P2IFG7 )){
        case P2IV_P2IFG1:
            if (S1==0){
                if(Running == 1){    // 0<=PWM<=100
                    Running = 0;   // Aumenta em 5%
                    TB0CTL |= MC_0;            // Para o timer TB.
                }else{
                    Running = 1;
                    TB0CTL |= MC_1;
                }
                S1 = 1;     // Marca botao S1 como apertado
                P2IES &= ~BIT1;      // Modo de interrupcao entre edge down-up
                __delay_cycles(5000);   // Debouncing
                P2IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S2.
                break;
            }else
                S1 = 0;     // Marca botao S1 como solto
            __delay_cycles(5000);   // Debouncing
            P2IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S1.
            P2IES |= BIT1;      // Modo de interrupcao entre edge up-down
            break;
        default:
            _never_executed();
    }
}

void dec_itoa(int i, char to[]){
    char const digit[]  = "0123456789";
    char* p = to;
    int shifter;
    shifter = i;
    do {
        ++p;
        shifter = shifter / 10;
    } while (shifter);
    *p = '\0';
    do {
        *--p = digit[(i % 10)];
        i = i / 10;
    } while (i);
}

void itoa(int i, char to[]){
    char const digit[]  = "0123456789";
    char* p = to;
    int shifter;
    if (i < 10) {
        *p++ = '0';
    }
    shifter = i;
    do {
        ++p;
        shifter = shifter / 10;
    } while (shifter);
    *p = '\0';
    do {
        *--p = digit[(i % 10)];
        i = i / 10;
    } while (i);
}

void format_time (void){
    char *buff;
    if(dec > 9){
        sec++;
        dec = dec- 10;
    }
    if(sec > 59){
        min++;
        sec = sec- 60;
    }
    if(min > 59){
        hour++;
        min = min- 60;
    }
    if(hour > 99){
        hour = 0;
        min = 0;
        sec = 0;
        dec = 0;
    }
    dec_itoa(dec, Dec);
    itoa(sec, Sec);
    itoa(min, Min);
    itoa(hour, Hour);
    buff=strcat(Hour,":");
    buff=strcat(buff, Min);
    buff=strcat(buff,":");
    buff=strcat(buff, Sec);
    buff=strcat(buff,",");
    buff=strcat(buff, Dec);
    strcpy(Time, buff);
    screen_update();
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
  nibble1 = (byte >> 4);
  lcd_nib(nibble1);
  nibble2 = (byte);
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



void screen_update(){
    lcd_cursor(0,0);
    lcd_str(Time);
    lcd_cursor(0,1);
    lcd_str(Lap);
}

void Make_lap (void){
    strcpy(Lap,Time);
}



void timer_setup(){
    TB0CTL |= TBCLR;            // Limpa o timer TB.
    TB0CTL |= TBSSEL_1;         // Usando ACLK (32,768kHz).
    TB0CTL |= ID__1;             // Divide por 1.
    TB0EX0 |= TBIDEX__1;         // Divide por 1 (extendido) = 32678Hz.
    TB0CCTL0 = CCIE;            // Ativa a interrupcao CCR0 do timer B0
    TB0CCR0 = ccr0;             // Marca o TB0CCR0 como ccr0
}

void s1_s2_setup(){
    P1DIR &= ~BIT1;         // Marca o pino P1.1 como entrada para S2.
    P1REN |= BIT1;          // Marca a entrada do pino P1.1 com resistor.
    P1OUT |= BIT1;          // Marca o resistor da entrada P1.1 como pullup.

    P2DIR &= ~BIT1;         // Marca o pino P2.1 como entrada para S1.
    P2REN |= BIT1;          // Marca a entrada do pino P2.1 com resistor.
    P2OUT |= BIT1;          // Marca o resistor da entrada P2.1 como pullup.

    P2IE |= BIT1;           // Ativa a interrupcao do S1.
    P2IES |= BIT1;          // Modo de interrupcao para edge up-down
    P2IFG &= ~BIT1;         // Limpa flag de interrupcao

    P1IE |= BIT1;           // Ativa a interrupcao do S2.
    P1IES |= BIT1;          // Modo de interrupcao para edge up-down
    P1IFG &= ~BIT1;         // Limpa flag de interrupcao
}

void core_initialize(void){
    //Configure clock system
    UCSCTL0 = DCO4_H | DCO2_H;              //SET DCO = 18

    UCSCTL1 = DCORSEL_3;                    // Select DCO range to 0.25MHz to 2,5MHz

    UCSCTL2 = FLLD__16 | FLLN(121);          // Loop multipliers: (16) * (121 + 1) * 32.768 / 12 = 5,330MHz

    UCSCTL3 = SELREF__XT1CLK  |             // Set DCO FLL reference = XT1CLK
              FLLREFDIV__12;                // XT1CLK 32,768/12 = 2,730666kHz

    UCSCTL4 = SELM__DCOCLK;                 // MCLK  = DCOCLK

    UCSCTL5 = DIVM__1;                      // MCLK  DCOCLK/1 = 5,330MHz

    UCSCTL6 &= ~XT1OFF;                      // Garante que o XT1 esta ligado
}
