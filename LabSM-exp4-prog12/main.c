#include <msp430.h> 

/*
*   Alunos: Lincoln Abreu Barbosa 140045023
*           Bruno Freitas Feitosa Nunes 120112388
*/

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    P1DIR |= BIT0;              // Marca o LED em P1.0 como saida.
    P1OUT &= ~BIT0;             // Marca o LED como desligado para comessar certo.
    while(1){
        __delay_cycles(305000); //Timer 2HZ
        P1OUT ^= BIT0;          // Toggle P1.0
    }
}
