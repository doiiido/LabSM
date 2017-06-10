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

            mov		#0, R5						; Inicializando contador Botao S2
            mov 	#0, R6						; Inicializando contador Botao S1
            mov 	#0, R9						; Inicializando marcador Piscando

            bis.b   #BIT0, &P1DIR           ; Marca o LED em P1.0 como saida.
            bic.b   #BIT0, &P1OUT           ; Marca o LED como desligado para comessar certo.

            bis.b   #BIT7, &P4DIR           ; Marca o LED em P4.7 como saida.
            bic.b   #BIT7, &P4OUT           ; Marca o LED como desligado para comessar certo.

            bic.b   #BIT1, &P1DIR           ; Marca o pino P1.1 como entrada para S2.
            bis.b   #BIT1, &P1REN           ; Marca a entrada do pino P1.1 com resistor.
            bis.b   #BIT1, &P1OUT           ; Marca o resistor da entrada P1.1 como pullup.

            bic.b   #BIT1, &P2DIR           ; Marca o pino P2.1 como entrada para S1.
            bis.b   #BIT1, &P2REN           ; Marca a entrada do pino P2.1 com resistor.
            bis.b   #BIT1, &P2OUT           ; Marca o resistor da entrada P2.1 como pullup.

            bis.b   #BIT1, &P2IE            ; Ativa a interrupcao do S1.
            bis.b   #BIT1, &P2IES			; Modo de interrupcao para edge up-down
            bic.b   #BIT1, &P2IFG			; Limpa flag de interrupcao
            bis.b   #BIT1, &P1IE            ; Ativa a interrupcao do S2.
            bis.b   #BIT1, &P1IES			; Modo de interrupcao para edge up-down
            bic.b   #BIT1, &P1IFG			; Limpa flag de interrupcao

            nop
            bis.w   #LPM4|GIE, SR           ;Ativa a interrupcao e vai pra baixo consumo LPM4
            nop;

S2_ISR:
            xor.b   #BIT1, &P1IES			; Alterna o modo de interrupcao entre edge up-down e down-up
            cmp.b   #1, R5					; Contagem do aperta/solta botao S2 (0=solto 1=apertado)
            jeq     Fim_ISRS2				; Rotina do solta botao S2
            mov		#1, R5					; Botao S2 apertado
            cmp		#1, R6					; Verifica o botao S1 (0=solto 1=apertado)
            jeq		pisca2					; Rotina pra piscar os leds
            xor.b   #BIT7, &P4OUT           ; Alterna o LED P4.7
            call    #Debouncing_Timer
            reti                            ; Volta pra interrupcao

Fim_ISRS2:  mov 	#0, R5					; Botao S2 solto
            cmp		#1, R9					; Verifica se esta piscando
            jeq		pisca2					; Rotina de piscar os leds
            call    #Debouncing_Timer
            reti                            ; Volta pra interrupcao

S1_ISR:
            xor.b   #BIT1, &P2IES			; Alterna o modo de interrupcao entre edge up-down e down-up
            cmp.b   #1, R6					; Contagem do aperta/solta botao S1 (0=solto 1=apertado)
            jeq     Fim_ISRS1				; Rotina do solta botao S1
           	mov		#1, R6					; Botao S1 apertado
            cmp		#1, R5					; Verifica o botao S2 (0=solto 1=apertado)
            jeq		pisca2					; Rotina pra piscar os leds
            xor.b   #BIT0, &P1OUT           ; Alterna o LED P1.0
            call    #Debouncing_Timer
			reti                            ; Volta pra interrupcao

Fim_ISRS1:  mov 	#0, R6					; Botao S1 solto
            cmp		#1, R9					; Verifica se esta piscando
            jeq		pisca2					; Rotina de piscar os leds
            call    #Debouncing_Timer
			reti                            ; Volta pra interrupcao
pisca2:
			cmp.b 	#0, R9					; Verifica se já esta piscando
			jeq		Timer					; Timer e alterna leds
			cmp 	#1, R5					; Verifica o botao S2 (0=solto 1=apertado)
			jeq		no_operation			; Nao faz nada (continua piscando)
			cmp 	#1, R6					; Verifica o botao S1 (0=solto 1=apertado)
			jeq		no_operation			; Nao faz nada (continua piscando)
			mov.b	R7, P1OUT				; Restaura o estado do led 1
			mov.b	R8, P4OUT				; Restaura o estado do led 2
            bic.b   #BIT1, &P2IFG           ; Reseta o sinal de interrupcao do S1.
            bic.b   #BIT1, &P1IFG           ; Reseta o sinal de interrupcao do S2.
			bic.w   #TAIFG, &TA1CTL         ; Reseta o sinal de interrupcao do Timer e alterna leds.
            bic.w   #TAIE, &TA1CTL          ; Desativa a interrupcao do timer TA1.
            mov 	#0, R5					; Botao S2 solto
            mov 	#0, R6					; Botao S1 solto
            mov 	#0, R9					; Nao esta piscando
            reti                            ; Volta pra interrupcao

no_operation:
			call    #Debouncing_Timer
			bic.b   #BIT1, &P2IFG  	     	; Reseta o sinal de interrupcao do S1.
        	bic.b   #BIT1, &P1IFG       	; Reseta o sinal de interrupcao do S2.
			reti                        	; Volta pra interrupcao

Debouncing_Timer:
            mov.w	#0, R10         		; Limpa o timer.
DTimer:		cmp.w	#500, R10				; Timer por loop 500 vezes
			jlo		loop
            bic.b   #BIT1, &P1IFG			; Limpa flags geradas durante o debouncing
            bic.b   #BIT1, &P2IFG			; Limpa flags geradas durante o debouncing
			ret
loop:		inc 	R10
			jmp		DTimer
			nop

Timer:
			mov.b	P1OUT, R7				; Salva o estado do led 1
			mov.b	P4OUT, R8				; Salva o estado do led 2
			bic.b   #BIT1, &P2IES			; Modo de interrupcao para edge down-up (soltar botao)
			bic.b   #BIT1, &P1IES			; Modo de interrupcao para edge down-up (soltar botao)
			bis.b   #BIT0, &P1OUT			; Led 1 ligado
			bic.b   #BIT7, &P4OUT			; Led 2 desligado
			mov.b	#1, R5					; Botao S2 apertado
			mov.b	#1, R6					; Botao S1 apertado
			mov.b	#1, R9					; Piscando
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
            bic.w   #TAIFG, &TA1CTL         ; Reseta o sinal de interrupï¿½ao.
            xor.b   #BIT0, &P1OUT           ; Alterna o LED P1.0
            xor.b   #BIT7, &P4OUT           ; Alterna o LED P4.7

            reti                            ;volta pra interrupcao


;Interrupt Config:
           .sect    ".int47"   				; Interrupt do pino 1.1
           .short   S2_ISR    				; Rotina de interrupcao do 1.1
           .sect    ".int42"   				; Interrupt do pino 2.1
           .short   S1_ISR    				; Rotina de interrupcao do 2.1
           .sect    ".int48"   				; Interrupt do timer e alternaleds
           .short   TimerISR  				; Rotina de interrupcao do timer e alterna leds
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

