#include <msp430.h> 

/*
*   Alunos: Lincoln Abreu Barbosa 140045023
*           Bruno Freitas Feitosa Nunes 120112388
*/

float pwm = 100;
float ccr0;
int led_on = 0 ;
float pwm_ctl;
int S1 = 0, S2 = 0;

void timer_setup(void);
void s1_s2_setup(void);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    P4DIR |= BIT7;              // Marca o LED em P4.7 como saida.
    P4OUT &= ~BIT7;             // Marca o LED como desligado para comessar certo.
    timer_setup();              // Configurando Timer
    s1_s2_setup();              // Configurando Botoes
    pwm_ctl = ccr0*(pwm/100);   // Condição inicial do PWM

    __no_operation();
    __bis_SR_register(LPM0_bits+GIE);  //Ativa a interrupcao e vai pra baixo consumo LPM3
    __no_operation();
}
// Timer1 B0 interrupt service routine
#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER0_CCR0_ISR(void)
{
    switch(led_on){
        case 1:                 // Se o led esta ligado
        TB0CCR0 = ccr0-pwm_ctl;     // Tempo apagado
        led_on = 0;             // Marca como apagado
        break;
        case 0:                 // Se o led esta desligado
        if(pwm == 0)            // Correção de erro de condição (TBCCR0 >= 4)
            pwm_ctl=4;          // Valor Minimo do CCR0
        TB0CCR0 = pwm_ctl;      // Tempo apagado
        led_on = 1;             // Marca o led como ligado
        break;
    }
    if(pwm == 0)
        P4OUT &= ~BIT7;         // PWM 0 o led deve ficar apagado
    else if(led_on == 0)
        P4OUT &= ~BIT7;         // Apaga o led
    else if(led_on == 1)
            P4OUT |= BIT7;      // Acende o led
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void){
    switch (__even_in_range( P1IV, P1IV_P1IFG7 )){
        case P1IV_P1IFG1:
            if (S2==0){
                if(pwm<100){    // 0<=PWM<=100
                    pwm += 5;   // Aumenta em 5%
                    pwm_ctl = ccr0*(pwm/100);   // Ajusta a temporização
                }
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
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void){
    switch (__even_in_range( P2IV, P2IV_P2IFG7 )){
            case P2IV_P2IFG1:
                if (S1==0){
                   if(pwm>0){// 0<=PWM<=100
                       pwm -= 5;   // Diminui em 5%
                       pwm_ctl = ccr0*(pwm/100);   // Ajusta a temporização
                   }
                   S1 = 1;     // Marca botao S1 como apertado
                   P2IES &= ~BIT1;      // Modo de interrupcao entre edge down-up
                   __delay_cycles(5000);   // Debouncing
                   P2IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S2.
                   break;
                }else{
                    S1 = 0;     // Marca botao S1 como solto
                    __delay_cycles(5000);   // Debouncing
                    P2IFG &= ~BIT1;     // Reseta o sinal de interrupcao do S1.
                    P2IES |= BIT1;      // Modo de interrupcao entre edge up-down
                    break;
                }
            default:
                _never_executed();
    }
}


void timer_setup(){
    ccr0 = 327;                 // Marca o ccr0 como 327, resultando em uma chamada de 100Hz
                                // da rotina (pwm de 10ms+10ms)
    TB0CTL |= TBCLR;            // Limpa o timer TB.
    TB0CTL |= TBSSEL_1;         // Usando ACLK (32,768kHz).
    TB0CTL |= ID_0;             // Divide por 1.
    TB0EX0 |= TBIDEX_0;         // Divide por 1 (extendido).
    TB0CCTL0 = CCIE;            // Ativa a interrupcao CCR0 do timer B0
    TB0CCR0 = ccr0;             // Marca o TB0CCR0 como ccr0
    TB0CTL |= MC_1;             // Marca o modo de timer para contar ate TB0CCR0.
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
