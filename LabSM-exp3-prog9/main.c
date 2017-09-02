#include <msp430.h>

/*
*   Alunos: Lincoln Abreu Barbosa 140045023
*           Bruno Freitas Feitosa Nunes 120112388
*/
void s2_setup();
void led1_led2_setup();

int S1 = 0, S2 = 0;
int LSB=0;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    led1_led2_setup();          // Configurando LEDs
    s2_setup();              // Configurando Botoes

    __no_operation();
    __bis_SR_register(LPM4_bits+GIE);  //Ativa a interrupcao e vai pra baixo consumo LPM4
    __no_operation();
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){
    switch (__even_in_range( P1IV, P1IV_P1IFG7 )){
        case P1IV_P1IFG1:
            if (S2==0){
                if (LSB == 1){
                    P1OUT ^= BIT0;      // Alterna o led vermelho
                }
                P4OUT ^= BIT7;      // Alterna o led verde
                LSB ^= 1;
                S2 = 1;     // Marca botao S2 como apertado
                P1IES &= ~BIT1;      // Modo de interrupcao entre edge down-up
                __delay_cycles(5000);   // Debouncing
                P1IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S2.
                break;
            }else{
                S2 = 0;     // Marca botao S2 como solto
                __delay_cycles(5000);   // Debouncing
                P1IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S2.
                P1IES |= BIT1;      // Modo de interrupcao entre edge up-down
                break;
            }
        default:
            _never_executed();
    }
}

void s2_setup(){
    P1DIR &= ~BIT1;         // Marca o pino P1.1 como entrada para S2.
    P1REN |= BIT1;          // Marca a entrada do pino P1.1 com resistor.
    P1OUT |= BIT1;          // Marca o resistor da entrada P1.1 como pullup.

    P1IE |= BIT1;           // Ativa a interrupcao do S2.
    P1IES |= BIT1;          // Modo de interrupcao para edge up-down
    P1IFG &= ~BIT1;         // Limpa flag de interrupcao
}

void led1_led2_setup(){
    P1DIR |= BIT0;          // Marca o LED em P1.0 como saida.
    P1OUT &= ~BIT0;         // Marca o LED como desligado para comecar certo.

    P4DIR |= BIT7;          // Marca o LED em P4.7 como saida.
    P4OUT &= ~BIT7;         // Marca o LED como desligado para comecar certo.
}
