#include <msp430.h>

/*
*   Alunos: Lincoln Abreu Barbosa 140045023
*           Bruno Freitas Feitosa Nunes 120112388
*/


#define ADCMEM0ADDR (__SFR_FARPTR) 0x0720;
#define ADCMEM1ADDR (__SFR_FARPTR) 0x0722;

unsigned short analog_x;
unsigned short analog_y;

void setTimerA();
void setPinos();
void setADC();
void setDMA();
void timerB_setup(void);
#define FLLN(x) (x-1)

int main(void) {

    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    setTimerA();
    setPinos();
    setADC();
    setDMA();
    timerB_setup();              // Configurando Timer B


    __no_operation();
    __bis_SR_register(LPM0_bits+GIE);  //Ativa a interrupcao e vai pra baixo consumo LPM0
    __no_operation();
}

void timerB_setup(){
    TB0CTL = (TBSSEL__SMCLK |                   // SMCLK = 1MHz
              ID_3         |                   // Dividir por 8: 1MHz/8 = 131kHz
              MC__UP);                          // Modo UP
    TB0EX0 |= TBIDEX_3;                         // 131/4 = 32khz
    TB0CCR0 = 257;                             // 100Hz, 10ms
    TB0CCR1 = 128;                             // 100hz, 10ms
    TB0CCTL1 = OUTMOD_7;                        // Modo toogle/set
    TB0CCR2 = 128;                             // 100Hz, 10ms
    TB0CCTL2 = OUTMOD_3;                        // Modo toogle/set
}

void setPinos(){
    P4DIR |= BIT7;
    P4SEL |= BIT7;
    P4DIR |= BIT3;
    P4SEL |= BIT3;
    // Port Mapping
    PMAPKEYID = 0x02D52;    // Escritas no modulo de mapeamento são protegidas
                            // por senha. Escrever 2D52 no registro PMAPKEYID
                            // permite reconfiguração de apenas uma porta.
    P4MAP7 = PM_TB0CCR2A;   // Conecta o pino P4.7 na saída do canal 2
                            // do Timer B.
    PMAPKEYID = 0x02D52;    // Escritas no modulo de mapeamento são protegidas
                                // por senha. Escrever 2D52 no registro PMAPKEYID
                                // permite reconfiguração de apenas uma porta.
    P4MAP3 = PM_TB0CCR1A;   // Conecta o pino P4.3 na saída do canal 1
                            // do Timer B.

    // pinos ADC
    P6SEL |= BIT1 | BIT2;
    P6DIR &= ~BIT1 & ~BIT2;
}

void setTimerA(){
    TA0CTL = (TASSEL__SMCLK |                   // SMCLK = 1MHz
                  ID_3         |                   // Dividir por 8: 1MHz/8 = 131kHz
                  MC__UP);                          // Modo UP
    TA0EX0 |= TAIDEX_3;                         // 131/4 = 32khz
    TA0CCR0 = 656;                            // 50Hz, 20ms
    TA0CCTL1 = OUTMOD_3;                        // Modo toogle/set
    TA0CCR1 = 655;                              //50hz, 20ms
    TA0CCTL0 = CCIE;            // Ativa a interrupcao CCR0 do timer A0
}

void setADC(){
    ADC12CTL0 &= ~ADC12ENC;   // desabilita - permitindo edição
    ADC12CTL0 &= ~ADC12MSC;     // multiple sample convertion
    ADC12CTL0 |= ADC12SHT0_0
              | ADC12ON       // habilita ADC
              | 0;
    ADC12CTL1 |= ADC12SHS_1   // timer do ADC TA0.1
              | ADC12SHP      // SAMPCON timerADC12SHP
              | ADC12DIV_0     // Divisor = 1
              | ADC12SSEL_3    // SMCLK
              | ADC12DIV_7      // SMCLK %8
              | ADC12CONSEQ_3 // repeat sequence of channels
              | 0;

    ADC12CTL2 |= ADC12TCOFF   // internal T sensor off
              //| ADC12RES_2
              | 0;

    ADC12MCTL0 |= ADC12INCH0
               | 0; // P6.1

    ADC12MCTL1 |= ADC12INCH1  // P6.1
               | ADC12EOS     // ultimo canal
               | 0;

    ADC12CTL0 |= ADC12ENC;    // habilita ADC
}

void setDMA(){
    DMACTL0 = DMA0TSEL_24     // DMA 0 ADC12IFGx
            | DMA1TSEL_24     // DMA 1 ADC12IFGx
            | 0;

    DMA0CTL |= DMAEN          // habilita DAM0
            |  DMADT_5        // repeated single transfer
            |  DMADSTINCR_0   // add dst unchanged
            |  DMASRCINCR_0   // add src unchanged
            |  0;

    DMA1CTL |= DMAEN          // habilita DAM1
            |  DMADT_5        // repeated block transfer
            |  DMADSTINCR_0   // add dst unchanged
            |  DMASRCINCR_0   // add src unchanged
            |  0;

    DMA0SA = ADCMEM0ADDR;
    DMA1SA = ADCMEM1ADDR;

    DMA0DA = &analog_x;
    DMA1DA = &analog_y;

    DMA0SZ = 1;
    DMA1SZ = 1;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0A_CCR0_ISR(void)
{
    TB0CCR1 = analog_x/16+1;
    TB0CCR2 = analog_y/16+1;
}
