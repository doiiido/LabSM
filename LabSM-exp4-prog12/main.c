#include <msp430.h> 

/*
 * main.c
 */
#define DELAY = 0xFFFFFFFF

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    P1DIR |= BIT0;
    P1OUT |= BIT0;
    for(;;){
        __delay_cycles(300000);
        P1OUT ^= BIT0;
    }
    return 0;
}
