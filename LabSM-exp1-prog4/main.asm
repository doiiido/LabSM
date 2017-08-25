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

Matr: .set  14004

			mov.w	#Matr,R6				;Copia a palavra 16bit para R6
			mov.w   #0x2403, R5				;R5 aponta pra o inicio da ram de uso comum + 4 nibbles (invertido por ser little endian)
			call 	#W16_ASC				;Chama subrotina W16_ASC
			jmp 	$						;Pausa a execuçao

W16_ASC:	mov		#0x0,R7					;Esvazia R7, contador de bits 0-16 para word 16bit
			mov		#0x0,R9					;Esvazia R9, R9 é contador de grandeza do bit (0-4) para um nibble
			mov		#0x0,R10				;Esvazia R10

loop:		inc		R9						;Proximo nibble
			cmp.b	#0x5,R9					;Verifica se completou um nibble
			jeq		Write					;escreve o nibble
			inc 	R7						;lendo o proximo bit
			rrc 	R6						;Extrai o bit LSB
			jc		bit1					;Se o bit for 1 vai pra bit1
			jmp 	loop					;repete

bit1:		clrc							;Limpando o carry
			cmp.b	#0x1,R9
			jeq		case1					;se a grandeza for 1, vai pra case 1
			cmp.b	#0x2,R9
			jeq		case2					;se a grandeza for 2, vai pra case 2
			cmp.b	#0x3,R9
			jeq		case4					;se a grandeza for 3, vai pra case 4
			cmp.b	#0x4,R9
			jeq		case8					;se a grandeza for 4, vai pra case 8
			jmp		BRK						;FAIL

case1:		add		#1, R10					;adiciona 1 no valor do nibble sendo analisado
			jmp		loop

case2:		add		#2, R10					;adiciona 2 no valor do nibble sendo analisado
			jmp		loop

case4:		add		#4, R10					;adiciona 4 no valor do nibble sendo analisado
			jmp		loop

case8:		add		#8, R10					;adiciona 8 no valor do nibble sendo analisado
			jmp		loop

Write:		mov		#0x0,R9					;Zerando por zer o proximo nibble
			cmp.b	#0xA,R10				;Verificando se o nibble é numero ou letra para gerar ASCII
			jlo		number					;é numero
			jhs		char					;é letra

number:		add		#48,R10					;Adiciona 48 para ficar a partir do 0 ASCII
			mov.b	R10,0(R5)				;Escreve o nibble
			mov.b	#0x0,R10				;Recomençando o nibble
			dec		R5						;proximo a ser escrito
			jmp		Exit

char:		add		#55,R10					;Adiciona 55 para ficar a partir do A ASCII
			mov.b	R10,0(R5)				;Escreve o nibble
			mov.b	#0x0,R10				;Recomençando o nibble
			dec		R5						;proximo a ser escrito
			jmp		Exit


Exit:		mov		#0x0,R9					;novo nibble
			cmp.w	#16,R7
			jeq		BRK						;se analisou 16 bit, é o fim
			jlo		loop					;se nao, roda denovo

BRK:		ret								;fim da subrotina


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
            
