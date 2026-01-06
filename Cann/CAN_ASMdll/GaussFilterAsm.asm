;----------------------------------------------------
;.586
;.include C:\masm32\include\windows.inc
;.xmm
.const
; Wagi dla filtra Gaussa 5x5, dope³nione do 8 zerami w celu u¿ycia instrukcji wektorowych
; GAUSS KERNEL 5x5
ALIGN 16
	K_W0 word 1 , 4 , 7 , 4 , 1 , 0 , 0 , 0
	K_W1 word 4 ,16 ,26 ,16 , 4 , 0 , 0 , 0
	K_W2 word 7 ,26 ,41 ,26 , 7 , 0 , 0 , 0
	K_W3 word 4 ,16 ,26 ,16 , 4 , 0 , 0 , 0
	K_W4 word 1 , 4 , 7 , 4 , 1 , 0 , 0 , 0 

KernelSum word 273

.CODE
public GaussFilterAsm
; RCX = input_data
; RDX = output_data
; R8  = size pointer (size.x = width, size.y = height)
; R9  = startY
; [RSP + 40] = endY (pi¹ty argument na stosie)
GaussFilterAsm proc
	push rbx
	push rsi
	push rdi
	push rbp
	push r12
	push r13
	push r14
	push r15

	; £adowanie argumentów
	mov r11d, dword ptr [rsp +104] ;Pobierz endY ze stosu do rejestru 
	mov r12d, dword ptr[r8]; size.x
	mov r13d, dword ptr[r8+4]; size.y
	mov r14, rcx       ; r14 = sta³a baza input
	mov r15, rdx       ; r15 = sta³a baza output
	;------------------------------------------------------------------
		
	Y_LOOP:
	cmp r9d, r11d
	jae L_Done
		
	xor r10d, r10d ;r10 -> x=0
		X_LOOP:
		cmp r10d, r12d
		jae L_NextRow
		
		xor ebx, ebx  ;zmienna r z kodu wysokiego poziomu -> akumulator do obliczeñ z j¹drem gaussa
		; pêtle kernela 5x5, mo¿liwoœæ zamiany na 7x7 
		mov rsi ,-2
			I_LOOP:
			mov eax, r9d	;sprawdzenie czy wiersz nie jest poza obrazem nad
			add eax, esi
			test eax, eax	; ustawiamy flagê 
			jns K_RowOk
			xor eax, eax	; jeœli wychodzi poza obraz, to przypisz 0
		K_RowOk:		; zawijamy dodatkowe wartoœci dla wiersza 0
			cmp eax, r13d	; to samo dla wiersza pod obrazem 
			jl K_Continue
			mov eax, r13d  
			dec eax
		K_Continue:
			imul rax, r12 ; eax = y * width
			push rax		; zachowaj bazê wiersza kernela na stosie
			;----------------------------------------------
			;Przygotowanie wag dla bie¿¹cego wiersza kernela
			;----------------------------------------------
			mov rdi, rsi
			add rdi, 2
			; przesuniêcie bitowe w lewo o 4 (mno¿enie przez 16) aby uzyskaæ rozmiar jednego wiersza kernela
			shl rdi, 4 ; r10 -> rozmiar jednego wiersza kernela zdefiniowanego w .const
			lea rcx, K_W0
			movdqa xmm1, [rcx + rdi] ; za³aduj wagi dla bie¿¹cego wiersza kernela do xmm1

			;Pobranie 5 pikseli s¹siednich w poziomie
			xor rdx, rdx 
			mov edi, -2
			L_GetPixels:
				mov ecx, r10d  ; do ecx licznik x
				add ecx, edi  ; x+i
				test ecx, ecx	; ustawiamy flagê jeœli wychodzimy poza zakres
				jns L_PixelOk
				xor ecx, ecx  ;jeœli wychodzimy pobieramy pixele z wiersza 0
			L_PixelOk:
				cmp ecx, r12d
				jl L_Continue
				mov ecx,r12d
				dec ecx
			L_Continue:
				movsxd rcx, ecx
				mov r8, [rsp] ; przywróæ bazê wiersza kernela]
				add rcx, r8	; rcx = (y+i) * width

				movzx eax, byte ptr [r14+rcx] ; pobieramy pixel do eax
				mov r8, rdi
				add r8, 2
				shl r8, 3

				push rcx		; zachowaj offset piksela na stosie
				mov cl , r8b
				shl rax, cl	; przesuwamy pixel do odpowied
				pop rcx			; przywróæ offset piksela ze stosu

				or rdx, rax	; dodajemy pixel do pakietu pixeli w rdx

				inc edi 
				cmp edi, 2
				jle L_GetPixels
				pop rax			; usuñ bazê wiersza kernela ze stosu
				movq xmm0, rdx		; przenosimy pakiet pixeli do rejestru xmm	
				vpmovzxbw xmm0, xmm0 ; roszerzamy bajty ma wordy do mno¿enia
				
				;----------------------
				pmullw xmm0, xmm1 ;mno¿ymy wartoœci razy wagi
				phaddw xmm0, xmm0 ; sumowanie wartoœci ca³ego wiersza
				phaddw xmm0, xmm0 ;
				;----------------------
				pextrw eax, xmm0, 0	
				add ebx, eax     ; dodajemy do akumulatora r
				pextrw eax, xmm0, 1
				add ebx, eax     ; dodajemy do akumulatora r

				inc rsi
				cmp rsi,2
				jle I_LOOP

				; Podziel przez sumê wag kernela
				mov eax, ebx	; za³aduj sumê do eax
				xor edx, edx
				mov r8d, 273
				div r8d           ; eax = eax / KernelSum

				; Zapisz wynik do wyjœcia
				mov rcx, r9    ; rcx = y
				imul rcx, r12 ; rcx = y * width
				add rcx, r10  ; rcx = y * width + x
				mov [r15 + rcx], al ; zapisz wynik do output

				; Nastêpny piksel
				inc r10d		;x++
				jmp X_LOOP
				
				L_NextRow:
				inc r9d		;y++
				jmp Y_LOOP
	
				L_Done:
				pop r15
				pop r14
				pop r13
				pop r12
				pop rbp
				pop rdi
				pop rsi
				pop rbx
				ret
GaussFilterAsm endp
END
