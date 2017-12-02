#include "ADC12A.h"
#include <msp430.h>
/*
 * ADC12A.c
 *
 *  Created on: 1 de dez de 2017
 *      Author: lincoln
 */


#define ADCMEM0ADDR (__SFR_FARPTR) 0x0720;
#define ADCMEM1ADDR (__SFR_FARPTR) 0x0722;

void setPinos(){
    // pinos ADC
    P6SEL |= BIT1 | BIT2;
    P6DIR &= ~BIT1 & ~BIT2;
}

void setTimerA(){
    TA0CTL = (TASSEL__SMCLK |                   // SMCLK = 1MHz
                  ID_3         |                   // Dividir por 8: 1MHz/8 = 131kHz
                  MC__UP);                          // Modo UP
    TA0EX0 |= TAIDEX_7;                         // 131/8 = 16khz
    TA0CCR0 = 8192;                            // 2Hz, 0.5s
    TA0CCTL1 = OUTMOD_3;                        // Modo toogle/set
    TA0CCR1 = 8190;                              //2Hz, 0.5s
//    TA0CCTL0 = CCIE;            // Ativa a interrupcao CCR0 do timer A0
}

void setADC(){
    setPinos();
    setTimerA();
    ADC12CTL0 &= ~ADC12ENC;   // desabilita - permitindo edição
    ADC12CTL0 &= ~ADC12MSC;     // multiple sample convertion
    ADC12CTL0 |= ADC12SHT0_0
              | ADC12ON       // habilita ADC
              | 0;
    ADC12CTL1 |= ADC12SHS_1   // timer do ADC TA0.1
              | ADC12SHP      // SAMPCON timerADC12SHP
              | ADC12DIV_0     // Divisor = 1
              | ADC12SSEL_3    // SMCLK
              | ADC12DIV_7      // SMCLK %8
              | ADC12CONSEQ_3 // repeat sequence of channels
              | 0;

    ADC12CTL2 |= ADC12TCOFF   // internal T sensor off
              //| ADC12RES_2
              | 0;

    ADC12MCTL0 |= ADC12INCH0
               | 0; // P6.1

    ADC12MCTL1 |= ADC12INCH1  // P6.1
               | ADC12EOS     // ultimo canal
               | 0;

    ADC12CTL0 |= ADC12ENC;    // habilita ADC
}

void setDMA(unsigned short *saida_0, unsigned short *saida_1){
    DMACTL0 = DMA0TSEL_24     // DMA 0 ADC12IFGx
            | DMA1TSEL_24     // DMA 1 ADC12IFGx
            | 0;

    DMA0CTL |= DMAEN          // habilita DAM0
            |  DMADT_5        // repeated single transfer
            |  DMADSTINCR_0   // add dst unchanged
            |  DMASRCINCR_0   // add src unchanged
            |  0;

    DMA1CTL |= DMAEN          // habilita DAM1
            |  DMADT_5        // repeated block transfer
            |  DMADSTINCR_0   // add dst unchanged
            |  DMASRCINCR_0   // add src unchanged
            |  0;

    DMA0SA = ADCMEM0ADDR;
    DMA1SA = ADCMEM1ADDR;

    DMA0DA = saida_0;
    DMA1DA = saida_1;

    DMA0SZ = 1;
    DMA1SZ = 1;
}
