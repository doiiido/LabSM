#include <msp430.h> 

/*
*   Alunos: Lincoln Abreu Barbosa 140045023
*           Bruno Freitas Feitosa Nunes 120112388
*/

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    P4DIR |= BIT7;              // Marca o LED em P4.7 como saida.
    P4OUT &= ~BIT7;             // Marca o LED como desligado para comessar certo.
    TA0CTL |= TACLR;            // Limpa o timer TA.
    TA0CTL |= TASSEL_1;         // Usando SMCLK (32,768kHz).
    TA0CTL |= ID_0;             // Divide por 1.
    TA0EX0 |= TAIDEX_0;         // Divide por 1 (extendido).
    TA0CCTL0 = CCIE;            // Ativa a interrupcao CCR0 do timer A0
    TA0CCR0 |= 16384;            // Marca o TA0CCR0 como 16384, resultando em uma chamada de 2Hz
                                // da rotina
    TA0CTL |= MC_1;             // Marca o modo de timer para contar ate TA0CCR0.

    __no_operation();
    __bis_SR_register(LPM3_bits+GIE);           //Ativa a interrupcao e vai pra baixo consumo LPM3
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
