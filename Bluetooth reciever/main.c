#include <msp430.h> 

/*
 * main.c
 */

void P2_setup(void);
void s1_s2_setup(void);
void baud_delay(void);
void baud_half_delay(void);
void time_delay(int mult);

int ccr0 = 3;   // alternando 3 e 4 dá aprox =~ 9600 hz erro de 2,5%
char word_S1= 0x0, word_S2 = 0x0;
int S1 = 0, S2 = 0;
int bit = 0, paridade = 0;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    P2_setup();
    s1_s2_setup();
    __bis_SR_register(LPM0_bits+GIE);
    return 0;
}


//emissor

void word_send(char word){
    char bit[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
    int i, parity = 0;

    bit[0] = word & 0x1;
    bit[1] = (word & 0x2) >> 1;
    bit[2] = (word & 0x4) >> 2;
    bit[3] = (word & 0x8) >> 3;
    bit[4] = (word & 0x10) >> 4;
    bit[5] = (word & 0x20) >> 5;
    bit[6] = (word & 0x40) >> 6;
    bit[7] = (word & 0x80) >> 7;

    P2OUT &= ~BIT4;         // Low
    baud_delay();
    for(i=0;i<8;i++){
        parity ^= bit[i];
        if(bit[i] == 0x0){
            P2OUT &= ~BIT4;     //Low
        }else{
            P2OUT |= BIT4;      // High
        }
        baud_delay();
    }
    /*if(parity == 0x0){
        P2OUT &= ~BIT4;     //Low
    }else{
        P2OUT |= BIT4;      // High
    }
    baud_delay();*/
    P2OUT |= BIT4;          //Stop Bit
}

//receptor

char listen(void){
    char bit[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
    int i, parity = 0;
    char word = 0x0;
    baud_half_delay();
    for(i=0;i<8;i++){
        bit[i] = (P2IN & 0x20)>>5; // lendo o p2.5
        parity ^= bit[i];
        baud_delay();
    }
    /*if(parity != P2IN & 0x20);
      //Error
    }
    baud_delay();*/
    for(i=0;i<8;i++){
        word |= bit[i]<<i;
    }
    return word;
}

void Port_2_5(void){
    char word;
    time_delay(5);   // Debouncing
    P2IE &= ~BIT5;           // Desativa a interrupcao do 2.5 (sincronizando os clocks).
    word = listen();
    if(word == 0x01){
        P1OUT |= BIT0;             // Marca o LED vermelho como ligado.
    }else if(word == 0x0A){
        P1OUT &= ~BIT0;             // Marca o LED vermelho como desligado.
    }
    if(word == 0x10){
        P4OUT |= BIT7;             // Marca o LED verde como ligado.
    }else if(word == 0xA0){
        P4OUT &= ~BIT7;             // Marca o LED verde como desligado.
    }
    P2IE |= BIT5;           // Ativa a interrupcao do 2.5.
    P2IFG &= ~BIT5;     // Reseta o sinal de interrupcao do 2.5.
}

//delay do debouncing
void time_delay (int mult){
    int i = 0;
    int ccr0 = 2 * mult;   //ccr0 ~=0,1 ms
    for(i = 0; i<ccr0; i++){
        __no_operation();
    }
}

void P2_setup(void){
    //emiter
    P2DIR |= BIT4;         // Marca o pino P2.4 como saida para S2.
    //P2DS |= BIT4;          // Drive Strength high
    P2OUT |= BIT4;         // High
    //reciever
    P2DIR &= ~BIT5;         // Marca o pino P2.5 como entrada para S1.
    P2REN |= BIT5;          // Marca a entrada do pino P2.5 com resistor.
    P2OUT |= BIT5;          // Marca o resistor da entrada P2.5 como pullup.
    P2IE |= BIT5;           // Ativa a interrupcao do S1.
    P2IES |= BIT5;          // Modo de interrupcao para edge up-down
    P2IFG &= ~BIT5;         // Limpa flag de interrupcao
}

void led_setup(void){
    P1DIR |= BIT0;              // Marca o LED em P4.7 como saida (vermelho).
    P1OUT &= ~BIT0;             // Marca o LED como desligado para comessar certo.
    P4DIR |= BIT7;              // Marca o LED em P4.7 como saida (verde).
    P4OUT &= ~BIT7;             // Marca o LED como desligado para comessar certo.

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
            time_delay(5);   // Debouncing
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
            time_delay(5);   // Debouncing
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
        //receiver
        case P2IV_P2IFG5:
            Port_2_5();
            break;
        default:
            _never_executed();
    }
}


// clock
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_CCR0_ISR(void)
{
    TA0CTL = MC_0;
    TA0CTL = TACLR;            // Limpa o timer TA.
    TA0CCTL0 &= ~CCIE;            // Ativa a interrupcao CCR0 do timer A0
    if(ccr0 == 3){
        ccr0 = 4;   //alternando ccr0 entre 3 e 4
    }else
        ccr0 = 3;
    __bic_SR_register_on_exit(LPM0_bits+GIE);
    //Desativa a interrupcao ao sair
}

void timerA_setup(){
    TA0CTL |= TACLR;            // Limpa o timer TA.
    TA0CTL |= TASSEL_1;         // Usando ACLK (32,768kHz).
    TA0CTL |= ID_0;             // Divide por 1.
    TA0EX0 |= TAIDEX_0;         // Divide por 1 (extendido).
    TA0CCTL0 |= CCIE;            // Ativa a interrupcao CCR0 do timer A0
}

void baud_delay (void){
    timerA_setup();
    TA0CCR0 = ccr0;
    TA0CTL |= MC_1; // up to ccr0
    __bis_SR_register(LPM0_bits+GIE);
    //Ativa a interrupcao e vai pra baixo consumo LPM0
}

//receiver
void baud_half_delay (void){
    timerA_setup();
    TA0CCR0 = 2;
    TA0CTL |= MC_1; // up to ccr0
    __bis_SR_register(LPM0_bits+GIE);
    //Ativa a interrupcao e vai pra baixo consumo LPM0
}
