#include <msp430.h> 

/*
 * main.c
 */
#define DELAY = 0xFFFFFFFF

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    P1DIR |= BIT0;              // Marca o LED em P1.0 como saida.
    P1OUT &= ~BIT0;             // Marca o LED como desligado para comessar certo.
    TA0CTL |= TACLR;            // Limpa o timer TA.
    TA0CTL |= TASSEL_1;         // Usando ACLK (32768 Hz).
    TA0CTL |= ID_0;             // Divide por 1.
    TA0EX0 |= TAIDEX_0;         // Divide por 1 (extendido).
    TA0CTL |= TAIE;             // Ativa a interrupcao do timer TA0.
    TA0CCR0 |= 16384;           // Marca o TA0CCR0 como 16384, resultando em uma chamada de 2Hz
                                // da rotina
    TA0CTL |= MC_1;             // Marca o modo de timer para contar ate TA0CCR0.

    __bis_SR_register(LPM3_bits+GIE);           //Ativa a interrup�ao e vai pra baixo consumo LPM3

}
// Timer1 A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
    P1OUT ^= BIT0;                            // Toggle P1.0
}
