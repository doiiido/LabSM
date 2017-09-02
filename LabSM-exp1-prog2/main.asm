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

vetor:	.byte	6,0,"L","I","N","C","O","L","N","B","R","U","N","O"

			mov.w 	#vetor,R5				;Copia o vetor para R5
			call 	#MAIOR16				;Chama subrotina MAIOR16
			jmp 	$						;Pausa a execução

MAIOR16:	mov.b 	#0x00,R7				;Esvazia R7
			mov.w 	#0x00,R6				;Esvazia R6
			mov.w 	@R5,R8					;Copia o tamanho do vetor para R8
			incd 	R5						;Pula 2 posições, indo pra L
;			add		#2,R5					Instrução equivalente ao INC


loop:		mov.w 	#0x00,R9				;Esvazia R9
			add.w	@R5, R9					;Adiciona os primeiro 8 bits para R9
			inc 	R5						;Proxima palavra
;			add		#1,R5					Instrução equivalente ao INC
			cmp.w	R9,R6					;Compara R9 a maior soma ja encontrada
			jeq		LB1						;Caso a soma ja exista
			jc	 	LB2						;Caso a soma seja maior que R9
			mov.w	R9,R6					;Copia a soma para R6
			mov.b 	#0x00,R7				;Zerando as ocorrencias

LB1:		inc		R7						;Marca a ocorrencia
;			add		#1,R7					Instrução equivalente ao INC

LB2:		dec 	R8						;Decrementa o tamanho restante
;			sub		#1,R5					Instrução equivalente ao DEC
			jnz		loop					;Se ainda há o que olhar executa a subrotina novamente
			ret
;			mov		@SP+,PC					Instrução equivalente ao RET

                                            

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
            
