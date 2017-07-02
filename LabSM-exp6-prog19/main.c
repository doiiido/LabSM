#include <msp430.h>
#include <stdlib.h>
#include <string.h>

#define RS_High BIT6
#define EN_High BIT5
#define D7_High BIT3
#define D6_High BIT2
#define D5_High BIT1
#define D4_High BIT0
#define FLLN(x) (x-1)

/*
*   Alunos: Lincoln Abreu Barbosa 140045023
*           Bruno Freitas Feitosa Nunes 120112388
*/

float ccr0 = 51;                 // Marca o ccr0 como 51, resultando em uma chamada de 10Hz (frequencia base 512Hz)
int S1 = 0, S2 = 0;
int Running = 0;
int hour = 0, min = 0, sec = 0, dec = 0;
char Hour[3], Min[3], Sec[3], Dec[3];
char Time[11] = "00:00:00,0", Lap[11] = "00:00:00,0";


void timer_setup(void);
void s1_s2_setup(void);
void format_time (void);
void Make_lap (void);
void limpa_tela(void);
void printString(char *);
void pulaLinha(void);
void core_initialize(void);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    P4DIR |= BIT7;              // Marca o LED em P4.7 como saida.
    P4OUT &= ~BIT7;             // Marca o LED como desligado para comessar certo.
    s1_s2_setup();              // Configurando Botoes
    timer_setup();              // Configurando Timer
    core_initialize ();         // Mudando os clocks para melhorar sincronia
    init_4bitmode();
    screen_update();

    __no_operation();
    __bis_SR_register(LPM0_bits+GIE);  //Ativa a interrupcao e vai pra baixo consumo LPM3
    __no_operation();
}
// Timer1 B0 interrupt service routine
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER0_CCR0_ISR(void)
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

void screen_update(){
    limpa_tela();
    printString(Time);
    pulaLinha();
    printString(Lap);
}

void Make_lap (void){
    strcpy(Lap,Time);
}

//EN = 1 , wait,  EN = 0
void pulse(){
    P3OUT |= EN_High;
    __delay_cycles(1000);
    P3OUT &= ~(EN_High);
}

void writeNible(char data){

    //zera os dados
    P6OUT &= ~(BIT0 | BIT1 | BIT2 | BIT3);
    //passa os dados
    P6OUT |= data;
    pulse();

}

void writeByte(char data){
    writeNible(data >> 4);
    writeNible(data);
}

void setInstruction(){
    //RS = 0, RW = 0
    P3OUT &= ~(RS_High);
}

void setData(){
    //RS = 1, RW = 0
    P3OUT |= RS_High;
}

void init_4bitmode(){
    //define os pinos como saida
    P3DIR |= (EN_High | RS_High);
    P6DIR |= (BIT0 | BIT1 | BIT2 | BIT3);

    //espera p15ms para VCC > 4.5
    __delay_cycles(21000);

    //define que as proximas instrucoes
    //serao de escrita de instrucoes
    setInstruction();

    //define modo de uso
    writeNible(0x03);   //MSB
    __delay_cycles(21000);
    writeNible(0x03);   //LSB
    __delay_cycles(100);

    writeNible(0x03);
    __delay_cycles(100);
    writeNible(0x02);
    __delay_cycles(100);

    //N=1 (2 linhas), F=0 (5x8)
    writeByte(0x28);
    __delay_cycles(2000);

    //mostador desativado
    writeByte(0x08);
    __delay_cycles(2000);

    //limpa mostrador
    writeByte(0x01);
    __delay_cycles(2000);

    //definir modo de entrada
    writeByte(0x06);
    __delay_cycles(2000);

    //display ON, cursor piscando
    writeByte(0x0F);

}

void printString(char* str){
    setData();
    int i;
    for(i = 0 ; i < strlen(str) ; i++){
        writeByte(str[i]);
    }
}

void pulaLinha(){
    setInstruction();
    writeByte(0xC0);
    __delay_cycles(7500);
}

void limpa_tela(){
    setInstruction();
    //limpa mostrador
    writeByte(0x01);
    __delay_cycles(7500);

}

void timer_setup(){
    TB0CTL |= TBCLR;            // Limpa o timer TB.
    TB0CTL |= TBSSEL_1;         // Usando ACLK (32,768kHz).
    TB0CTL |= ID__8;             // Divide por 8.
    TB0EX0 |= TBIDEX__8;         // Divide por 8 (extendido) = 512Hz.
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
