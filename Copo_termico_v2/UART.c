#include "UART.h"
#include <msp430.h>
/*
 * UART.c
 *
 *  Created on: 1 de dez de 2017
 *      Author: lincoln
 */

volatile char UART_input[100];
unsigned int RXByteCtr = 0;
int cnt = 0;

void UART_setup(){
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

void word_send(const char *str){
    while (*str != 0) { //Do this during current element is not
           //equal to null character
           while (!(UCTXIFG & UCA0IFG));
           //Ensure that transmit interrupt flag is set
           UCA0TXBUF = *str++;
           //Load UCA0TXBUF with current string element
       }       //then go to the next element
}

int has_input(void){
    return cnt;
}

char * get_input(){
    return &UART_input;
}

void clear_input(void){
    cnt = 0;
    //Reset cnt
    RXByteCtr = 0;
}


#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void){   //Check if the UCA0RXBUF is different from 0x0A
    //(Enter key from keyboard)
    if(UCA0RXBUF != 0x0A)
        UART_input[RXByteCtr++] = UCA0RXBUF;
    //If it is, load received character
    //to current UART_input string element
    else {
        cnt = 1;
        //If it is not, set cnt
        UART_input[RXByteCtr-1] = 0;
    }   //Add null character at the end of UART_input string (on the /r)
}
