#include <msp430.h> 

/*
*   Alunos: Lincoln Abreu Barbosa 140045023
*           Bruno Freitas Feitosa Nunes 120112388
*/

float pwm = 0;

float TAccr0_default = 655;       // Marca o ccr0 como 655, resultando em uma chamada de 50Hz
                                  // da rotina (0-100 em 0,5s + 100-0 em 0,5s)

float TBccr0_default = 327;       // Marca o ccr0 como 327, resultando em uma chamada de 100Hz;
                                  // da rotina (pwm de 10ms+10ms)
int led_on = 0 ;
float pwm_ctl;
int mode = 0;                   // Mode= 0 => aumentando PWM e mode= 1 =>diminuindo PWM
float Visible_pwm [25] = {0,10,20,30,41,51,62,74,85,98,110,123,136,149,163,178,193,208,223,240,256,273,291,309,327};


void timerA_setup(void);
void timerB_setup(void);
void core_initialize(void);
#define FLLN(x) (x-1)
void oscilate (void);

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    P4DIR |= BIT7;
    P4SEL |= BIT7;
    // Port Mapping
    PMAPKEYID = 0x02D52;    // Escritas no modulo de mapeamento são protegidas
                            // por senha. Escrever 2D52 no registro PMAPKEYID
                            // permite reconfiguração de apenas uma porta.
    P4MAP7 = PM_TB0CCR1A;   // Conecta o pino P4.7 na saída do canal 1
                            // do Timer B.
    timerA_setup();              // Configurando Timer A
    timerB_setup();              // Configurando Timer B
    core_initialize ();         // Mudando os clocks para melhorar sincronia

    pwm_ctl = TBccr0_default*(pwm/100);   // Condição inicial do PWM

    __no_operation();
    __bis_SR_register(LPM0_bits+GIE);  //Ativa a interrupcao e vai pra baixo consumo LPM0
    __no_operation();
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0A_CCR0_ISR(void)
{
    if(pwm == 100)
        mode = 1;
    if(pwm == 0)
        mode = 0;
    if(pwm<100 && mode == 0){    // 0<=PWM<=100
        pwm += 4;   // Aumenta em 4%
        pwm_ctl = Visible_pwm [(int)(pwm/4)-1];   // Ajusta a temporização
    }
    if(pwm>0 && mode == 1){// 0<=PWM<=100
        pwm -= 4;   // Diminui em 4%
        pwm_ctl = Visible_pwm [(int)(pwm/4)];   // Ajusta a temporização
    }
    TB0CCR1 = pwm_ctl;
}

void timerA_setup(){
    TA0CTL |= TACLR;            // Limpa o timer TA.
    TA0CTL |= TASSEL_1;         // Usando ACLK (32,768kHz).
    TA0CTL |= ID_0;             // Divide por 1.
    TA0EX0 |= TAIDEX_0;         // Divide por 1 (extendido).
    TA0CCTL0 = CCIE;            // Ativa a interrupcao CCR0 do timer B0
    TA0CCR0 = TAccr0_default;   // Marca o TA0CCR0 como ccr0
    TA0CTL |= MC_1;             // Marca o modo de timer para contar ate TA0CCR0.
}

void timerB_setup(){
    TB0CTL |= TBCLR;            // Limpa o timer TB.
    TB0CTL |= TBSSEL_1;         // Usando ACLK (32,768kHz).
    TB0CTL |= ID_0;             // Divide por 1.
    TB0EX0 |= TBIDEX_0;         // Divide por 1 (extendido).
    TB0CCTL1 = OUTMOD_3;
    TB0CCR0 = TBccr0_default;   // Marca o TB0CCR0 como ccr0
    TB0CTL |= MC_1;             // Marca o modo de timer para contar ate TB0CCR0.
}

void core_initialize(void){
    //Configure clock system
    UCSCTL0 = DCO4_H | DCO2_H;              //SET DCO = 18

    UCSCTL1 = DCORSEL_3;                    // Select DCO range to 0.25MHz to 2,5MHz

    UCSCTL2 = FLLD__16 | FLLN(121);          // Loop multipliers: (16) * (121 + 1) * 32.768 / 12 = 5,330MHz

    UCSCTL3 = SELREF__XT1CLK  |             // Set DCO FLL reference = XT1CLK
              FLLREFDIV__12;                // XT1CLK 32,768/12 = 2,730666kHz

    UCSCTL4 = SELM__DCOCLK;                 // MCLK  = DCOCLK

    UCSCTL5 = DIVM__1;                      // MCLK  DCOCLK/1 = 5,330MHz

    UCSCTL6 &= ~XT1OFF;                      // Garante que o XT1 esta ligado
}
