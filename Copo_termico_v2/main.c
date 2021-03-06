/*Alunos: Lincoln Abreu Barbosa 140045023
          Bruno Freitas Feitosa Nunes 120112388 */

#include <msp430.h>
#include <stdlib.h>
#include <string.h>
#include "UART.h"
#include "LCD.h"
#include "ADC12A.h"

void time_delay(int mult);
void pin_config(void);
void itoa(int num, char to[]);
void muda_faixa(char *input);
void screen_update(int value);
void update ();

volatile int high_temp = 0, low_temp = 0;
unsigned int temp_diss = 25, t_amb = 25;
unsigned short raw_value_1, raw_value_2, raw_value_3;
volatile int previous = 0;
volatile int  on = 0, mode = 1, halt = 0, vent = 0;
char *input;
int temp = 25;
int med = 25;


int main(void) {

    char buff[6];

    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    UART_setup(input);
    setADC();
    setDMA(&raw_value_1, &raw_value_2, &raw_value_3);
    pin_config();
    lcd_pins();
    lcd_init();

    _enable_interrupt();                        // Enable global interrupt

    word_send("Se quiser mudar a temperatura envie high ou low para mudar os limites \n \r");
    word_send("Limiares: \n \r");
    itoa(low_temp, buff);
    word_send(buff);
    word_send("<T<");
    itoa(high_temp, buff);
    word_send(buff);
    word_send("\n \r");

    while(1){
        update();
        delay_100us(500);
    }

}

void shutdown_relays(){
    P3OUT |= BIT5;
    P3OUT &= ~BIT6;
    on = 0;
}

void relay_control(){
   if(temp_diss >= 65){//Controle do ventilador
       shutdown_relays();
       halt = 1;
       vent = 0;
       P7OUT &= ~BIT0;
   }else{
       if (mode == 1){
           if(temp_diss > t_amb+2){
               P7OUT |= BIT0;
               vent = 1;
           }else if(temp_diss < t_amb){
               P7OUT &= ~BIT0;
               vent = 0;
           }
       }else{
           if(vent != 1 && temp_diss <= t_amb+1){
               P7OUT |= BIT0;
               vent = 1;
           }else  if(vent != 0 && temp_diss > t_amb+5){
               P7OUT &= ~BIT0;
               vent = 0;
           }
       }
       halt = 0;
   }
   if (!halt && temp <= low_temp - 2 && mode != 1){
       mode = 1;
       P3OUT |= BIT6;
       P3OUT |= BIT5;
       on = 1;
   }
   if (!halt && temp >= high_temp +2 && mode != 0){
       mode = 0;
       P3OUT &= ~BIT6;
       P3OUT &= ~BIT5;
       on = 1;
   }
   if(mode == 0){
       if (on && (temp <= low_temp
               || (temp <= t_amb && temp_diss >= t_amb + (int)((float)(20.0*(float)t_amb/(float)temp)))
               || temp_diss >= temp + 20)){
           shutdown_relays();
           on = 0;
       }
       if (!halt && !on && temp >= high_temp && (temp_diss <= temp+2 && temp > t_amb || (temp_diss <= t_amb+2 && temp <= t_amb))){
           P3OUT &= ~BIT5;
           P3OUT &= ~BIT6;
           on = 1;
       }
   }else{
       if (!halt && !on && temp <= low_temp){
           P3OUT |= BIT5;
           P3OUT |= BIT6;
           on = 1;
       }
       if (on && temp >= high_temp){
           shutdown_relays();
           on = 0;
       }
   }
}

void update () {
    //Preset
   if(high_temp<=0 && low_temp <=0 && med == raw_value_1){
       high_temp = temp +2;
       low_temp = temp -2;
       screen_update(temp);
   }
   if(high_temp>65)    //Hard limit, plastic safe temperature
       high_temp=65;
   temp_diss = (int)(temp_diss + (int)(77-(12*raw_value_2/458)))/2;
   t_amb = (int)(raw_value_3 - 915);
   //corrigindo erro temperatura interna
   if(vent==1 && on==0) t_amb -= 7;
   else if(on == 0) t_amb -= 7;
   else if(vent == 1 && on == 1 && mode == 1) t_amb -= 9;
   else if(mode == 1 && on == 1) t_amb -=9;
   else if (vent == 1) t_amb -= 3;

   if (has_input() == 1) {
       input = get_input();
       clear_input();
       muda_faixa(input);
   }
   med = (med+raw_value_1)/2;
   temp = (int)(77-(12*med/458));
   if (temp != previous){
       previous = temp;
       screen_update(temp);
   }
   relay_control();
}

void screen_update(int value){
    char buff[6];
    itoa(value, buff);
    lcd_cursor(0,0);
    lcd_str("  Temperatura:");
    lcd_cursor(0,1);
    lcd_str("   ");
    lcd_str(buff);
    lcd_str(" C");
    lcd_str(" ");
    itoa(low_temp, buff);
    lcd_str(buff);
    lcd_str("<T<");
    itoa(high_temp, buff);
    lcd_str(buff);
    word_send("Temperatura: \n \r");
    itoa(value, buff);
    word_send(buff);
    word_send(" C \n \r");
    word_send("Temperatura do dissipador: \n \r");
    itoa(temp_diss, buff);
    word_send(buff);
    word_send(" C \n \r");
    word_send("Temperatura ambiente: \n \r");
    itoa(t_amb, buff);
    word_send(buff);
    word_send(" C \n \r");
}

void muda_faixa(char *input){
    int integer;
        int i;
        char buff[6];
        shutdown_relays();
        if(strcmp("high", input)==0){
            word_send("Qual o limiar superior? \n \r");
            while(has_input() == 0);
            input = get_input();
            clear_input();
            for(i = 0; i< (int)(sizeof(input[i])/sizeof(input[0]));i++){
                integer = (input[i]-'0');
                if(integer<0 || integer >9 ){
                    word_send("Valor invalido! Operacao cancelada \n \r");
                    return;
                }
            }
            integer = atoi(input);
            high_temp = integer;
            lcd_init();
            screen_update(previous);
        }else if(strcmp("low", input)==0){
            word_send("Qual o limiar inferior? \n \r");
            while(has_input() == 0);
            input = get_input();
            clear_input();
            for(i = 0; i< (int)(sizeof(input[i])/sizeof(input[0]));i++){
                integer = (input[i]-'0');
                if(integer<0 || integer >9 ){
                    word_send("Valor invalido! Operacao cancelada \n \r");
                    return;
                }
            }
            integer = atoi(input);
            low_temp = integer;
            lcd_init();
            screen_update(previous);
        }else
            word_send("Nao entendi, mande high ou low pra mudar os limiares \n \r");

        word_send("Limiares: \n \r");
        itoa(low_temp, buff);
        word_send(buff);
        word_send("<T<");
        itoa(high_temp, buff);
        word_send(buff);
        word_send("\n \r");
}

void time_delay (int mult){
    int i = 0;
    int ccr0 = 2 * mult;   //ccr0 ~=0,1 ms
    for(i = 0; i<ccr0; i++){
        __no_operation();
    }
}

void itoa(int num, char to[]){
    char* p = to;
    int aux, div = 1;
    int j, k = 2;
    while(div*10 < num){
        div *= 10;
        k++;
    }
    for(j = 0; j < k; j++) {
        aux = (num/div) % 10;
        div /= 10;
        *p=aux + '0';
        ++p;
    }
    to[k-1] = '\0';
}

//----------------------------------------------------------------------------------------

void pin_config(void) {
    //Rele ventoinha
    P7DIR |= BIT0;              // Marca o pino P2.4 como saida.
    P7DS |= BIT0;               // Drive Strength high
    P7OUT &= ~BIT0;             // Marca o pino como desligado.
    //Reles temp se estado de 1 != do outro = desligado
    P3DIR |= BIT6;              // Marca o pino P2.4 como saida.
    P3DS |= BIT6;               // Drive Strength high
    P3OUT &= ~BIT6;             // Marca o pino como desligado.
    P3DIR |= BIT5;              // Marca o pino P2.4 como saida.
    P3DS |= BIT5;               // Drive Strength high
    P3OUT |= ~BIT5;             // Marca o pino como ligado.

}
//----------------------------------------------------------------------------------------

