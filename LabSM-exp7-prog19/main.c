/*Alunos: Lincoln Abreu Barbosa 140045023
          Bruno Freitas Feitosa Nunes 120112388 */

#include <msp430.h>

void time_delay (int mult);
void timer_config(void);
void adc_config(void);


int values[501];
int * pointer = values;
unsigned char volatile number_of_readings = 0;
int volatile raw_value;

#define SET(x, y)    (x |= (y))
#define CLR(x, y)    (x &= ~(y))


int main(void) {
    int med = 0;

    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    timer_config();
    adc_config();
    _enable_interrupt();                        // Enable global interrupt

    while(1) {
        while ( (ADC12IFG & BIT0) == 0);
        number_of_readings++;
        raw_value = ADC12MEM0;
        raw_value = raw_value;
        *pointer = raw_value;
        //if(med==0 || *pointer >= med -10 && *pointer <= med +10){
            med = (med+*pointer)/2;
        //}
        *pointer = med;
        if (pointer == values + 500) {
            pointer = values;
            while(1)
                __no_operation();

        }
        pointer++;
        time_delay(100);
    }
}

void time_delay (int mult){
    int i = 0;
    int ccr0 = 2 * mult;   //ccr0 ~=0,1 ms
    for(i = 0; i<ccr0; i++){
        __no_operation();
    }
}

void timer_config(void) {
    // Timer A, que controlará o funcionamento do adc
    TA0CTL = (TASSEL__SMCLK |                   // SMCLK = 1MHz
              ID__4         |                   // Dividir por 4: 1MHz/4 = 250kHz
              MC__UP);                          // Modo UP
    TA0CCR0 = 1250;                            // 50Hz, 20ms
    TA0CCTL1 = OUTMOD_6;                        // Modo toogle/set
    TA0CCR1 = 625;                              //100hz, 10ms
}

void adc_config(void) {
    CLR(ADC12CTL0, ADC12ENC);                   // Desabilitar para configurar

    ADC12CTL0 = (ADC12SHT0_3 |                  // ADC tempo amostragem ADCMEM[0-7]
                ADC12ON);                       // Ligar ADC

    ADC12CTL1 = (ADC12CSTARTADD_0 |             // Armazenar na Posicao 0
                ADC12SHS_1        |             // Usar TA0.1 (Selecionar (ADC12SC bit))
                ADC12SHP          |             // S/H usar timer
                ADC12DIV_0        |             // Divisor = 1
                ADC12SSEL_3       |             // SMCLK
                ADC12CONSEQ_2);                 // Modo Único Canal

    ADC12CTL2  = (ADC12TCOFF |                  // Desligar sensor temperatura
                 ADC12RES_2);                   // Resolucao 12-bit

    ADC12MCTL0 = (ADC12EOS   |                  // Fim
                 ADC12SREF_0 |                  // VR+ = AVCC e VR- = AVSS
                 ADC12INCH_4);                  // A4 = Canal 4

    SET(ADC12CTL0, ADC12ENC);                   // Habilitar ADC12
}
