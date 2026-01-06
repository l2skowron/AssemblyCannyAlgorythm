;-------------------------------------------------------------------------
;.586
;INCLUDE C:\masm32\include\windows.inc 
;.xmm
.const
;Wagi dla formatu BGR: Blue=0.0722, Green=0.7152, Red=0.2126
W_BLUE real4 0.114
W_GREEN real4 0.587
W_RED real4 0.299
.CODE
public ConvertToGrayAsm 
; RCX = input_data
; RDX = output_data
; R8  = size pointer (size.x = width, size.y = height)
; R9  = startY
; [RSP + 96] = endY (pi¹ty argument na stosie)



convertToGrayAsm proc
push rbx
push rsi
push rdi
push r12
push r13
push r14
push r15

; £adowanie argumentów
mov r11d, dword ptr [rsp + 96] ;Pobierz endY ze stosu do rejestru 
mov r12d, dword ptr[r8]; size.x
mov r13d, dword ptr[r8+4]; size.y
mov r14, rcx       ; r14 = sta³a baza input
mov r15, rdx       ; r15 = sta³a baza output
;------------------------------------------------------------------

; Oblicz Stride (d³ugoœæ wiersza w bajtach z paddingiem)
mov eax, r12d       ; eax = width
imul eax, 3         ;width * 3 (iloœæ bajtów na wiersz bez paddingu)
add eax, 3          ; przygotowanie do zaokr¹glenia w górê
and eax, -4         ; wyzerowanie dwóch najm³odszych bitów (wyrównanie do 4)
mov r10d, eax       ; r10d = Stride (faktyczna d³ugoœæ wiersza w bajtach)
;---------------------------------------------------------------
; Za³aduj wagi do rejestrów XMM
movss xmm8, W_BLUE
movss xmm9, W_GREEN
movss xmm10, W_RED


row_loop:
    cmp r9d, r11d
    jge end_all

    ; Adres WEJŒCIOWY(rsi)
    ; rsi= input + (height -1-y) * Stride
    mov eax, r13d           ; eax = height
    dec eax               ; eax = height - 1
    sub eax, r9d           ; eax = height - 1 - y
    imul rax, r10        ; eax = (height - 1 - y) * Stride

    mov rsi, r14           ; rsi = baza input
    add rsi, rax          ; rsi = adres wejœciowego

    ; Adres WYJŒCIOWY(rdi)
    ; Adres = output + (y * width)
    mov eax, r9d           ; r10d = y
    imul rax, r12         ; r10 = y * width
    mov rdi, r15           ; rdi = baza output
    add rdi, rax           ; rdi = adres wyjœciowego 

    xor rbx, rbx            ; rbx = x (licznik pikseli)

pixel_loop:
    cmp ebx, r12d   ; porównujemy x z width
    jge next_row    ; jeœli x >= width, przejdŸ do nastêpnego wiersza

    ; Pobierz BGR (3 bajty)
    movzx eax, byte ptr [rsi]      ; Blue
    cvtsi2ss xmm0, eax             ; konwersja do float
    mulss xmm0, xmm8               ; mno¿enie przez wagê niebieskiego

    movzx eax, byte ptr [rsi + 1]  ; Green
    cvtsi2ss xmm1, eax             ; konwersja do float
    mulss xmm1, xmm9               ; mno¿enie przez wagê zielonego

    addss xmm0, xmm1               ; Dodanie do wyników dla niebieskiego i zielonego pixela
   
    movzx eax, byte ptr [rsi + 2]  ; Red
    cvtsi2ss xmm1, eax             ; konwersja do float
    mulss xmm1, xmm10              ; mno¿enie przez wagê czerwonego
    addss xmm0, xmm1               ; Dodanie do wyniku wartoœci dla czerwonego pixela
    
    ; Konwersja wyniku float do integer
    cvttss2si eax, xmm0             ; Konwersja float do int (wynik w eax)
    mov byte ptr [rdi], al         ; Zapisz 1 bajt do wyjœcia

    add rsi, 3              ; Przesuñ o 3 bajty w wejœciu (BGR)
    inc rdi                 ; Przesuñ o 1 bajt w wyjœciu (Gray)
    inc rbx                 ; Nastêpny piksel
    jmp pixel_loop

next_row:
    inc r9d                 ; Nastêpny wiersz
    jmp row_loop

end_all:
pop r15
pop r14
pop r13
pop r12
pop rdi
pop rsi
pop rbx
ret

ConvertToGrayAsm endp
END 			;no entry point
;-------------------------------------------------------------------------
