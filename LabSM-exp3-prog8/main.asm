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
;		 Bruno Freitas Feitosa Nunes 120112388

			bis.b	#BIT0, &P1DIR	        ; Marca o LED em P1.0 como saida.
		    bic.b	#BIT0, &P1OUT			; Marca o LED como desligado para comessar certo.

		    bis.w	#TACLR, &TA0CTL			; Limpa o timer TA.

		    bis.w	#TASSEL_1, &TA0CTL		; Usando ACLK (32768 Hz).
		    bic.w	#ID_0, &TA0CTL			; Divide por 1.
		    bis.b	#TAIDEX_0, &TA0EX0		; Divide por 1 (extendido).

		    bis.w	#TAIE, &TA0CTL			; Ativa a interrup�ao do timer TA0.

			bis.w	#16384, &TA0CCR0		; Marca o TA0CCR0 como 16384, resultando em uma chamada de 2Hz
											; da rotina

		    bis.w	#MC_1, &TA0CTL			; Marca o modo de timer para contar ate TA0CCR0.

			nop
		    bis.w	#LPM3|GIE, SR			;Ativa a interrup�ao e vai pra baixo consumo LPM3
		    nop

TAinterruptRoutine:
            bic.w	#TAIFG, &TA0CTL			; Reseta o sinal de interrup�ao.
            xor.b	#BIT0, &P1OUT			; Alterna o LED P1.0
            reti							;volta pra interrup�ao

;-------------------------------------------------------------------------------
;Configura�ao Interrup�ao:
;-------------------------------------------------------------------------------
            .sect   ".int52"
            .short  TAinterruptRoutine

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

