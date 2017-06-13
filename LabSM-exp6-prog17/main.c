#include <msp430.h> 

/*
*   Alunos: Lincoln Abreu Barbosa 140045023
*           Bruno Freitas Feitosa Nunes 120112388
*/

float pwm = 100;
float ccr0;
int led_on = 0 ;
float buff;
int S1 = 0, S2 = 0;

void timer_setup(void);
void s1_s2_setup(void);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    P4DIR |= BIT7;              // Marca o LED em P4.7 como saida.
    P4OUT &= ~BIT7;             // Marca o LED como desligado para comessar certo.
    timer_setup();
    s1_s2_setup();
    buff = ccr0*(pwm/100);

    __no_operation();
    __bis_SR_register(LPM0_bits+GIE);           //Ativa a interrupcao e vai pra baixo consumo LPM3
    __no_operation();
}
// Timer1 A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_CCR0_ISR(void)
{
    switch(led_on){
        case 1:
        TA0CCR0 = ccr0-buff;
        led_on = 0;
        break;
        case 0:
        if(pwm == 0)
            buff=1;
        TA0CCR0 = buff;
        led_on = 1;
        break;
    }
    if(pwm == 0)
        P4OUT &= ~BIT7;
    else if(led_on == 0)
        P4OUT &= ~BIT7;
    else if(led_on == 1)
            P4OUT |= BIT7;
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){
    switch (__even_in_range( P1IV, P1IV_P1IFG7 )){
        case P1IV_P1IFG1:
            if (S2==0){
                if(pwm<100){
                    pwm += 5;
                    buff = ccr0*(pwm/100);
                }
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
                if (S1==0){
                   if(pwm>0){
                       pwm -= 5;
                       buff = ccr0*(pwm/100);
                   }
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


void timer_setup(){
    ccr0 = 327;                 // Marca o ccr0 como 327, resultando em uma chamada de 100Hz
                                // da rotina (pwm de 10ms+10ms)
    TA0CTL |= TACLR;            // Limpa o timer TA.
    TA0CTL |= TASSEL_1;         // Usando ACLK (32,768kHz).
    TA0CTL |= ID_0;             // Divide por 1.
    TA0EX0 |= TAIDEX_0;         // Divide por 1 (extendido).
    TA0CCTL0 = CCIE;            // Ativa a interrupcao CCR0 do timer A0
    TA0CCR0 = ccr0;             // Marca o TA0CCR0 como ccr0
    TA0CTL |= MC_1;             // Marca o modo de timer para contar ate TA0CCR0.
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
