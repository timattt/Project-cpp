.186
.model tiny, c
.code

org 100h


Start:	
		mov al, 9h						; interrupt number
		mov bx, offset TEST_FUNC		; head of the function to wedge
		mov dx, offset TEST_FUNC_RET	; tail of the function where required function to finish interruption will be added
		call MakeInterrupt
		
		
		
				
FRAME_X equ 10
FRAME_Y equ 5

FRAME_WIDTH equ 40
FRAME_HEIGHT equ 15

SHADOW_X equ 22
SHADOW_Y equ 7

VIDEO_SEGMENT_CONST equ 0B800h 

;------------------------------------------------------------------------------
; Inititalizes register es with 0B800h value.
; Destr: bx
; Ret: es
;------------------------------------------------------------------------------
InitEs PROC
	mov bx, VIDEO_SEGMENT_CONST	; es = Video_segm
	mov es, bx			;
	mov bx, 0000h			; bx = 00000h
	ret
InitEs ENDP
;------------------------------------------------------------------------------



;------------------------------------------------------------------------------
; Makes shadow rectngle on the screen.
; Entry: ah - color.
; Destr: ax, bx, cx, dx, es
;------------------------------------------------------------------------------
MakeShadow PROC
	call InitEs	
	xor cx, cx	; i
	xor dx, dx	; j
	mov bx, 2*(80*SHADOW_Y + SHADOW_X)
	
LOOP_MKSH:
	mov byte ptr es:[bx], 0		; set no character
	inc bx				; bx++
	mov byte ptr es:[bx], ah	; paint background
	inc bx				; bx++
	
	inc dl				; dl++
	cmp dl, 40d			; if (dl != 40) then continue
	jne LOOP_MKSH		;

	add bx, 80d			; bx = bx + 80
	
	xor dl, dl			; dl = 0
	inc cl				; cl++
	cmp cl, 15d			; if (cl != 15) then continue
	jne LOOP_MKSH			;
	
	xor ax, ax
	ret
MakeShadow ENDP
;------------------------------------------------------------------------------



;------------------------------------------------------------------------------
; Makes background rectangle.
; Entry: ah - color 
; Destr: ax, bx, cx, dx, es
;------------------------------------------------------------------------------
MakeBackground PROC
	call InitEs
	xor cx, cx			; i
	xor dx, dx			; j
	mov bx, 2 *(FRAME_Y*80 + FRAME_X)
	
LOOP_MKBG:
	mov byte ptr es:[bx], 0		; set no character
	inc bx						; bx++
	mov byte ptr es:[bx], ah	; set background	
	inc bx						; bx++
	
	inc dl						; dl++
	cmp dl, 40d					; if (dl != 40) then continue
	jne LOOP_MKBG			

	add bx, 80d			; bx = bx + 80
	
	xor dl, dl			; dl = 0
	inc cl				; cl++
	cmp cl, 15d			; if (cl != 15) then continue
	jne LOOP_MKBG			;
	xor ax, ax
	ret
MakeBackground ENDP
;------------------------------------------------------------------------------



;------------------------------------------------------------------------------
; Clears all the screen.
; Destr: ax, di, es, cx
;------------------------------------------------------------------------------
ClearScreen PROC
	call InitEs
	mov cx, 25 * 80
	mov di, 0000h
	mov ax, 0000h
	rep stosw
	ret
ClearScreen ENDP
;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
; Makes text in the frame.
; Destr: ax, bx, cx, dx, es
;------------------------------------------------------------------------------
MakeText PROC
	call InitEs
	
	xor dx, dx		; dx = 0 - string byte pointer
	xor cx, cx		; cx = 0 - screen byte pointer

	mov dx, offset MESSAGE	; dx - string start
	mov cx, 2*((FRAME_Y + 1)*80+FRAME_X+1)	;1002d		; top left corner

MakeTextLoop:
	mov bx, dx		; bx points to string
	inc dx			; dx++
	mov al, cs:[bx]		; char from string
	cmp al, 10d		; if the end of string then go away
	je MakeTextRet		;
	
	mov bx, cx			; bx points to screen
	mov ah, byte ptr es:[bx]	; al text symbol from screen

	cmp ah, 186d			; if there is border then go to
	jne SkipNextLine		; the next line
	add bx, 84d			; bx = bx + 84
SkipNextLine:

	mov byte ptr es:[bx], al	; setting character
	inc bx
	mov byte ptr es:[bx], 18d
	inc bx
	
	mov cx, bx			; cx = bx

	jmp MakeTextLoop	

MakeTextRet:

	ret
MakeText ENDP
;------------------------------------------------------------------------------

MESSAGE: db "Test message1! Test Message2! TestMessage3! TestMessage3! TestMessage4!", 10d
	
	
;------------------------------------------------------------------------------
; Draws frame.
; Entry: ah - color
; Destr: ax, bx, cx, dx, es
;------------------------------------------------------------------------------
MakeFrame PROC
		call InitEs
		mov di, 2*(FRAME_Y*80+FRAME_X)			; setting di to begin point
		
		mov al, 205d
		mov cx, FRAME_WIDTH
		rep stosw 
		
		sub di, FRAME_WIDTH * 2
		mov cx, FRAME_HEIGHT
		mov al, 186d
vertical_line:
		stosw
		add di, FRAME_WIDTH * 2	- 4		
		stosw
		add di, 160 - FRAME_WIDTH * 2
		
		dec cx
		cmp cx, 0
		jne vertical_line
		
		mov al, 205d
		sub di, 160
		mov cx, FRAME_WIDTH
		rep stosw
		
	mov byte ptr es:[2*(FRAME_Y*80+FRAME_X)], 201d	; Corners
	mov byte ptr es:[2*(FRAME_Y*80+FRAME_X + FRAME_WIDTH - 1)], 187d	;
	mov byte ptr es:[2*((FRAME_HEIGHT + FRAME_Y - 1)*80+FRAME_X)], 200d	;
	mov byte ptr es:[2*((FRAME_HEIGHT + FRAME_Y - 1)*80+FRAME_X + FRAME_WIDTH - 1)], 188d	;
		
		xor ax, ax
	ret		 		
MakeFrame ENDP
;------------------------------------------------------------------------------
				
		
;------------------------------------------------------------------------------
; function that wedges into interrupt
;------------------------------------------------------------------------------
TEST_FUNC PROC

		push ax bx cx dx di es		

		xor bx, bx
		xor cx, cx
		xor dx, dx
		mov es, bx
		xor ax, ax
		call ClearScreen
		
		mov ah, 64d
		call MakeShadow
		
		mov ah, 16d
		call MakeBackground
		
		mov ah, 080d
		call MakeFrame
		call MakeText	
		
		pop es di dx cx bx ax
		
TEST_FUNC_RET:
		db 0, 0, 0, 0, 0
TEST_FUNC ENDP
;------------------------------------------------------------------------------
;------------------------------------------------------------------------------
		
		
		

		
		
;------------------------------------------------------------------------------
; Entry: al - interrupt index, bx - function head, dx - function tail
; Destr: es, ax, bx, cx, dx
;
;------------------------------------------------------------------------------
MakeInterrupt PROC func_head : word, func_tail : word
		mov func_head, bx		; Saving offsets
		mov func_tail, dx		;
		
		xor bx, bx				; bx = 0
		xor cx, cx				; cx = 0
		
		xor dx, dx		; cx = 0
		mov es, dx		; es = 0
		
		xor ah, ah		; ah = 0
		mov bl, 4d		; bl = 4
		mul bl			; ax = ax * 4

		xor bx, bx		; ax = 0
		xor dx, dx		; dx = 0
		
		; ax stores inter address, bx, cx, dx - are free
		
		; now we create code in the end of our new handler function so it stable finish the interruption. 
		mov cl, 0EAh				;
		mov bx, func_tail			; jmp far 
		mov byte ptr [bx], cl		; 
		
		mov bx, ax					; saving old inter handler value
		mov cx, es:[bx]				;
		mov bx, func_tail			;
		mov word ptr [bx + 1], cx	;
		
		mov bx, ax					; saving cs
		mov cx, es:[bx + 2]			;
		mov bx, func_tail			;
		mov word ptr [bx + 3], cx	;
		
		; now we are moving new inter handler address into inter table
		cli
		mov bx, ax
		mov cx, func_head
		mov word ptr es:[bx], cx
		mov cx, cs
		mov word ptr es:[bx + 2], cx		
		sti
		
		mov ah, 31h				; ah = 31h					;
		mov dx, func_tail		; dx = offset TheEnd		; SHUT DOWN AND STAY RESIDENT
		add dx, 5				; 5 bytes for functions that will help to finish the interruption
		shr dx, 4				; dx /= 16					;
		inc dx					; dx++						;
		int 21h					; 							;
			
		ret
MakeInterrupt ENDP
;------------------------------------------------------------------------------
	
		
		
		
TheEnd:
end Start