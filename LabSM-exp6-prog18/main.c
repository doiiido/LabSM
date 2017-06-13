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
int mode = 0;                   // Mode= 0 => aumentando PWM e mode= 1 =>diminuindo PWM

void timerA_setup(void);
void oscilate (void);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    P4DIR |= BIT7;              // Marca o LED em P4.7 como saida.
    P4OUT &= ~BIT7;             // Marca o LED como desligado para comessar certo.
    timer_setup();              // Configurando Timer
    pwm_ctl = ccr0*(pwm/100);   // Condição inicial do PWM

    __no_operation();
    __bis_SR_register(LPM0_bits+GIE);  //Ativa a interrupcao e vai pra baixo consumo LPM3
    __no_operation();
}
// Timer1 A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_CCR0_ISR(void)
{
    oscilate ();
    switch(led_on){
        case 1:                 // Se o led esta ligado
        TA0CCR0 = ccr0-pwm_ctl;     // Tempo apagado
        led_on = 0;             // Marca como apagado
        break;
        case 0:                 // Se o led esta desligado
        if(pwm == 0)            // Correção de erro de condição (TACCR0 >= 1)
            pwm_ctl=1;          // Valor Minimo do CCR0
        TA0CCR0 = pwm_ctl;      // Tempo apagado
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

void oscilate (){
    if(pwm == 100)
        mode = 1;
    if(pwm == 0)
        mode = 0;
    if(pwm<100 && mode == 0){    // 0<=PWM<=100
        pwm += 5;   // Aumenta em 5%
        pwm_ctl = ccr0*(pwm/100);   // Ajusta a temporização
    }
    if(pwm>0 && mode == 1){// 0<=PWM<=100
       pwm -= 5;   // Diminui em 5%
       pwm_ctl = ccr0*(pwm/100);   // Ajusta a temporização
   }
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
                   if(pwm>0){// 0<=PWM<=100
                       pwm -= 5;   // Diminui em 5%
                       pwm_ctl = ccr0*(pwm/100);   // Ajusta a temporização
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


void timerA_setup(){
    ccr0 = 16384;                 // Marca o ccr0 como 327, resultando em uma chamada de 1Hz
                                // da rotina (pwm de 0,5s+0,5s)
    TA0CTL |= TACLR;            // Limpa o timer TA.
    TA0CTL |= TASSEL_1;         // Usando ACLK (32,768kHz).
    TA0CTL |= ID_0;             // Divide por 1.
    TA0EX0 |= TAIDEX_0;         // Divide por 1 (extendido).
    TA0CCTL0 = CCIE;            // Ativa a interrupcao CCR0 do timer A0
    TA0CCR0 = ccr0;             // Marca o TA0CCR0 como ccr0
    TA0CTL |= MC_1;             // Marca o modo de timer para contar ate TA0CCR0.
}
