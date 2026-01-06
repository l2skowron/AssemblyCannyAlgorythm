
.DATA
ALIGN 16
F_2 REAL4 8 DUP(2.0)
F_0 REAL4 8 DUP(0.0)
F_255 REAL4 8 DUP(255.0)
F_PI REAL4 8 DUP(3.14159)
F_1 REAL4 8 DUP(1.0)
F_028 REAL4 8 DUP(0.28086)

.code
public GradientAsm


; RCX = input, RDX = output, R8 = direction, R9 = size_ptr
; [RSP+40] (after pushes +104) = startY, [RSP+48] (+112) = endY
GradientAsm proc
    push rbp
    mov rbp, rsp
    push rbx
    push rsi
    push rdi
    push r12
    push r13
    push r14
    push r15
    sub rsp, 96              

    mov r12, rcx             ; input
    mov r13, rdx             ; output
    mov r14, r8              ; direction vector
     
    mov ebx, dword ptr [r9]     ; width
    mov edi, dword ptr [r9+4]   ; height
    mov r10d, dword ptr [rbp+48] ; currentY (startY)
    mov r11d, dword ptr [rbp+56] ; endY

Y_LOOP:
    cmp r10d, r11d          ; Obecny y < endY
    jae DONE                ; Jeśli nie, zakończ
    xor r15d, r15d          ; currentX = 0
    

X_LOOP:
; PRZETWARZANIE PIXELI BRZEGOWYCH W SPOSÓB SKALARNY
    test r10d, r10d         ; Sprawdzamy czy r10d (currentY) = 0
    jz SCALAR_X             ; Jeśli tak, idź do SCALAR_X
    mov eax, edi            ; edi = height
    dec eax
    cmp r10d, eax           ; sprawdzamy czy r10d (currentY) = height - 1
    jae YES                  ; jeśli tak, idź do SCALAR_X

    test r15d, r15d         ; Sprawdzamy czy r15d (currentX) = 0
    jz SCALAR_X             ; Jeśli tak, idź do SCALAR_X
    mov eax, ebx            ; ebx = width
    sub eax, 9              ; szerokość - 1 (8 pikseli + 1 na indeks 0)
    cmp r15d, eax           ; sprawdzamy czy r15d (currentX) >= width - 1
    jae SCALAR_X            ; jeśli tak, idź do SCALAR_X
    ; --- WEKTORYZACJA 8 PIXELI ---
    mov eax, r10d           ; rax = currentY
    imul rax, rbx           ; rax = currentY * width
    mov rsi, rax            ; rsi = row start index 
    lea r8, [r12 + rsi]     ; adres bazowy pierwszego piksela w wierszu
    sub r8, rbx             ; odejmujemy -width, aby uzyskać wiersz powyżej (Top)
    lea r9, [r12 + rsi]     ; adres bazowy pierwszego piksela w wierszu(Mid))
    lea rax, [r12 + rsi]    ; adres bazowy pierwszego piksela w wierszu
    add rax, rbx            ; dodajemy +width, aby uzyskać wiersz poniżej (Bot)
;	; Załaduj 8 pikseli z 3 wierszy: Top, Mid, Bot
    vpmovzxbd ymm0, qword ptr [r8 + r15 - 1] ;ładowanie 8 pikseli z wiersza powyżej (Top-Left) -1
     vcvtdq2ps ymm0, ymm0                       ; konwersja do float
    vpmovzxbd ymm1, qword ptr [r8 + r15]     ;ładowanie 8 pikseli z wiersza powyżej (Top)
    vcvtdq2ps ymm1, ymm1                        ; konwersja do float
    vpmovzxbd ymm2, qword ptr [r8 + r15 + 1] ;ładowanie 8 pikseli z wiersza powyżej (Top-Right)+1
    vcvtdq2ps ymm2, ymm2                        ; konwersja do float

    vpmovzxbd ymm3, qword ptr [r9 + r15 - 1] ;ładowanie 8 pixeli z wiersza środkowego (Mid-Left) -1
    vcvtdq2ps ymm3, ymm3                        ; konwersja do float
    vpmovzxbd ymm4, qword ptr [r9 + r15 + 1] ;ładowanie 8 pixeli z wiersza środkowego (Mid-Right) +1
    vcvtdq2ps ymm4, ymm4

    vpmovzxbd ymm5, qword ptr [rax + r15 - 1] ;ładowanie 8 pixeli z wiersza poniżej (Bot-Left) -1
    vcvtdq2ps ymm5, ymm5                        ; konwersja do float
    vpmovzxbd ymm6, qword ptr [rax + r15]     ;ładowanie 8 pixeli z wiersza poniżej (Bot)
    vcvtdq2ps ymm6, ymm6                        ; konwersja do float
    vpmovzxbd ymm7, qword ptr [rax + r15 + 1] ;ładowanie 8 pixeli z wiersza poniżej (Bot-Right) +1
    vcvtdq2ps ymm7, ymm7                        ; konwersja do float

    ; --- OBLICZENIA GRADIENTU ---
    ; Przy obliczaniu gradientu poziomega istotne są wagi kolumn zewnętrznych
    ; dla Gx: -1, 0, 1 (Top), -2, 0, 2 (Mid), -1, 0, 1 (Bot) 
    ; ponieważ jak widać , środkowe kolumny mają wagę 0 i nie wpływają na wynik
    ; stąd pomijamy je w obliczeniach
    vsubps ymm8, ymm2, ymm0                     ; Gx = Top-Right - Top-Left
    ;W pierwszym wierszu pixele mają wagę 1 i -1, co zapewniamy odejmowaniem
    vsubps ymm11, ymm4, ymm3                    ; Gx += Mid-Right - Mid-Left
    vmulps ymm11, ymm11, YMMWORD PTR [F_2]      ; Gx += 2 * (Mid-Right - Mid-Left)
    ;W drugim wierszu pixele mają wagę 2 i -2, co zapewniamy mnożeniem przez 2
    vaddps ymm8, ymm8, ymm11                    ; Gx += poprzedni wynik
    ;W trzecim wierszu pixele mają wagę 1 i -1, co zapewniamy odejmowaniem
    vsubps ymm11, ymm7, ymm5                    ; Gx = Bot-Right - Bot-Left
    vaddps ymm8, ymm8, ymm11                    ; Gx += poprzedni wynik
    ;Teraz w rejestrze ymm8 mamy Gx dla 8 pikseli

    ; Przy obliczaniu gradientu pionowego sytuacja jest ciekawsza
    ; dla Gy: 1, 2, 1 (Top), 0, 0, 0 (Mid), -1, -2, -1 (Bot)
    ; stąd pomijamy środkowy wiersz w obliczeniach

    vaddps ymm9, ymm0, ymm2                     ; Gy = Top-Left + Top-Right
    vmulps ymm11, ymm1, YMMWORD PTR [F_2]       ; Gy += 2 * Top
    vaddps ymm9, ymm9, ymm11                    ; Gy += poprzedni wynik
    
    vaddps ymm12, ymm5, ymm7                    ; Suma-Bot = Bot-Left + Bot-Right
    vmulps ymm11, ymm6, YMMWORD PTR [F_2]       ; Dla pixela środkowego += 2 * Bot
    vaddps ymm12, ymm12, ymm11                  ; Suma-Bot += poprzedni wynik
    
    vsubps ymm9, ymm9, ymm12                    ; Gy = Gy - (Bot sum)
    ;Teraz w rejestrze ymm9 mamy Gy dla 8 pikseli
    ; --- OBLICZENIE AMPLITUDY I KĄTA ---
    vmulps ymm11, ymm8, ymm8                    ; Gx^2
    vmulps ymm12, ymm9, ymm9                    ; Gy^2
    vaddps ymm10, ymm11, ymm12                  ; Gx^2 + Gy^2
    vsqrtps ymm10, ymm10                        ; sqrt(Gx^2 + Gy^2)

    ;Jeśli wartość jest mniejsza niż 0, ustawiamy ją na 0
    vmaxps ymm10, ymm10, YMMWORD PTR [F_0]      ; Clamp min 0
    ;Jeśli wartość jest większa niż 255, ustawiamy ją na 255
    vminps ymm10, ymm10, YMMWORD PTR [F_255]    ; Clamp max 255
    vcvttps2dq ymm10, ymm10                     ; Konwersja do int32

    vextracti128 xmm11, ymm10, 1                ; Pobierz górne 4 int32
    vpackssdw xmm10, xmm10, xmm11               ; Spakuj do int16
    vpackuswb xmm10, xmm10, xmm10               ; Spakuj do uint8
    ;Obliczenie indeksu wyjściowego: rax = y * width + x
    mov rax, r10                               ;rax = currentY
    imul rax, rbx                              ;rax = currentY * width
    add rax, r15                               ;rax = rax + currentX
    vmovq qword ptr [r13 + rax], xmm10          ; Zapis 8 bajtów amplitudy
 
    ; --- Obliczanie atan ---
    ;Obliczanamy przybliżając wartość atan(z) gdzie z = Gy / Gx
    ;Wzór przybliżenia: atan(z) ≈ z / (0.28086 * z^2 + 1) dla |z| <= 1
    vdivps ymm10, ymm9, ymm8 				  ; z = Gy / Gx
    
    vmulps ymm11, ymm10, ymm10                  ; z^2
    vmulps ymm11, ymm11, YMMWORD PTR [F_028]    ; z^2 * 0.28086
    vaddps ymm11, ymm11, YMMWORD PTR [F_1]      ; z^2 * 0.28086 + 1.0
    vdivps ymm10, ymm10, ymm11                  ; atan approx

    ; Dodaj PI jeśli kąt jest ujemny
    vxorps ymm11, ymm11, ymm11                  ; domyślnie 0.0
    vcmpltps ymm12, ymm10, ymm11                ; if (angle < 0) 

    vandps ymm13, ymm12, YMMWORD PTR [F_PI]     ; angle += PI jeśli był ujemny
    vaddps ymm10, ymm10, ymm13                  ; Dodajemy do kąta
    ; Teraz w ymm10 mamy kąty dla 8 pikseli
    mov rax, r10                                ;rax = currentY
    imul rax, rbx                               ;rax = currentY * width
    add rax, r15                                ;rax = rax + currentX
    vmovups YMMWORD PTR [r14 + rax * 4], ymm10  ; Zapis 8 kątów do wektora kierunków

    add r15d, 8                                 ; Przejdź do następnych 8 pikseli
    jmp X_LOOP
;---------------------------------------------------
; PRZETWARZANIE PIXELI BRZEGOWYCH W SPOSÓB SKALARNY
;---------------------------------------------------

SCALAR_X:
    ; Sprawdzamy czy r15d (currentX) < width
    mov eax, ebx
    cmp r15d, eax
    jae NEXT_Y

    ; Przygotowujemy rejestry na akumulację Gx i Gy (jako float)
    pxor xmm4, xmm4          ; Gx = 0
    pxor xmm5, xmm5          ; Gy = 0

    
    mov r11, -1              
LOOP_J:
    ; py = clamp(y + j, 0, height - 1)
    mov eax, r10d           ; eax = currentY
    add eax, r11d           ; eax = y + j
    test eax, eax           ; sprawdzamy czy y + j < 0
    js PY_ZERO              ; jeśli tak, py = 0
    mov edx, edi            ; edi = height
    dec edx                 ; edx = height - 1
    cmp eax, edx            ; porównaj y + j z height - 1
    jg PY_MAX               ; jeśli y + j > height - 1, py = height - 1
    jmp PY_OK  
PY_ZERO: 
    xor eax, eax     
    jmp PY_OK
PY_MAX:  
    mov eax, edi     
    dec eax
PY_OK:   
    mov r8d, eax        ; r8d = py

    mov r9, -1               ; r9 = i (kolumny)
LOOP_I:
    ; px = clamp(x + i, 0, width - 1)
    mov eax, r15d       ; eax = currentX
    add eax, r9d        ; eax = x + i
    test eax, eax       ; sprawdzamy czy x + i < 0
    js PX_ZERO          ; jeśli tak, px = 0
    mov edx, ebx        ; ebx = width
    dec edx             ; edx = width - 1
    cmp eax, edx        ; porównaj x + i z width - 1
    jg PX_MAX           ; jeśli x + i > width - 1, px = width - 1
    jmp PX_OK           
PX_ZERO: 
    xor eax, eax     
    jmp PX_OK
PX_MAX:  
    mov eax, ebx       
    dec eax
PX_OK:   
    mov ecx, eax        ; ecx = px

    ; Pobierz pIndex = input[py * width + px]
    mov rax, r8                         ; rax = py
    imul rax, rbx                       ; rax = py * width
    add rax, rcx                        ; rax = py * width + px
    movzx edx, byte ptr [r12 + rax]     ; edx = pIndex (uint8) 
    cvtsi2ss xmm0, edx                  ; xmm0 = pIndex (float)

    ; --- WAGI SOBELA (Sx i Sy) ---
    ; Oblicz Gx += pIndex * Sx
    ; Sx: {{-1,0,1}, {-2,0,2}, {-1,0,1}} dla i+1, j+1
    mov eax, r9d            ;eax = i
    test eax, eax           ;sprawdzamy czy i = 0
    jz SKIP_GX              ; jeśli tak, pomiń
    js GX_MINUS             ; skok jeśli i = -1 (lewy kolumna)
    ; Jeśli i = 1
    mov eax, 1              ; waga Sx dla i=1
    cmp r11d, 0             ; jeśli j=0, waga * 2
    jne GX_ADD              ; jeśli j != 0, pomiń
    mov eax, 2              ; waga Sx dla i=1, j=0
GX_ADD:
    cvtsi2ss xmm1, eax      ; konwersja wagi do float
    mulss xmm1, xmm0        ; mnożenie przez pIndex
    addss xmm4, xmm1        ; dodaj do Gx
    jmp SKIP_GX             
GX_MINUS:
    mov eax, 1              ; waga Sx dla i=-1
    cmp r11d, 0             ; jeśli j=0, waga * 2
    jne GX_SUB              ; jeśli j != 0, pomiń
    mov eax, 2              ; waga Sx dla i=-1, j=0
GX_SUB:
    cvtsi2ss xmm1, eax      ; konwersja wagi do float
    mulss xmm1, xmm0        ; mnożenie przez pIndex
    subss xmm4, xmm1        ; odejmij od Gx
SKIP_GX:

    ; Oblicz Gy += pIndex * Sy
    ; Sy: {{1,2,1}, {0,0,0}, {-1,-2,-1}} dla i+1, j+1
    mov eax, r11d            ; waga Sy zależy od j
    test eax, eax            ; sprawdzamy czy j = 0
    jz SKIP_GY               ; jeśli tak, pomiń
    js GY_PLUS               ; j = -1 to górny wiersz (dodatni w Twoim Sy)
    ; Jeśli j = 1 (dolny wiersz)
    mov eax, 1              ; waga Sy dla j=1
    cmp r9d, 0              ; jeśli i=0, waga * 2
    jne GY_SUB_Y            ; jeśli i != 0, pomiń
    mov eax, 2              ; waga Sy dla j=1, i=0
GY_SUB_Y:
    cvtsi2ss xmm1, eax      ; konwersja wagi do float
    mulss xmm1, xmm0        ; mnożenie przez pIndex
    subss xmm5, xmm1        ; odejmij od Gy
    jmp SKIP_GY             
GY_PLUS:        
    mov eax, 1              ; waga Sy dla j=-1
    cmp r9d, 0              ; jeśli i=0, waga * 2
    jne GY_ADD_Y            ; jeśli i != 0, pomiń
    mov eax, 2              ; waga Sy dla j=-1, i=0
GY_ADD_Y:
    cvtsi2ss xmm1, eax      ; konwersja wagi do float
    mulss xmm1, xmm0        ; mnożenie przez pIndex
    addss xmm5, xmm1        ; dodaj do Gy
SKIP_GY:            
    
    inc r9                  ;i++
    cmp r9, 1               ;jeśli i <= 1
    jle LOOP_I              ; wróć do LOOP_I
    inc r11                 ;j++
    cmp r11, 1              ;jeśli j <= 1
    jle LOOP_J              ; wróć do LOOP_J

    ; --- WYNIK KOŃCOWY ---
    ; xmm4 = gradX, xmm5 = gradY
     movss xmm0, xmm4          ; Obliczenie amplitudy
    mulss xmm0, xmm0         ; Gx^2
    movss xmm1, xmm5         
    mulss xmm1, xmm1         ; Gy^2
    addss xmm0, xmm1            ; Gx^2 + Gy^2
    sqrtss xmm0, xmm0        ; gradient = sqrt(Gx^2 + Gy^2)

    ; Clamp i konwersja do uint8
    minss xmm0, dword ptr [F_255]   ;Jeśli wartość jest większa niż 255, ustawiamy ją na 255
    maxss xmm0, dword ptr [F_0]     ;Jeśli wartość jest mniejsza niż 0, ustawiamy ją na 0
    cvttss2si eax, xmm0             ; konwersja do int32
    
    ; Obliczenie indeksu wyjściowego: r8 = y * width + x
    mov r8, r10                     ;r8 = currentY
    imul r8, rbx                    ;r8 = currentY * width
    add r8, r15                     ;r8 = r8 + currentX
    
    ; Zapis amplitudy (1 bajt na piksel)
    mov byte ptr [r13 + r8], al    ; Zapis amplitudy

    ; --- BEZPIECZNY ATAN2 ---
    pxor xmm6, xmm6                 ; domyślnie 0.0
    comiss xmm4, dword ptr [F_0]    ; testujemy Gx
    jz STORE_ANGLE_SCALAR           ; jeśli Gx == 0, zapisz 0.0 i kontynuuj
    
    divss xmm5, xmm4                ; z = Gy/Gx
    movss xmm6, xmm5                ;  kopia z do obliczeń
    mulss xmm6, xmm6                ; z^2
    mulss xmm6, dword ptr [F_028]   ; z^2 * 0.28086
    addss xmm6, dword ptr [F_1]     ; z^2 * 0.28086 + 1.0
    divss xmm5, xmm6                ; atan approx
    
    pxor xmm6, xmm6                 ; domyślnie 0.0
    comiss xmm5, xmm6               ; if (angle < 0)
    jae SKIP_PI_ADD                 ; jeśli nie, pomiń
    addss xmm5, dword ptr [F_PI]    ; angle += PI
SKIP_PI_ADD:
    movss xmm6, xmm5                ; kopia kąta do zapisu

STORE_ANGLE_SCALAR:
    ; Zapis kąta 
    movss dword ptr [r14 + r8*4], xmm6 

    inc r15d        ; currentX++
    jmp SCALAR_X
NEXT_Y:
    inc r10d        ; currentY++
    jmp Y_LOOP


YES: 
    jmp SCALAR_X
DONE:
    add rsp, 96
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbx
    pop rbp
    ret
GradientAsm endp
END