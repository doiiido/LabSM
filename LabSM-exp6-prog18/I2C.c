#include "I2C.h"
#include <msp430.h>

/*
 * I2C.c
 *
 *  Created on: 16 de nov de 2017
 *      Author: lincoln
 */

void I2C_config(void){

    UCB0CTL1 |= UCSWRST; // Enable SW reset

    P3SEL |= BIT0 | BIT1;   //p3.0 SDA, 3.1 SCL
    P3REN |= BIT0 | BIT1;
    P3OUT |= BIT0 | BIT1;

    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;   // I2C Master, synchronous mode
    UCB0CTL1 |= UCSSEL_2 + UCSWRST; // Use SMCLK, keep SW reset
    UCB0BR0 = 11; //1048576/11 ~= 100khz
    UCB0BR1 = 1; //((int)1048576/11 - 1048576/11)*16

    UCB0I2CSA = 0x3f;   //Slave Address

    UCB0CTL1 &= ~UCSWRST;   //Clear SW reset, resume operation
}

void write_I2C(char byte) {

        UCB0CTL1 |= UCTR | UCTXSTT; // I2C TX, start condition

        while((UCB0IFG & UCTXIFG) == 0); //Wait for line idle

        UCB0TXBUF = byte;

        while((UCB0IFG & UCTXIFG) == 0); //Wait for line idle

        UCB0CTL1 |= UCTXSTP;

        while(UCB0CTL1 & UCTXSTP);  // Ensure stop condition got sent

        delay_100us(1);
}

void delay_100us (int mult){
    int i = 0;
    int ccr0 = 2 * mult;   //ccr0 ~=0,1 ms
    for(i = 0; i<ccr0; i++){
        __no_operation();
    }
}
