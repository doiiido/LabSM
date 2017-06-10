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

            mov #0, R5
            mov #0, R6

            bis.b   #BIT0, &P1DIR           ; Marca o LED em P1.0 como saida.
            bic.b   #BIT0, &P1OUT           ; Marca o LED como desligado para comessar certo.

            bis.b   #BIT7, &P4DIR           ; Marca o LED em P4.7 como saida.
            bic.b   #BIT7, &P4OUT           ; Marca o LED como desligado para comessar certo.

            bic.b   #BIT1, &P1DIR           ; Marca o pino P1.1 como entrada para S2.
            bis.b   #BIT1, &P1REN           ; Marca a entrada do pino P1.1 com resistor.
            bis.b   #BIT1, &P1OUT           ; Marca o resistor da entrada P1.1 como pulldown.

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
            inc 	R5
            xor.b   #BIT7, &P4OUT           ; Alterna o LED P4.7
            cmp		#1, R6
            jeq		MSB
            inc 	R6
            call    #Debouncing_Timer
            reti                            ; Volta pra interrupcao

Fim_ISRS2:  mov #0, R5
            call    #Debouncing_Timer
            reti

MSB:	  	mov #0, R6
			xor.b   #BIT0, &P1OUT           ; Alterna o LED P1.0
            call    #Debouncing_Timer
            reti

Debouncing_Timer:
            mov.w	#0, R7         ; Limpa o timer TA.
Timer:		cmp.w	#500, R7
			jlo		loop
            bic.b   #BIT1, &P1IFG
			ret
loop:		inc 	R7
			jmp		Timer
			nop

;Interrupt Config:
           .sect    ".int47"   ; added this line
           .short   S2_ISR  ; added this line
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

