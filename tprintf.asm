extern _GetStdHandle@4                  ; kernel32.dll
extern _WriteConsoleA@20                ; kernel32.dll
extern _ExitProcess@4                   ; kernel32.dll

global __start

section .text

;--------------------------------------------------------------------------
; Function prints single character
; Param: al - character to print
;--------------------------------------------------------------------------
fastOut:		
				push eax			;
				push edx			; saving registers
				
				mov BYTE [FastOutText], BYTE al
				xor eax, eax
				push dword -11          ; STD_OUTPUT_HANDLE = -11
                call _GetStdHandle@4    ; stdout = GetStdHandle (-11)
                
                xor edx, edx
                push edx                ; Resvd = 0
                push edx                ; Ptr to number of chars written = NULL
                push 1                  ; strlen ("Text\n")
                push dword FastOutText
                push eax                ; stdout = GetStdHandle (STD_OUTPUT_HANDLE) 
                call _WriteConsoleA@20  ; WriteConsole (stdout, msg, 5, NULL, 0)
				
				pop edx				; restoring registers
				pop eax				;
				
				ret
;--------------------------------------------------------------------------


;--------------------------------------------------------------------------
; Closes the program
;--------------------------------------------------------------------------
fastExit:
			    xor eax, eax
                push eax                ; ExitCode = 0
                call _ExitProcess@4     ; ExitProcess (0)
				ret
;--------------------------------------------------------------------------




;--------------------------------------------------------------------------
; Simply prints string into console
; eax - pointer to this string, string may ends with 0Ah symbol
;--------------------------------------------------------------------------
tSimplePrintString:
		push ebx
		push eax
		mov ebx, eax
		
T_SIMPLE_PRINT_STRING_LOOP:	
		mov al, BYTE [ebx]		; cl now stores current symbol
		cmp al, '$'			; if current symbol is 0Ah then return
		je T_SIMPLE_PRINT_STRING_RETURN
		
		inc ebx					; ebx++
		call fastOut
		
		jmp T_SIMPLE_PRINT_STRING_LOOP
	
T_SIMPLE_PRINT_STRING_RETURN:
		pop eax
		pop ebx
		ret
;--------------------------------------------------------------------------






;--------------------------------------------------------------------------
; This function displays number into console.
; eax - number to display
; ecx - radix
;--------------------------------------------------------------------------
tPrintNumber:
    push eax			; save registers
    push edx			;
	push ecx			;
	
    xor edx,edx          ; edx = 0
    div dword ecx  		 ; eax = quotient, edx = remainder
    test eax,eax         ; Is quotient zero?
    je T_PRINT_NUMBER_POINT               ; yes, don't display it
    call tPrintNumber    ; Display the quotient
T_PRINT_NUMBER_POINT:
    mov eax, edx
	
	cmp eax, 10d		; checking if this digit is bigger then 9 
	jl T_PRINT_NUMBER_SKIP_SYMBOL_CONVERT 
	add eax, 7d			; so we add 7 to skip some bad symbols
T_PRINT_NUMBER_SKIP_SYMBOL_CONVERT:	
	
	add eax, '0'
	
    call fastOut  		 ; Display the remainder
	
	pop ecx				; 
    pop edx				; load registers
	pop eax				; 
    ret
;--------------------------------------------------------------------------



;--------------------------------------------------------------------------
; Standart printf function
; Param: first DWORD - string pointer
;		 DWORD1, DWORD2... - arguments for this string
;
;--------------------------------------------------------------------------
tPrintf:	push eax	;
			push ebx	; Saving registers
			push ecx	;
			push edx	;

			mov ebx, DWORD [esp + 20]			; stores pointer to string characters
			mov edx, 24							; edx - argument pointer
T_PRINTF_LOOP:
			
			mov ah, [ebx]						; ah now stores current symbol
			inc ebx								; ebx++
			
			
			;--------------------------------
			;--------------------------------
			; End of the loop
			;--------------------------------
			cmp ah, '$'
			je T_PRINTF_LOOP_END
			;--------------------------------
			;--------------------------------
			
			

			
			;--------------------------------
			;--------------------------------
			; just printing symbol
			;--------------------------------
			cmp ah, '%'
			je T_PRINTF_PRINT_CHAR_END
			
			mov al, ah
			call fastOut
			T_PRINTF_PRINT_CHAR_END:
			;--------------------------------
			;--------------------------------
			
			
			;--------------------------------
			;--------------------------------
			; special operations
			;--------------------------------
			cmp ah, '%'
			jne T_PRINTF_SPECIAL_OPERATION_END
			
			mov ah, [ebx]						; ah now stores special symbol
			inc ebx
			
			;---
			; single character
			;---
			cmp ah, 'c'
			jne T_PRINTF_SO_C_END
			
			mov al, [esp + edx]					; al now stores character to print
			add edx, 4 								; edx += 4			next argument
			call fastOut
			
			jmp T_PRINTF_SPECIAL_OPERATION_END
			T_PRINTF_SO_C_END:
			;---
			
			;---
			; 10 number
			;---
			cmp ah, 'd'
			jne T_PRINTF_SO_D_END
			
			mov eax, [esp + edx]					; eax now stores number to print
			mov ecx, 10d							; setting radix 10
			add edx, 4 								; edx += 4			next argument
			call tPrintNumber
			
			jmp T_PRINTF_SPECIAL_OPERATION_END
			T_PRINTF_SO_D_END:
			;---
			
			;---
			; 8 number
			;---
			cmp ah, 'o'
			jne T_PRINTF_SO_O_END
			
			mov eax, [esp + edx]					; eax now stores number to print
			mov ecx, 8d								; setting radix 8
			add edx, 4 								; edx += 4			next argument
			call tPrintNumber
			
			jmp T_PRINTF_SPECIAL_OPERATION_END
			T_PRINTF_SO_O_END:
			;---
			;---
			; 16 number
			;---
			cmp ah, 'x'
			jne T_PRINTF_SO_X_END
			
			mov eax, [esp + edx]					; eax now stores number to print
			mov ecx, 16d							; setting radix 16
			add edx, 4 								; edx += 4			next argument
			call tPrintNumber
			
			jmp T_PRINTF_SPECIAL_OPERATION_END
			T_PRINTF_SO_X_END:
			;---
			
			;---
			; 2 number
			;---
			cmp ah, 'b'
			jne T_PRINTF_SO_B_END
			
			mov eax, [esp + edx]					; eax now stores number to print
			mov ecx, 2d								; setting radix 2
			add edx, 4 								; edx += 4			next argument
			call tPrintNumber
			
			jmp T_PRINTF_SPECIAL_OPERATION_END
			T_PRINTF_SO_B_END:
			;---
			
			;---
			; % symbol
			;---
			cmp ah, '%'
			jne T_PRINTF_SO_PER_END
			
			mov eax, [esp + edx]					; eax now stores number to print
			mov al, '%'
			call fastOut
			
			jmp T_PRINTF_SPECIAL_OPERATION_END
			T_PRINTF_SO_PER_END:
			;---
			
			;---
			; String
			;---
			cmp ah, 's'
			jne T_PRINTF_SO_S_END
			
			mov eax, [esp + edx]					; eax now stores string to print
			add edx, 4 								; edx += 8 next argument

			call tSimplePrintString

			jmp T_PRINTF_SPECIAL_OPERATION_END
			T_PRINTF_SO_S_END:
			;---
			
			
			
			T_PRINTF_SPECIAL_OPERATION_END:
			;--------------------------------
			;--------------------------------
			
			jmp T_PRINTF_LOOP			; continue loop untill we find 0Ah symbol
			
T_PRINTF_LOOP_END:
			pop edx			; loading registers
			pop ecx			;
			pop ebx			;
			pop eax			;
			ret
;--------------------------------------------------------------------------

				
				
;--------------------------------------------------------------------------
__start:    	push 21h
				push 100d
				push 3802d
				push DWORD SecondString
				push DWORD TestString
				
				call tPrintf
				call fastExit
;--------------------------------------------------------------------------

section .data
FastOutText		db 0, 0Ah, 0
TestString		db "I %s %x %d %% %c", '$'
SecondString	db "Love", '$'


