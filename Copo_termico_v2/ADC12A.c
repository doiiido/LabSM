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
#define ADCMEM2ADDR (__SFR_FARPTR) 0x0724;

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

    REFCTL0 &= ~REFMSTR;       //ADC12 controls the REF module

    ADC12CTL0 &= ~ADC12ENC;   // desabilita - permitindo edição
    ADC12CTL0 &= ~ADC12MSC;     // multiple sample convertion
    ADC12CTL0 |= ADC12SHT0_0
              | ADC12REFON
              | ADC12ON       // habilita ADC
              | 0;
    ADC12CTL1 |= ADC12SHS_1   // timer do ADC TA0.1
              | ADC12SHP      // SAMPCON timerADC12SHP
              | ADC12SSEL_3    // SMCLK
              | ADC12PDIV       // SMCLK %4
              | ADC12DIV_7      // SMCLK %8 = %32
              | ADC12CONSEQ_3 // repeat sequence of channels
              | 0;

    ADC12CTL2 &= ~ADC12TCOFF;   // internal T sensor on

    ADC12CTL2 |= ADC12RES_2
              | 0;

    ADC12MCTL0 |= ADC12INCH_1
               | 0; // P6.1

    ADC12MCTL1 |= ADC12INCH_2  // P6.1
               | 0;

    ADC12MCTL2 |= ADC12INCH_10  // internal T
                   | ADC12EOS     // ultimo canal
                   | 0;

    ADC12CTL0 |= ADC12ENC;    // habilita ADC
}

void setDMA(unsigned short *saida_6_1, unsigned short *saida_6_2, unsigned short *saida_t_amb){
    DMACTL0 = DMA0TSEL_24     // DMA 0 ADC12IFGx
            | DMA1TSEL_24     // DMA 1 ADC12IFGx
            | 0;
    DMACTL1 |= DMA2TSEL_24;   // DMA 2 ADC12IFGx

    DMA0CTL |= DMAEN          // habilita DMA0
            |  DMADT_5        // repeated block transfer
            |  DMADSTINCR_0   // add dst unchanged
            |  DMASRCINCR_0   // add src unchanged
            |  0;

    DMA1CTL |= DMAEN          // habilita DMA1
            |  DMADT_5        // repeated block transfer
            |  DMADSTINCR_0   // add dst unchanged
            |  DMASRCINCR_0   // add src unchanged
            |  0;

    DMA2CTL |= DMAEN          // habilita DMA2
            |  DMADT_5        // repeated block transfer
            |  DMADSTINCR_0   // add dst unchanged
            |  DMASRCINCR_0   // add src unchanged
            |  0;

    DMA0SA = ADCMEM0ADDR;
    DMA1SA = ADCMEM1ADDR;
    DMA2SA = ADCMEM2ADDR;

    DMA0DA = saida_6_1;
    DMA1DA = saida_6_2;
    DMA2DA = saida_t_amb;

    DMA0SZ = 1;
    DMA1SZ = 1;
    DMA2SZ = 1;
}
