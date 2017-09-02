#include <msp430.h> 

/*
*   Alunos: Lincoln Abreu Barbosa 140045023
*           Bruno Freitas Feitosa Nunes 120112388
*/

int S1 = 0, S2 = 0;
int pisca = 0;
int BAKLED1_0;  // Salva o estado do led 1
int BAKLED4_7;  // Salva o estado do led 2
void pisca2();
void led1_led2_setup();
void s1_s2_setup();

void timer_setup();

int main(void) {

        WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer

        timer_setup();              // Configurando Timer
        led1_led2_setup();
        s1_s2_setup();

        __no_operation();
        __bis_SR_register( LPM0_bits+GIE );     // Ativa a interrupcao e vai pra baixo consumo LPM4
        __no_operation();

    return 0;
}

// Timer1 A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_CCR0_ISR(void)
{
    if(pisca){
        P4OUT ^= BIT7;  // Alterna o LED P4.7
        P1OUT ^= BIT0;  // Alterna o LED P1.0
    }
}


#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){
    switch (__even_in_range( P1IV, P1IV_P1IFG7 )){
        case P1IV_P1IFG1:
            if (S2==0){     // Verifica o botao S1 (0=solto 1=apertado)
                if (!pisca){
                    BAKLED4_7 = P4OUT;  // Salva o estado do led 2
                    P4OUT ^= BIT7;  // Alterna o LED P4.7
                }
                S2 = 1;     // Marca botao S2 como apertado
                P1IES &= ~BIT1;      // Modo de interrupcao entre edge down-up
                __delay_cycles(5000);   // Debouncing
                P1IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S2.
                pisca2();
                break;
            }
            else{
                S2 = 0;     // Marca botao S2 como solto
                __delay_cycles(5000);   // Debouncing
                P1IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S2.
                P1IES |= BIT1;      // Modo de interrupcao entre edge up-down
                pisca2();
                break;
            }
       default:
            _never_executed();
    }
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void){
    switch (__even_in_range( P2IV, P2IV_P2IFG7 )){
            case P2IV_P2IFG1:
                if (S1==0){     // Verifica o botao S1 (0=solto 1=apertado)
                   if (!pisca){
                       BAKLED1_0 = P1OUT;  // Salva o estado do led 1
                       P1OUT ^= BIT0;  // Alterna o LED P1.0
                   }
                   S1 = 1;     // Marca botao S1 como apertado
                   P2IES &= ~BIT1;      // Modo de interrupcao entre edge down-up
                   __delay_cycles(5000);   // Debouncing
                   P2IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S2.
                   pisca2();
                   break;
                }
                else{
                    S1 = 0;     // Marca botao S1 como solto
                    __delay_cycles(5000);   // Debouncing
                    P2IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S1.
                    P2IES |= BIT1;      // Modo de interrupcao entre edge up-down
                    pisca2();
                    break;
                }
            default:
                _never_executed();
    }
}

void pisca2(void) {
    if(pisca){
        if(S2==0 && S1==0){      // Interrupt flag S2
            P1OUT = BAKLED1_0;     // Restaura led 1
            P4OUT = BAKLED4_7;     // Restaura led 2
            pisca = 0;
        }
    }else{
        if(S2==1 && S1==1){
            P1OUT &= ~BIT0;         // Marca o LED como desligado para comecar certo.
            P4OUT |= BIT7;          // Marca o LED como ligado para comecar certo.
            pisca = 1;
        }
    }
    return;
}


void led1_led2_setup(){
    P1DIR |= BIT0;          // Marca o LED em P1.0 como saida.
    P1OUT &= ~BIT0;         // Marca o LED como desligado para comecar certo.

    P4DIR |= BIT7;          // Marca o LED em P4.7 como saida.
    P4OUT &= ~BIT7;         // Marca o LED como desligado para comecar certo.
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
void timer_setup(){
    int ccr0 = 16384;                 // Marca o ccr0 como 16384, resultando em uma chamada de 2Hz
                                // da rotina (0,5s aceso+0,5s apagado)
    TA0CTL |= TACLR;            // Limpa o timer TA.
    TA0CTL |= TASSEL_1;         // Usando ACLK (32,768kHz).
    TA0CTL |= ID_0;             // Divide por 1.
    TA0EX0 |= TAIDEX_0;         // Divide por 1 (extendido).
    TA0CCTL0 = CCIE;            // Ativa a interrupcao CCR0 do timer B0
    TA0CCR0 = ccr0;             // Marca o TA0CCR0 como ccr0
    TA0CTL |= MC_1;             // Marca o modo de timer para contar ate TA0CCR0.
}
