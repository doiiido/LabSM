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

vetor:	.byte	12,"L","I","N","C","O","L","N","B","R","U","N","O"

			mov.w 	#vetor,R5				;Copia o vetor para R5
			call 	#menor					;Chama a subrotina menor
			jmp 	$						;Pausa a execução apos a rotina

menor:		mov.b 	#0x00,R7				;zerando R7
			mov.b 	#0xFF,R6				;enchendo R6
			mov.b 	@R5,R8					;copia o primeiro elemento para R8

loop:		inc 	R5						;ponteiro passa para o próximo elemento de R5
;			add		#1,R5					Instrução equivalente ao INC
			cmp.b	@R5,R6					;compara o elemento de R5 atual com o de R6
			jeq		LB1						;se forem iguais pula para LB1
			jlo 	LB2						;se R6 for menor que o elemento de R5 vai para LB2
			mov.b	@R5,R6					;copia o elemento atual de R5 para R6
			mov.b 	#0x00,R7				;Zerando R7

LB1:		inc		R7						;Incrementa R7
;			add		#1,R7					Instrução equivalente ao INC

LB2:		dec 	R8						;Decrementa R8
;			sub		#1,R8					Instrução equivalente ao DEC
			jnz		loop					;se R8 nao for zero volta pra loop
			ret								;Fim da subrotina
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
            
