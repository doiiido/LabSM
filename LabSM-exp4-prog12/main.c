#include <msp430.h> 

/*
 * main.c
 */
#define DELAY = 0xFFFFFFFF

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    P1DIR |= BIT0;
    P1OUT |= BIT0;
    long int i=0;
    for(;;){
            P1OUT ^= BIT0;
    }
    return 0;
}
