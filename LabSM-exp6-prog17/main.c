#include <msp430.h>

void I2C_config(void);
void write_I2C(char byte);
void lcdBacklightON();
void lcdBacklightOFF();
void time_delay (int mult);
/**
 * main.c
 */
int main(void){
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	I2C_config();
	while(1){
	    lcdBacklightON();
        time_delay(16383);
        time_delay(16383);
        time_delay(16383);
        lcdBacklightOFF();
        time_delay(16383);
        time_delay(16383);
        time_delay(16383);
	}

	return 0;
}

void lcdBacklightON(){
    volatile char buff = 0;
    buff |= BIT3;
    write_I2C(buff);
}

void lcdBacklightOFF(){
    volatile char buff = 0;
    buff &= ~BIT3;
    write_I2C(buff);
}

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
}

void time_delay (int mult){
    int i = 0;
    int ccr0 = 2 * mult;   //ccr0 ~=0,1 ms
    for(i = 0; i<ccr0; i++){
        __no_operation();
    }
}
