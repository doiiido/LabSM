;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file

;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------

;Alunos: Lincoln Abreu Barbosa 140045023
;        Bruno Freitas Feitosa Nunes 120112388

            mov		#0, R5
            mov 	#0, R6
            mov 	#0, R9

            bis.b   #BIT0, &P1DIR           ; Marca o LED em P1.0 como saida.
            bic.b   #BIT0, &P1OUT           ; Marca o LED como desligado para comessar certo.

            bis.b   #BIT7, &P4DIR           ; Marca o LED em P4.7 como saida.
            bic.b   #BIT7, &P4OUT           ; Marca o LED como desligado para comessar certo.

            bic.b   #BIT1, &P1DIR           ; Marca o pino P1.1 como entrada para S2.
            bis.b   #BIT1, &P1REN           ; Marca a entrada do pino P1.1 com resistor.
            bis.b   #BIT1, &P1OUT           ; Marca o resistor da entrada P1.1 como pulldown.

            bic.b   #BIT1, &P2DIR           ; Marca o pino P2.1 como entrada para S1.
            bis.b   #BIT1, &P2REN           ; Marca a entrada do pino P2.1 com resistor.
            bis.b   #BIT1, &P2OUT           ; Marca o resistor da entrada P2.1 como pulldown.

            bis.b   #BIT1, &P2IE            ; Ativa a interrupcao do S1.
            bis.b   #BIT1, &P2IES
            bic.b   #BIT1, &P2IFG
            bis.b   #BIT1, &P1IE            ; Ativa a interrupcao do S2.
            bis.b   #BIT1, &P1IES
            bic.b   #BIT1, &P1IFG

            nop
            bis.w   #LPM4|GIE, SR           ;Ativa a interrupcao e vai pra baixo consumo LPM4
            nop;

S2_ISR:
            xor.b   #BIT1, &P1IES
            cmp.b   #1, R5
            jeq     Fim_ISRS2
            inc R5
            cmp	#1, R6
            jeq	pisca2
            xor.b   #BIT7, &P4OUT           ; Alterna o LED P4.7
            jmp     Debouncing_Timer
            reti                            ; Volta pra interrupcao

Fim_ISRS2:  mov #0, R5
            cmp	#1, R9
            jeq	pisca2
            jmp     Debouncing_Timer
            reti

S1_ISR:
            xor.b   #BIT1, &P2IES
            cmp.b   #1, R6
            jeq     Fim_ISRS1
            inc R6
            cmp	#1, R5
            jeq	pisca2
            xor.b   #BIT0, &P1OUT           ; Alterna o LED P1.0
            jmp     Debouncing_Timer

Fim_ISRS1:  mov #0, R6
            cmp	#1, R9
            jeq	pisca2
            jmp     Debouncing_Timer

pisca2:
			cmp.b 	#0, R9
			jeq		Timer
			cmp 	#1, R5
			jeq		no_operation
			cmp 	#1, R6
			jeq		no_operation
			mov.b	R7, P1OUT
			mov.b	R8, P4OUT
            bic.b   #BIT1, &P2IFG           ; Reseta o sinal de interrupcao.
            bic.b   #BIT1, &P1IFG           ; Reseta o sinal de interrupcao.
			bic.w   #TAIFG, &TA1CTL         ; Reseta o sinal de interrup�ao.
            bic.w   #TAIE, &TA1CTL          ; Desativa a interrupcao do timer TA1.
            mov #0, R5
            mov #0, R6
            mov #0, R9
            reti

no_operation:	nop

Debouncing_Timer:
            bis.w   #TACLR, &TA0CTL         ; Limpa o timer TA.

            bis.w   #TASSEL_1, &TA0CTL      ; Usando ACLK (32768 Hz).
            bic.w   #ID_0, &TA0CTL          ; Divide por 1.
            bis.b   #TAIDEX_0, &TA0EX0      ; Divide por 1 (extendido).

            bis.w   #TAIE, &TA0CTL          ; Ativa a interrupcao do timer TA0.
            bic.b   #BIT1, &P2IE            ; Desativa a interrupcao do S1.
            bic.b   #BIT1, &P1IE            ; Desativa a interrupcao do S2.

            bis.w   #3276, &TA0CCR0         ; Marca o TA0CCR0 como 3276, resultando em uma chamada de 10Hz (100ms)
                                            ; da rotina
            bis.w   #MC_1, &TA0CTL          ; Marca o modo de timer para contar ate TA0CCR0.
            reti

DebouncingISR:
            bic.w   #TAIFG, &TA0CTL         ; Reseta o sinal de interrup�ao.
            bic.w   #TAIE, &TA0CTL          ; Desativa a interrupcao do timer TA0.
            bis.b   #BIT1, &P2IE            ; Ativa a interrupcao do S1.
            bis.b   #BIT1, &P1IE            ; Ativa a interrupcao do S2.
            bic.b   #BIT1, &P2IFG           ; Reseta o sinal de interrupcao.
            bic.b   #BIT1, &P1IFG           ; Reseta o sinal de interrupcao.
            reti                            ;volta pra interrup�ao

Timer:
			mov.b	P1OUT, R7
			mov.b	P4OUT, R8
			bic.b   #BIT1, &P2IES
			bic.b   #BIT1, &P1IES
			bis.b   #BIT0, &P1OUT
			bic.b   #BIT7, &P4OUT
			mov.b	#1, R5
			mov.b	#1, R6
			mov.b	#1, R9
            bis.w   #TACLR, &TA1CTL         ; Limpa o timer TA1.

            bis.w   #TASSEL_1, &TA1CTL      ; Usando ACLK (32768 Hz).
            bic.w   #ID_0, &TA1CTL          ; Divide por 1.
            bis.b   #TAIDEX_0, &TA1EX0      ; Divide por 1 (extendido).

            bis.w   #TAIE, &TA1CTL          ; Ativa a interrupcao do timer TA1.

            bis.w	#16384, &TA1CCR0		; Marca o TA0CCR0 como 16384, resultando em uma chamada de 2Hz
                                            ; da rotina
            bis.w   #MC_1, &TA1CTL          ; Marca o modo de timer para contar ate TA1CCR0.

TimerISR:
            bic.b   #BIT1, &P2IFG           ; Reseta o sinal de interrupcao.
            bic.b   #BIT1, &P1IFG           ; Reseta o sinal de interrupcao.
            bic.w   #TAIFG, &TA1CTL         ; Reseta o sinal de interrup�ao.
            xor.b   #BIT0, &P1OUT           ; Alterna o LED P1.0
            xor.b   #BIT7, &P4OUT           ; Alterna o LED P4.7

            reti                            ;volta pra interrup�ao


;Interrupt Config:
           .sect    ".int47"   ; added this line
           .short   S2_ISR  ; added this line
           .sect    ".int42"   ; added this line
           .short   S1_ISR  ; added this line
           .sect    ".int52"
           .short   DebouncingISR
           .sect    ".int48"
           .short   TimerISR
;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack

;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET

