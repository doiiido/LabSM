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
			bis.b	#BIT1, &P1OUT	        ; Marca o resistor da entrada P1.1 como pullup.

			bic.b	#BIT1, &P2DIR	        ; Marca o pino P2.1 como entrada para S1.
			bis.b	#BIT1, &P2REN	        ; Marca a entrada do pino P2.1 com resistor.
			bis.b	#BIT1, &P2OUT	        ; Marca o resistor da entrada P2.1 como pullup.

			bis.b	#BIT1, &P2IE			; Ativa a interrupcao do S1.
		    bis.b	#BIT1, &P2IES			; Modo de interrupcao para edge up-down
			bic.b	#BIT1, &P2IFG			; Limpa flag de interrupcao
		    bis.b	#BIT1, &P1IE			; Ativa a interrupcao do S2.
		    bis.b	#BIT1, &P1IES			; Modo de interrupcao para edge up-down
		    bic.b	#BIT1, &P1IFG			; Limpa flag de interrupcao

			nop
		    bis.w	#LPM4|GIE, SR			;Ativa a interrupcao e vai pra baixo consumo LPM4
		   	nop

S2_ISR:
			xor.b	#BIT1, &P1IES			; Alterna o modo de interrupcao entre edge up-down e down-up
			xor.b	#BIT7, &P4OUT			; Alterna o LED P4.7
			call 	#Debouncing_Timer
			reti                            ; Volta pra interrupcao

S1_ISR:
			xor.b	#BIT1, &P2IES			; Alterna o modo de interrupcao entre edge up-down e down-up
			xor.b   #BIT0, &P1OUT			; Alterna o LED P1.0
			call 	#Debouncing_Timer
			reti                            ; Volta pra interrupcao

Debouncing_Timer:
            mov.w	#0, R7        			; Limpa o timer
Timer:		cmp.w	#500, R7				; Timer por loop 500 vezes
			jlo		loop
            bic.b   #BIT1, &P1IFG			; Limpa flags geradas durante o debouncing
            bic.b   #BIT1, &P2IFG			; Limpa flags geradas durante o debouncing
			ret
loop:		inc 	R7
			jmp		Timer
			nop

;Interrupt Config:
           .sect    ".int47"   				; Interrupt do pino 1.1
           .short   S2_ISR    				; Rotina de interrupcao do 1.1
           .sect    ".int42"   				; Interrupt do pino 2.1
           .short   S1_ISR    				; Rotina de interrupcao do 2.1
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
            
