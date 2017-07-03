/* This program asks you for the password, and then tells
 * you if you typed it right.  You need to set the bluetooth
 * terminal to send \r\n, at the end of a send.  The app
 * I used on my Android phone is from:
 * https://github.com/Sash0k/bluetooth-spp-terminal
 *
 * Originally from the textbook, modified for the MSP430F5529
 * by Rob Frohne 7/21/2015.
 *
 * The HC-06 bluetoooth module I used to test this is connected
 * to the +5V and GND on the launchpad.  The RXD of the HC-06 is
 * connected to P3.3 and TXD of the HC-06 is connected to P3.4.
 *
 * Note the changes to the interrupt as compared to the G2 Launchpad.
 */
#include <msp430.h>
#define RedLed BIT0
#define GreenLed BIT7
const char password[] = "12345"; //The Password
const char enter[] = "Enter Your Password\r\n";
const char correct[] = "Your password is correct\r\n";
const char incorrect[] = "Your password is incorrect\r\n";
const char reenter[] = "Please re-enter your password\r\n";
char input[100];
unsigned int RXByteCtr = 0;
int cnt = 0;
int inputlength, passwordlength;
int difference;
void transmit(const char *str);
int compare(const char *strin, const char *strpass);
int arraylength(const char *str);
int abs(int a);

/*
 * main.c
 */
void main(void)

{
    WDTCTL = WDTPW | WDTHOLD;
    /*  BCSCTL1 = CALBC1_1MHZ;//Adjust the clock
     DCOCTL = CALDCO_1MHZ;*/
    P1DIR = RedLed; //Make P1.0 an output so we can use the red LED
    P4DIR = GreenLed; //Make P4.7 an output so we can use the red LED
    P1OUT &= ~RedLed;  //Clear the red LED
    P4OUT &= ~GreenLed;  //Clear the green LED

    P3SEL = BIT3 + BIT4;                        // P3.3,4 = USCI_A0 TXD/RXD
    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 6;                              // 1MHz 9600 (see User's Guide)
    UCA0BR1 = 0;                              // 1MHz 9600
    UCA0MCTL = UCBRS_0 + UCBRF_13 + UCOS16;   // Modln UCBRSx=0, UCBRFx=0,
// over sampling
    UCA0CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**
    transmit(enter);
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
    _enable_interrupts();
    while (1) {
        if (cnt == 1) {
            //Check if cnt is 1
            inputlength = arraylength(input);
            //Get your input length
            passwordlength = arraylength(password);
            //Get your password length
            {
                difference = compare(input, password);
            }
            //Compare the received password with your password
            if (difference == 0) {
                //Check if they match
                {
                    transmit(correct);
                }
                //If they match, transmit correct string
                P1OUT &= ~RedLed;
                P4OUT |= GreenLed;
                //Turn on the green LED
                __delay_cycles(5000000);
                //Wait for 5 seconds
                P4OUT &= ~GreenLed;
                //WDTCTL = WDT_MRST_0_064; // Turns off the LED too.
                transmit(enter);
                //Reset the system
            } else {                         //If they do not match
                {
                    transmit(incorrect);
                }
                //Transmit incorrect string
                P1OUT = RedLed;
                //Turn on the red LED
                __delay_cycles(2000000);
                //Wait for 2 seconds
                P4OUT &= ~RedLed;
                //Turn off the red LED
                {
                    transmit(reenter);
                }
            }                         //Transmit reenter string
            cnt = 0;
            //Reset cnt
            RXByteCtr = 0;
        }
    }                         //Reset Receive Byte counter
}

//USCI A receiver interrupt
// The stuff immediately below is to make it compatible with GCC, TI or IAR
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{   //Check if the UCA0RXBUF is different from 0x0A
//(Enter key from keyboard)
    if(UCA0RXBUF != 0x0A) input[RXByteCtr++] = UCA0RXBUF;
//If it is, load received character
//to current input string element
    else {cnt = 1;
        //If it is not, set cnt
        input[RXByteCtr-1] = 0;
    }   //Add null character at the end of input string (on the /r)
}
void transmit(const char *str) {
    while (*str != 0) { //Do this during current element is not
        //equal to null character
        while (!(UCTXIFG & UCA0IFG))
            ;
        //Ensure that transmit interrupt flag is set
        UCA0TXBUF = *str++;
        //Load UCA0TXBUF with current string element
    }       //then go to the next element
}

int max(int a, int b) {
    if (a > b)
        return a;
    else
        return b;
} // Find the max between two numbers.

int compare(const char *strin, const char *strpass) {
    int i = 0;
    int result = 0; //Clear result
    int len = max((passwordlength), inputlength);
    for (i = len; i > 0; i--) {
        result = result + abs((*strin++) - (*strpass++));
    }  // abs used to make sure differences don't cancel
    return result;
}  //Return result value

int arraylength(const char *str) {
    int length = 0;
    //Clear length
    while (*str != 0) {
        //Until null character is reached
        str++;
        //Increase array address
        length++;
    }   //Increase length value
    return length;
}   //Return length value

int abs(int a) {
    if (a < 0)
        a = -a;
    return a;
}
