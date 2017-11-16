#include "LCD.h"
#include <msp430.h>

void time_delay (int mult);

int main(void) {
    int i;
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    lcd_pins();
    lcd_init();
    while(1){
        for(i = 0; i<=128; i++){
            if(i%32==0&&i!=0){
                time_delay(30000);
                lcd_clr();
            }else if(i%16==0&&i!=0){
                lcd_cursor(0,1);
            }
            if(i<128){
                lcd_char((char)i);
                time_delay(2000);
            }
        }
    }
    return(0);
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_CCR0_ISR(void)
{
    TA0CTL = MC_0;
    TA0CTL = TACLR;            // Limpa o timer TA.
    TA0CCTL0 &= ~CCIE;            // Ativa a interrupcao CCR0 do timer A0
    __bic_SR_register_on_exit(LPM0_bits+GIE);
    //Desativa a interrupcao ao sair
}

void timerA_setup(){
    TA0CTL |= TACLR;            // Limpa o timer TA.
    TA0CTL |= TASSEL_1;         // Usando ACLK (32,768kHz).
    TA0CTL |= ID_0;             // Divide por 1.
    TA0EX0 |= TAIDEX_2;         // Divide por 3 (extendido) (11khz).
    TA0CCTL0 |= CCIE;            // Ativa a interrupcao CCR0 do timer A0
}

void time_delay (int mult){
    int ccr0 = 1;   //0,1 ms
    timerA_setup();
    TA0CCR0 = ccr0 * mult;
    TA0CTL |= MC_1;
    __bis_SR_register(LPM0_bits+GIE);
    //Ativa a interrupcao e vai pra baixo consumo LPM0
}


