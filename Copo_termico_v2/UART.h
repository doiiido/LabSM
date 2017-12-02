/*
 * UART.h
 *
 *  Created on: 1 de dez de 2017
 *      Author: lincoln
 */

#ifndef UART_H_
#define UART_H_

void UART_setup();

void word_send(const char *str);

int has_input(void);

char * get_input();

void clear_input(void);

__interrupt void USCI_A0_ISR(void);

#endif /* UART_H_ */
