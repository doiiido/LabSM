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

.data
; Declarar vetor [LINCOLNBRUNO]
vetor: .byte  12,'L','I','N','C','O','L','N','B','R','U','N','O'

			mov.w 	#vetor, R5				;Copia o vetor para R5
			call 	#M2M4					;Chama subrotina M2M4
			jmp 	$						;Pausa a execuçao

M2M4: 		mov.b	@R5,R8					;coloca no R8 (contador) o tamanho do vetor
			mov		#0,R6					;zera o contador de multiplos de 2
			mov		#0,R7					;zera o contador de multiplos de 4

loop: 		inc		R5						;Avança para o proximo elemnto do vetor
			mov.b	#0x1,R9					;Marca R9 como 1
			and.b	@R5,R9
			jnz		nm						;se nao for zero, nao e multiplo de 2
			inc		R6						;é multiplo de 2
			mov.b	#0x3,R9					;Marca R9 como 3
			and.b	@R5,R9
			jnz		nm						;se nao for zero, nao e multiplo de 4
			inc		R7						;é multiplo de 4

nm:			dec 	R8						;Decrementa o tamanho restante
			jnz		loop					;Se ainda há o que olhar executa a subrotina novamente
			ret
                                            

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
            
