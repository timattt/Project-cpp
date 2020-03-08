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
	add eax, '0'
    call fastOut  		 ; Display the remainder
	
	pop ecx				; 
    pop edx				; load registers
	pop eax				; 
    ret
;--------------------------------------------------------------------------



;--------------------------------------------------------------------------
; Standart printf function
; Param: first - string
;		 ... - arguments for this string
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
			cmp ah, 0Ah
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
			; String
			;---
			cmp ah, 's'
			jne T_PRINTF_SO_S_END
			
			mov eax, [esp + edx]					; eax now stores string to print
			add edx, 4 								; edx += 8			next argument
			
			push esp								; saving sp and bp registers 
			push ebp								; to restore safely other registers in the end
			
			push DWORD eax							; invoking printf function to print string
			call tPrintf							;
			
			pop ebp									; flush trash data
			pop ebp									; 
			pop esp									; load sp and bp
			
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
__start:    	push 1234h
				push DWORD SecondString
				push 1234o
				push 1234d
				push 'X'
				push 100h - 1h
				push DWORD TestString
				
				call tPrintf
				
				call fastExit
;--------------------------------------------------------------------------

section .data
FastOutText		db 0, 0Ah, 0
TestString		db "Test printf! %b %c %d %o %s %x abc", 0Ah
SecondString	db "Second", 0Ah


