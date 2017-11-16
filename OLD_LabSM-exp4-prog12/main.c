#include <msp430.h>

 /*
 *   Alunos: Lincoln Abreu Barbosa 140045023
 *           Bruno Freitas Feitosa Nunes 120112388
 */

 void loop();
 void timerA_setup();

 int main(void) {
     WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
     P4DIR |= BIT7;              // Marca o LED em P1.0 como saida.
     P4OUT &= ~BIT7;             // Marca o LED como desligado para comessar certo.
     timerA_setup();
     //loop();
     __bis_SR_register(LPM0_bits+GIE);
 }

 void timerA_setup(){
     TA0CTL |= TACLR;            // Limpa o timer TA.
     TA0CTL |= TASSEL_1;         // Usando ACLK (32,768kHz).
     TA0CTL |= ID_0;             // Divide por 0.
     TA0EX0 |= TAIDEX_0;         // Divide por 0 (extendido).
     TA0CCTL0 |= CCIE;           // Ativa a interrupcao CCR0 do timer A0
     TA0CCR0 = 16392;            // CCR0 = 16393, resultando em 2 Hz
     TA0CTL |= MC_1;
 }

 #pragma vector=TIMER0_A0_VECTOR
 __interrupt void TIMER0_CCR0_ISR(void)
 {
 // Timer detect TAIFG
 //void loop(){
   //  if(TA0CCTL0&CCIFG == 1 || TA0CTL&TAIFG == 1 || TA0IV == 0xE){
         P4OUT ^= BIT7;                            // Toggle P1.0
     //    TA0CTL &= ~TAIFG;
         //TA0CCTL0&= ~BIT0;
         //TA0CCTL0&= ~BIT1;
     //}
     //loop();
 }

/*int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    P1DIR |= BIT0;              // Marca o LED em P1.0 como saida.
    P1OUT &= ~BIT0;             // Marca o LED como desligado para comessar certo.
    while(1){
        __delay_cycles(305000); //Timer 2HZ
        P1OUT ^= BIT0;          // Toggle P1.0
    }
}*/