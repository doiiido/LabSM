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

			mov #MEMO,R5

			mov.w	#16,R8					;Contador
			mov.b	#0,R6					;Receberá a palavra de 16 bits
			mov.b	#0x04,R7				;Conta de 4 seq de 4 bits (1 hex)
			mov.b	#0x00,R9
			call	#ASC_W16				;Chama a subrotina

OK:			jc		OK 						;travar execução com sucesso
NOK:		jnc		NOK 					;travar execução com falha


ASC_W16:	mov.b	@R5,R11					;Cria uma posição auxiliar pra trabalhar
			cmp.b	#57,R11					;Descobrir se o número é maior que 9 ASCII
			jhs		big9					;Testar segunda condição
			cmp.b	#48,R11					;Se ASCII for menor que 0x30 não serve
			jlo		invalido				;Salta direto pro retorno
			sub		#48,R11					;Se for valido, subtrai de 30 p/ ASCII
			jmp		valido					;Já garanti que é valido, pode saltar

big9:		cmp.b	#65,R11					;Testa se é menor que 0x41 e maior q 0x39
			jlo		invalido				;Se for, salta direto pro retorno
			cmp.b	#70,R11					;Teste se é maior que F ASCII
			jhs		invalido				;Se for, salta direto pro retorno
			sub		#55,R11					;Se não for, subtrai de 0x37 p/ ASCII

valido:		inc		R5						;Caminha com o ponteiro
			decd	R8						;Volta 4 bits e
			decd	R8						;mata primeiro hex
			mov.b	R8,R10					;R10 é um contador auxiliar p/ somar R11 ao R6
			jz		ultimo

lb:											;Pegar o nº obtido com R11 e somar ao R6
			clrc
			rlc		R11						;Rotacionar o R11 até chega nos bits que interessam
			dec		R10						;Usa o contador auxiliar pra chegar nos bits queinteressam
			jnz		lb
			clrz

ultimo:		add		R11,R6					;Vai montando em R6 a palavra de 16 bits
			dec		R7						;Decrementa o contador
			jnz		ASC_W16					;Volta pra achar prox hexadecimal até zerar R7
			setc							;Sucesso! Retorna com o carry em 1
			jmp 	OK						;Marcar sucesso

invalido:	clrc							;Deu invalido. Retorna o carry em 0
			jmp		NOK						;Marcar falha

			ret

;----------------------------------------------------------------------------
; Segmento de dados inicializados (0x2400)
;----------------------------------------------------------------------------
								.data
; Declarar 4 caracteres ASCII (0x38, 0x39, 0x41, 0x42)
MEMO: .byte '3','6','B','4'



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

