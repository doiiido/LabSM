#include <msp430.h> 

#define FLLN(x) (x-1)

/*
*   Alunos: Lincoln Abreu Barbosa 140045023
*           Bruno Freitas Feitosa Nunes 120112388
*/

void core_initialize(void);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    core_initialize ();         // Mudando os clocks
    P4DIR |= BIT7;              // Marca o LED em P4.7 como saida.
    P4OUT &= ~BIT7;             // Marca o LED como desligado para comessar certo.
    TA0CTL |= TACLR;            // Limpa o timer TA.
    TA0CTL |= TASSEL_2;         // Usando SMCLK (333,141kHz).
    TA0CTL |= ID_3;             // Divide por 8 (= 41,642khz).
    TA0EX0 |= TAIDEX_0;         // Divide por 1 (extendido).
    TA0CCTL0 = CCIE;            // Ativa a interrupcao CCR0 do timer A0
    TA0CCR0 |= 20821;           // Marca o TA0CCR0 como 20821 = 41642/2, resultando em uma chamada de 2Hz
                                // da rotina (0,5s aceso, 0,5s apagado)
    TA0CTL |= MC_1;             // Marca o modo de timer para contar ate TA0CCR0.

    /*
    TA0CTL |= TAIE;             // Ativa a interrupcao do timer TA0.
    requer:
            // Timer1 A0 interrupt service routine
        #pragma vector=TIMER0_A1_VECTOR
        __interrupt void TIMER0_CCR0_ISR(void)
        {
            TA0IV = 0;            // Limpa todas as flags do timer TA.
        }
       */
    __no_operation();
    __bis_SR_register(LPM0_bits+GIE);           //Ativa a interrupcao e vai pra baixo consumo LPM0 (LPM3 desliga SMclock)
    __no_operation();
}
// Timer1 A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_CCR0_ISR(void)
{
    /*
     * Vector 0: No interrupt
     * Vector 2: TA0CCR1
     * Vector 4: TA0CCR2
     * Vector 6: TA0CCR3
     * Vector 8: TA0CCR4
     * Vector 10: TA0CCR5
     * Vector 12: TA0CCR6
     * Vector 14: TA0IFG (Timer overflow)
     */
    P4OUT ^= BIT7;             // Toggle P4.7
}

void core_initialize(void){
    //Configure clock system
    UCSCTL0 = DCO4_H | DCO2_H;              //SET DCO = 18

    UCSCTL1 = DCORSEL_1;                    // Select DCO range to 0.25MHz to 2,5MHz

    UCSCTL2 = FLLD__4 | FLLN(121);          // Loop multipliers: (4) * (121 + 1) * 32.768 / 12 = 1,332565MHz

    UCSCTL3 = SELREF__XT1CLK  |             // Set DCO FLL reference = XT1CLK
              FLLREFDIV__12;                // XT1CLK 32,768/12 = 2,730666kHz

    UCSCTL4 = SELS__DCOCLKDIV   |              // SMCLK = DCOCLKDIV (DCOCLK/4 = 333,141kHz)
              SELM__DCOCLK;                 // MCLK  = DCOCLK

    UCSCTL5 = DIVS__1      |                // SMCLK DCOCLKDIV/1 = 333,141kHz
              DIVM__1;                      // MCLK  DCOCLK/1 = 1,332565MHz

    UCSCTL6 &= ~SMCLKOFF;                   // Garante que o SMCLK esta ligado
    UCSCTL6 &= ~XT1OFF;                      // Garante que o XT1 esta ligado
}
