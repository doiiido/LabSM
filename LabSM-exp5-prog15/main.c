#include <msp430.h>

/*
 * main.c
 */

void P2_setup(void);
void s1_s2_setup(void);
void UART_setup(void);
void time_delay(int mult);
void led_setup(void);

char word_S1= 0x0, word_S2 = 0x0;
int S1 = 0, S2 = 0;
int bit = 0, paridade = 0;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    P2_setup();
    s1_s2_setup();
    UART_setup();
    led_setup();
    __bis_SR_register(LPM0_bits+GIE);
    return 0;
}


//emissor

void word_send(char word){
    UCA0TXBUF = word;
}

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void){   //Check if the UCA0RXBUF is different from 0x0A
//(Enter key from keyboard)
    if(UCA0RXBUF == 0x01){
        P1OUT |= BIT0;             // Marca o LED vermelho como ligado.
    }else if(UCA0RXBUF == 0x0A){
        P1OUT &= ~BIT0;             // Marca o LED vermelho como desligado.
    }
    if(UCA0RXBUF == 0x10){
        P4OUT |= BIT7;             // Marca o LED verde como ligado.
    }else if(UCA0RXBUF == 0xA0){
        P4OUT &= ~BIT7;             // Marca o LED verde como desligado.
    }
}

void led_setup(void){
    P1DIR |= BIT0;              // Marca o LED em P1.0 como saida (vermelho).
    P1OUT &= ~BIT0;             // Marca o LED como desligado para comessar certo.
    P4DIR |= BIT7;              // Marca o LED em P4.7 como saida (verde).
    P4OUT &= ~BIT7;             // Marca o LED como desligado para comessar certo.
}

//delay do debouncing
void time_delay (int mult){
    int i = 0;
    int ccr0 = 2 * mult;   //ccr0 ~=0,1 ms
    for(i = 0; i<ccr0; i++){
        __no_operation();
    }
}

void UART_setup(void){
    P3SEL = BIT3 + BIT4;                        // P3.3,4 = USCI_A0 TXD/RXD
       UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
       UCA0CTL1 |= UCSSEL_2;                     // SMCLK
       UCA0BR0 = 54;                              // 1MHz 1200 (see User's Guide)
       UCA0BR1 = 0;                              // 1MHz 1200
       UCA0MCTL = UCBRS_0 + UCBRF_13 + UCOS16;   // Modln UCBRSx=0, UCBRFx=0,
   // over sampling
       UCA0CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**
       UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
}

void P2_setup(void){
    //emiter
    P2DIR |= BIT4;         // Marca o pino P2.4 como saida para S2.
    //P2DS |= BIT4;          // Drive Strength high
    P2OUT |= BIT4;         // High
}

void s1_s2_setup(){
    P1DIR &= ~BIT1;         // Marca o pino P1.1 como entrada para S2.
    P1REN |= BIT1;          // Marca a entrada do pino P1.1 com resistor.
    P1OUT |= BIT1;          // Marca o resistor da entrada P1.1 como pullup.

    P2DIR &= ~BIT1;         // Marca o pino P2.1 como entrada para S1.
    P2REN |= BIT1;          // Marca a entrada do pino P2.1 com resistor.
    P2OUT |= BIT1;          // Marca o resistor da entrada P2.1 como pullup.

    P2IE |= BIT1;           // Ativa a interrupcao do S1.
    P2IES |= BIT1;          // Modo de interrupcao para edge up-down
    P2IFG &= ~BIT1;         // Limpa flag de interrupcao

    P1IE |= BIT1;           // Ativa a interrupcao do S2.
    P1IES |= BIT1;          // Modo de interrupcao para edge up-down
    P1IFG &= ~BIT1;         // Limpa flag de interrupcao
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){
    switch (__even_in_range( P1IV, P1IV_P1IFG7 )){
        case P1IV_P1IFG1:
            time_delay(100);   // Debouncing
            P1IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S2.
            if (S2==0){
                if(word_S2 == 0x10){    // palavra 1
                    word_S2= 0xA0;            // Palavra2.
                }else{
                    word_S2= 0x10;
                }
                word_send(word_S2);
                S2 = 1;     // Marca botao S2 como apertado
                P1IES &= ~BIT1;      // Modo de interrupcao entre edge down-up
                break;
            }else
                S2 = 0;     // Marca botao S2 como solto
            P1IES |= BIT1;      // Modo de interrupcao entre edge up-down
            break;
        default:
            _never_executed();
    }
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void){
    switch (__even_in_range( P2IV, P2IV_P2IFG7 )){
        case P2IV_P2IFG1:
            time_delay(100);   // Debouncing
            P2IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S1.
            if (S1==0){
                if(word_S1 == 0x01){    // palavra 1
                    word_S1= 0x0A;            // Palavra2.
                }else{
                    word_S1= 0x01;
                }
                word_send(word_S1);
                S1 = 1;     // Marca botao S1 como apertado
                P2IES &= ~BIT1;      // Modo de interrupcao entre edge down-up
                break;
            }else
                S1 = 0;     // Marca botao S1 como solto
            P2IES |= BIT1;      // Modo de interrupcao entre edge up-down
            break;
        default:
            _never_executed();
    }
}
