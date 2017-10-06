#include <msp430.h>
#include "clock.h"
#include "pmm.h"

/*
*   Alunos: Lincoln Abreu Barbosa 140045023
*           Bruno Freitas Feitosa Nunes 120112388
*/
void P1_2_setup(void);
void timerA_setup();
void time_delay (int mult);

volatile int timecount = 0;
volatile int error = 0;
volatile int send = 0;
volatile int start=0;
volatile double Time=0;

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    pmmVCore(1);
    pmmVCore(2);                // Mudando os clocks
    clockInit();
    P1_2_setup();
    timerA_setup();
    __bis_SR_register(LPM0_bits+GIE);

}

void P1_2_setup(void){
    //emiter
    P2DIR |= BIT4;         // Marca o pino P2.4 como saida.
    //P2DS |= BIT4;          // Drive Strength high
    P2OUT &= ~BIT4;         // LOW

    //reciever
    P1DIR &= ~BIT2;         // Marca o pino P1.2 como entrada.
    P1REN |= BIT2;          // Marca a entrada do pino P1.2 com resistor.
    P1OUT |= BIT2;          // Marca o resistor da entrada P1.2 como pullup.
    P1SEL |= BIT2;           //
    P1IES &= ~BIT2;         // Modo de interrupcao entre edge down-up

}

void timerA_setup(){
    TA0CTL |= TACLR;            // Limpa o timer TA.
    TA0CTL |= TASSEL_2;         // Usando SMCLK (1MHz)(1/u).
    TA0CTL |= ID_0;             // Divide por 0.
    TA0EX0 |= TAIDEX_0;         // Divide por 0 (extendido).
    TA0CTL |= MC_1;             // Marca o modo de timer para contar ate TA0CCR0.
    TA0CCTL1 |= CM_1;            //
    TA0CCTL1 |= CCIS_0;            //
    TA0CCTL1 |= SCS;            //
    TA0CCTL1 |= CAP;            //
    TA0CCTL1 |= CCIE;            //

    TA0CCTL0 |= CCIE;            // Ativa a interrupcao CCR0 do timer A0
    TA0CCR0 = 9;               // Marca o TA0CCR0 como 2, resultando em uma chamada de 0,1Mhz(1/10u)
}

void END(){
    Time = (double)((double)timecount)/(double)100;
    TA0CTL |= TACLR;            // Limpa o timer TA.
    while(1);
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_CCRN_ISR(void){
    if((TA0CCTL1 & CCIFG) == 1){
        start^=1;
        P1OUT &= ~BIT2;          // Marca o resistor da entrada P1.2 como pulldown.
        TA0CCTL1 &= ~CM_1;            //
        TA0CCTL1 |= CM_2;            //
        TA0CCTL1 &= ~CCIFG;
    }
}


#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_CCR0_ISR(void){
    switch (send){
            case 0:
                send = 1;
                P2OUT |= BIT4;
                break;
            case 1:
               send = 2;
               P2OUT &= ~BIT4;
               break;
    }
    if(start == 1){
        if(timecount < 0xEFFF){
           timecount++;
       }else{
           error=1;
           while(1);
       }
    }
    if(timecount > 0 && start == 0)
        END();
    TA0CCTL0 &= ~CCIFG;
}
