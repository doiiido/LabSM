#include <msp430.h> 

/*
*   Alunos: Lincoln Abreu Barbosa 140045023
*           Bruno Freitas Feitosa Nunes 120112388
*/

int S1 = 0, S2 = 0;
void pisca2(void);

int main(void) {

        WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer

        P1DIR |= BIT0;          // Marca o LED em P1.0 como saida.
        P1OUT &= ~BIT0;         // Marca o LED como desligado para comecar certo.

        P4DIR |= BIT7;          // Marca o LED em P4.7 como saida.
        P4OUT &= ~BIT7;         // Marca o LED como desligado para comecar certo.

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


        __no_operation();
        __bis_SR_register( LPM4_bits+GIE );     // Ativa a interrupcao e vai pra baixo consumo LPM4
        __no_operation();

    return 0;
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){
    switch (__even_in_range( P1IV, P1IV_P1IFG7 )){
        case P1IV_P1IFG1:
            if (S1==1){     // Verifica o botao S1 (0=solto 1=apertado)
                S2 = 1;     // Marca botao S2 como apertado
                P1IES &= ~BIT1;      // Modo de interrupcao entre edge down-up
                P1IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S2.
                __delay_cycles(5000);   // Debouncing
                pisca2();       // Rotina pra piscar os leds
                break;
            }
            if (S2==0){
                P4OUT ^= BIT7;  // Alterna o LED P4.7
                S2 = 1;     // Marca botao S2 como apertado
                P1IES &= ~BIT1;      // Modo de interrupcao entre edge down-up
                __delay_cycles(5000);   // Debouncing
                P1IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S2.
                break;
            }
            else
                S2 = 0;     // Marca botao S2 como solto
            __delay_cycles(5000);   // Debouncing
            P1IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S2.
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
                if (S2==1){    // Verifica o botao S2 (0=solto 1=apertado)
                    S1 = 1;     // Marca botao S1 como apertado
                    P2IES &= ~BIT1;      // Modo de interrupcao entre edge down-up
                    __delay_cycles(5000);   // Debouncing
                    P2IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S2.
                    pisca2();      // Rotina pra piscar os leds
                    break;
                }
                if (S1==0){
                   P1OUT ^= BIT0;  // Alterna o LED P1.0
                   S1 = 1;     // Marca botao S1 como apertado
                   P2IES &= ~BIT1;      // Modo de interrupcao entre edge down-up
                   __delay_cycles(5000);   // Debouncing
                   P2IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S2.
                   break;
                }
                else
                    S1 = 0;     // Marca botao S1 como solto

                __delay_cycles(5000);   // Debouncing
                P2IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S1.
                P2IES |= BIT1;      // Modo de interrupcao entre edge up-down
                break;
            default:
                _never_executed();
    }
}

void pisca2(void) {
    int BAKLED1_0 = P1OUT;  // Salva o estado do led 1
    int BAKLED4_7 = P4OUT;  // Salva o estado do led 2
    int BaKP1IFG;           // Variavel pra checar mudança de estado do botao S2
    int BaKP2IFG;           // Variavel pra checar mudança de estado do botao S1

    P1OUT &= ~BIT0;         // Marca o LED como desligado para comecar certo.
    P4OUT |= BIT7;          // Marca o LED como ligado para comecar certo.

    BaKP1IFG = P1IFG;   // salva o estado do botao S2 sem interrupcao
    BaKP2IFG = P2IFG;   // salva o estado do botao S1 sem interrupcao
    for(;;){
        __delay_cycles(305000);     //Timer 2HZ
        P1OUT ^= BIT0;      // Alterna o LED P1.0
        P4OUT ^= BIT7;      // Alterna o LED P4.7
        if(BaKP1IFG != P1IFG){      // Interrupt flag S2
            __delay_cycles(5000);   // Debouncing
            if(S1==0 && S2 == 1){      // Se S1 esta solto e soltei o S2
                P1OUT = BAKLED1_0;     // Restaura led 1
                P4OUT = BAKLED4_7;     // Restaura led 2
                P1IFG &= ~BIT1;        // Limpa flag
                P1IES |= BIT1;      // Modo de interrupcao entre edge up-down
                S2 = 0;     // Marca S2 como solto
                break;
            }else{
                P1IFG &= ~BIT1;        // Limpa flag
                if (S2 == 1){
                    P1IES |= BIT1;      // Modo de interrupcao entre edge up-down
                    S2 = 0;     // Marca S2 como solto
                }else{
                    P1IES &= ~BIT1;      // Modo de interrupcao entre edge down-up
                    S2 = 1;     // Marca S2 como apertado
                }
            }
        }
        else if(BaKP2IFG != P2IFG){
            __delay_cycles(5000);   // Debouncing
            if(S2==0 && S1==1){      // Se S2 esta solto e soltei o S1
                P1OUT = BAKLED1_0;     // Restaura led 1
                P4OUT = BAKLED4_7;     // Restaura led 2
                P2IFG &= ~BIT1;        // Limpa flag
                P2IES |= BIT1;      // Modo de interrupcao entre edge up-down
                S1 = 0;     // Marca S1 como solto
                break;
            }else{
                P2IFG &= ~BIT1;        // Limpa flag
                if (S1 == 1){
                    P2IES |= BIT1;      // Modo de interrupcao entre edge up-down
                    S1 = 0;     // Marca S1 como solto
                }else{
                    P2IES &= ~BIT1;      // Modo de interrupcao entre edge down-up
                    S1 = 1;     // Marca S1 como apertado
                }
            }
        }
    }
    return;
}
