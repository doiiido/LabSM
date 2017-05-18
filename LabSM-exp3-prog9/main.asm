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

			bis.b	#BIT7, &P4DIR	        ; Marca o LED em P4.7 como saida.
		    bic.b	#BIT7, &P4OUT			; Marca o LED como desligado para comessar certo.

			bic.b	#BIT1, &P1DIR	        ; Marca o pino P1.1 como entrada para S2.
			bis.b	#BIT1, &P1REN	        ; Marca a entrada do pino P1.1 com resistor.
			bic.b	#BIT1, &P1OUT	        ; Marca o resistor da entrada P1.1 como pulldown.

			bic.b	#BIT1, &P2DIR	        ; Marca o pino P2.1 como entrada para S1.
			bis.b	#BIT1, &P2REN	        ; Marca a entrada do pino P2.1 com resistor.
			bic.b	#BIT1, &P2OUT	        ; Marca o resistor da entrada P2.1 como pulldown.

			bis.b	#BIT1, &P2IE			; Ativa a interrup�ao do S1.
		    bis.b	#BIT1, &P1IE			; Ativa a interrup�ao do S2.

			nop
		    bis.w	#LPM0|GIE, SR			;Ativa a interrup�ao e vai pra baixo consumo LPM3
		   	nop

S1interruptRoutine:

			bic.b	#BIT1, &P2IFG			; Reseta o sinal de interrup�ao.
			xor.b	#BIT0, &P1OUT			; Alterna o LED P1.0
			reti							; Volta pra interrup�ao

S2interruptRoutine:
			bic.b	#BIT1, &P1IFG		; Reseta o sinal de interrup�ao.
			xor.b   #BIT7, &P4OUT			; Alterna o LED P4.7
			reti							; Volta pra interrup�ao

;Interrupt Config:
            .sect   ".int52"
;            .short  S1interruptRoutine
            .short  S2interruptRoutine
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
            
