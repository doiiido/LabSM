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

			mov		#vetor,R5				;Ponteiro
			call 	#ORDENA 				;chamar subrotina
			jmp 	$ 						;travar execução
			nop								;exigido pelo montador



;Subrotina ORDENA prepara o ambiente para execuçao
ORDENA:		mov.b	@R5,R6		      		;Contador principal (tamanho do vetor)
			mov.w	R5,R8					;posiçao inicial
			mov.b	R6,R7					;Copindo para o contador secundario
			dec	R7							;o valor de n-1

			inc	R5							;Move para o primeiro elemento
			mov.w	R5,R10					;Ponteiro auxiliar
			call	#loop					;chamar subrotina de execuçao
			mov.w	R8,R5					;restaurando a referencia
			ret

loop:		inc	R10							;Elemento n+k
			call	#ORD					;Chamando subrotina que compara n e n+1
			dec		R7						;Decrementando um ciclo
			jz		prox					;Subrotina que troca o Pivo
			jmp		loop



prox:		inc		R5						;Troca o Pivo
			dec		R6						;Decrementando um ciclo do Pivo
			mov.w	R5,R10					;Movendo o auxiliar para posiçao Pivo
			mov.b	R6,R7					;Contador secundario fica definido
			dec		R7						;como primario -1
			jnz		loop					;Roda novo ciclo com Pivo novo
			ret								;Caso secundario = 0, terminou os ciclos



ORD:		cmp.b	@R5,0(R10)				;Comparandoo elemento n ao n+k
			jlo		swap					;Se o elemento n+k < n,troca de posiçao
			ret



swap:		mov.b	@R5,R9					;Salva elemento n para posiçao temporaria
			mov.b	@R10,0(R5)				;Escreve o elemento n+k no lugar do n
			mov.b	R9,0(R10)				;Escreve o elemento n no lugar do n+k
			ret



; Declarar vetor [LINCOLNBRUNO]

		.data
vetor:	.byte	12,'L','I','N','C','O','L','N','B','R','U','N','O'

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
            
