#include <msp430.h> 

/*
*   Alunos: Lincoln Abreu Barbosa 140045023
*           Bruno Freitas Feitosa Nunes 120112388
*/

float pwm = 0;
float TAccr0, TBccr0;
int led_on = 0 ;
float pwm_ctl;
int S1 = 0, S2 = 0;
int mode = 0;                   // Mode= 0 => aumentando PWM e mode= 1 =>diminuindo PWM

void timerA_setup(void);
void timerB_setup(void);
void oscilate (void);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    P4DIR |= BIT7;              // Marca o LED em P4.7 como saida.
    P4OUT &= ~BIT7;             // Marca o LED como desligado para comessar certo.
    timerA_setup();              // Configurando Timer A
    timerB_setup();              // Configurando Timer B
    pwm_ctl = TAccr0*(pwm/100);   // Condição inicial do PWM

    __no_operation();
    __bis_SR_register(LPM0_bits+GIE);  //Ativa a interrupcao e vai pra baixo consumo LPM3
    __no_operation();
}
// Timer1 A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0A_CCR0_ISR(void)
{
    switch(led_on){
        case 1:                 // Se o led esta ligado
        TA0CCR0 = TAccr0-pwm_ctl;     // Tempo apagado
        led_on = 0;             // Marca como apagado
        break;
        case 0:                 // Se o led esta desligado
        if(pwm <= 4)            // Correção de erro de condição (TACCR0 >= 4)
            pwm_ctl=4;          // Valor Minimo do CCR0
        TA0CCR0 = pwm_ctl;      // Tempo apagado
        led_on = 1;             // Marca o led como ligado
        break;
    }
    if(pwm <= 4)
        P4OUT &= ~BIT7;         // PWM 0 o led deve ficar apagado
    else if(led_on == 0)
        P4OUT &= ~BIT7;         // Apaga o led
    else if(led_on == 1)
            P4OUT |= BIT7;      // Acende o led
}

#pragma vector=TIMER0_B0_VECTOR
__interrupt void TIMER0_CCR0_ISR(void)
{
    if(pwm == 100)
        mode = 1;
    if(pwm == 0)
        mode = 0;
    if(pwm<100 && mode == 0){    // 0<=PWM<=100
        pwm += 4;   // Aumenta em 4%
        pwm_ctl = TAccr0*(pwm/100);   // Ajusta a temporização
    }
    if(pwm>0 && mode == 1){// 0<=PWM<=100
       pwm -= 4;   // Diminui em 4%
       pwm_ctl = TAccr0*(pwm/100);   // Ajusta a temporização
   }
}

void timerB_setup(){
    TBccr0 = 655;                // Marca o ccr0 como 655, resultando em uma chamada de 50Hz
                                // da rotina (0-100 em 0,5s + 100-0 em 0,5s)
    TB0CTL |= TBCLR;            // Limpa o timer TB.
    TB0CTL |= TBSSEL_1;         // Usando ACLK (32,768kHz).
    TB0CTL |= ID_0;             // Divide por 1.
    TB0EX0 |= TBIDEX_0;         // Divide por 1 (extendido).
    TB0CCTL0 = CCIE;            // Ativa a interrupcao CCR0 do timer B0
    TB0CCR0 = TBccr0;             // Marca o TB0CCR0 como ccr0
    TB0CTL |= MC_1;             // Marca o modo de timer para contar ate TB0CCR0.
}

void timerA_setup(){
    TAccr0 = 327;                 // Marca o ccr0 como 327, resultando em uma chamada de 100Hz
                                // da rotina (pwm de 10ms+10ms)
    TA0CTL |= TACLR;            // Limpa o timer TA.
    TA0CTL |= TASSEL_1;         // Usando ACLK (32,768kHz).
    TA0CTL |= ID_0;             // Divide por 1.
    TA0EX0 |= TAIDEX_0;         // Divide por 1 (extendido).
    TA0CCTL0 = CCIE;            // Ativa a interrupcao CCR0 do timer A0
    TA0CCR0 = TAccr0;             // Marca o TA0CCR0 como ccr0
    TA0CTL |= MC_1;             // Marca o modo de timer para contar ate TA0CCR0.
}
