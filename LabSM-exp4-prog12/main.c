#include <msp430.h> 

/*
*   Alunos: Lincoln Abreu Barbosa 140045023
*           Bruno Freitas Feitosa Nunes 120112388
*/

int pwm = 25;
int led_on = 0;
int pwm_ctl=0;
int mode = 0;
float Visible_pwm [25] = {1,10,20,30,41,51,62,74,85,98,110,123,136,149,163,178,193,208,223,240,256,273,291,309,327};

void timerA_setup(void);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    P4DIR |= BIT7;              // Marca o LED em P4.7 como saida.
    P4OUT &= ~BIT7;             // Marca o LED como desligado para comessar certo.
    timerA_setup();              // Configurando TimerA

    pwm_ctl = 327;   // Condição inicial do PWM
    __no_operation();
    __bis_SR_register(LPM0_bits+GIE);  //Ativa a interrupcao e vai pra baixo consumo LPM3
    __no_operation();
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMERA_CCR0_ISR(void){
    if(mode == 0){
        if(pwm<25){    // 0<=PWM<=25
            pwm ++;
        }
        if(pwm == 25){
            mode = 1;
        }
    }else{
        if(pwm>0){      // 0<=PWM<=25
            pwm --;
        }
        if(pwm == 0){
            mode = 0;
        }
    }
    P4OUT &= ~BIT7;
    led_on = 0;
    TA0CTL &= ~TAIFG;
    TA0CCTL0 &= ~BIT0;
    TA0CCTL0 &= ~BIT1;
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMERA_CCR1_ISR(void){
    pwm_ctl = Visible_pwm [pwm-1];
    P4OUT |= BIT7;
    led_on = 1;
    TB0CCR1 = pwm_ctl;
    TA0CTL &= ~TAIFG;
    TA0CCTL1 &= ~BIT0;
    TA0CCTL1 &= ~BIT1;
}

void timerA_setup(){
    TA0CTL |= TACLR;            // Limpa o timer TA.
    TA0CTL |= TASSEL_1;         // Usando ACLK (32,768kHz).
    TA0CTL |= ID_0;             // Divide por 0.
    TA0EX0 |= TAIDEX_0;         // Divide por 0 (extendido).
    TA0CCTL0 |= CCIE;           // Ativa a interrupcao CCR0 do timer A0
    TA0CCR0 = 654;              // Marca o ccr0 como 654, resultando em uma chamada de 50Hz
                                // da rotina (0-100 em 0,5s + 100-0 em 0,5s)

    TA0CCTL1 |= CCIE;
    TA0CCR1 = 327;
    TA0CTL |= MC_1;
}
