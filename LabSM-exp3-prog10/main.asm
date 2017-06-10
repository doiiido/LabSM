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

            mov #0, R5						; Inicializando contadores
            mov #0, R6						; Inicializando contadores

            bis.b   #BIT0, &P1DIR           ; Marca o LED em P1.0 como saida.
            bic.b   #BIT0, &P1OUT           ; Marca o LED como desligado para comessar certo.

            bis.b   #BIT7, &P4DIR           ; Marca o LED em P4.7 como saida.
            bic.b   #BIT7, &P4OUT           ; Marca o LED como desligado para comessar certo.

            bic.b   #BIT1, &P1DIR           ; Marca o pino P1.1 como entrada para S2.
            bis.b   #BIT1, &P1REN           ; Marca a entrada do pino P1.1 com resistor.
            bis.b   #BIT1, &P1OUT           ; Marca o resistor da entrada P1.1 como pullup.

            bis.b   #BIT1, &P1IE            ; Ativa a interrupcao do S2.
            bis.b   #BIT1, &P1IES			; Modo de interrupcao para edge up-down
            bic.b   #BIT1, &P1IFG			; Limpa flag de interrupcao

            nop
            bis.w   #LPM4|GIE, SR           ; Ativa a interrupcao e vai pra baixo consumo LPM4
            nop

S2_ISR:
            xor.b   #BIT1, &P1IES			; Alterna o modo de interrupcao entre edge up-down e down-up
            cmp.b   #1, R5					; Contagem do aperta/solta botao (0=solto 1=apertado)
            jeq     Fim_ISRS2				; Rotina do solta botao
            mov 	#1, R5					; Botao apertado
            xor.b   #BIT7, &P4OUT           ; Alterna o LED P4.7
            cmp		#1, R6					; Se contador do LSB ja era 1, alterna o MSB
            jeq		MSB
            mov 	#1, R6					; LSB = 1
            call    #Debouncing_Timer
            reti                            ; Volta pra interrupcao

Fim_ISRS2:  mov #0, R5						; Marca o botao como solto
            call    #Debouncing_Timer
            reti                            ; Volta pra interrupcao

MSB:	  	mov #0, R6						; LSB = 0
			xor.b   #BIT0, &P1OUT           ; Alterna o LED P1.0
            call    #Debouncing_Timer
            reti                            ; Volta pra interrupcao

Debouncing_Timer:
            mov.w	#0, R7         			; Limpa o timer.
Timer:		cmp.w	#500, R7				; Timer por 500 loops
			jlo		loop
            bic.b   #BIT1, &P1IFG			; Limpa flags geradas durante o debouncing
			ret
loop:		inc 	R7
			jmp		Timer
			nop

;Interrupt Config:
           .sect    ".int47"   				; Interrupt so pino 1.1
           .short   S2_ISR  				; Rotina de interrupcao
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

