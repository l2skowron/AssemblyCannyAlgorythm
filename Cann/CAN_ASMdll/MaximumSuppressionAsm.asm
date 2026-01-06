.const

.data
    align 16
    prog1 dd 0.392699987, 0.392699987, 0.392699987, 0.392699987, 0.392699987, 0.392699987, 0.392699987, 0.392699987
    prog2 dd 1.17709994, 1.17709994, 1.17709994, 1.17709994, 1.17709994, 1.17709994, 1.17709994, 1.17709994
    prog3 dd 1.96350002, 1.96350002, 1.96350002, 1.96350002, 1.96350002, 1.96350002, 1.96350002, 1.96350002
    prog4 dd 2.74889994, 2.74889994, 2.74889994, 2.74889994, 2.74889994, 2.74889994, 2.74889994, 2.74889994
    prog5 dd 3.14159989, 3.14159989, 3.14159989, 3.14159989, 3.14159989, 3.14159989, 3.14159989, 3.14159989
    prog1_scalar dd 0.392699987
    prog2_scalar dd 1.17709994
    prog3_scalar dd 1.96350002
    prog4_scalar dd 2.74889994
    prog5_scalar dd 3.14159989
.code
public NonMaximumSuspensionAsm


NonMaximumSuspensionAsm proc
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
    movsxd rbx, dword ptr [r9]     ; width

    mov edi, dword ptr [r9+4]       ; height
    mov r10d, dword ptr [rbp+48] ; currentY (startY)
    mov r11d, dword ptr [rbp+56] ; endY
    
    
    mov ebp, edi 		 ; height

    Y_LOOP:
    cmp r10d, r11d           ;Obecny Y < endY?
    jae DONE                 ; jesli nie, zakoncz
    cmp r10d, 0			 ;Obecny Y = 0?
    je Y_EDGE
    mov r8d, ebp
    dec r8d                  ; endY-1
    cmp r10d, r8d            ;Obecny Y = height-1?
    je Y_EDGE
    

    xor r15d, r15d          ; X = 0
    jmp FIRST_X_PIXEL
    X_LOOP:
    mov r8d, ebx
    cmp r15d, ebx           ; X < width?
    jae NEXT_Y              ; jesli nie, przejdz do nastepnego Y
    dec r8d                  ; width-1
    cmp r15d, r8d           ; X = width-1?
    je SET_0
    cmp r15d, 0              ; X = 0?
    je SET_0
    mov r9d, ebx             ; width
    sub r9d, 33             ; width-32, bo przetwarzamy po 32 pixele na raz
    cmp r15d, r9d           ; X <= width-32?
    jae SKALAR_X_LOOP         ; jesli width < 32, przejdz do przetwarzania skalarnego

    ;Przetwarzanie pixeli wewnetrznych.
    ;Ustawiamy obecny index
    mov rax, r10           ; Y
    imul rax, rbx           ; Y * width
    add rax, r15           ; index = Y * width + X
    ;Pobierz wartosc kierunku
    vmovups ymm0, [r14 + rax*4]     ;Pobieranie 8 wartoœci z wektora kierunku
    vmovups ymm1, [r14 + rax*4 + 32] ;Pobieranie kolejnych 8 wartoœci z wektora kierunku
    vmovups ymm2, [r14 + rax*4 + 64] ;Pobieranie kolejnych 8 wartoœci z wektora kierunku
    vmovups ymm3, [r14 + rax*4 + 96] ;Pobieranie kolejnych 8 wartoœci z wektora kierunku
    ;Teraz mamy 32 wartosci kierunku w ymm0, ymm1, ymm2, ymm3
    vmovups ymm11, prog1     ;Wczytanie prog1 do ymm11
    vmovups ymm12, prog2    ;Wczytanie prog2 do ymm12
    vmovups ymm13, prog3    ;Wczytanie prog3 do ymm13
    vmovups ymm14, prog4    ;Wczytanie prog4 do ymm14
    vmovups ymm15, prog5    ;Wczytanie prog5 do ymm15
    

    ;Poniewa¿ przetwarzamy po 32 pixele na raz.
    ;Musimy stworzyc maski dla kazdego z progow.
    ;Pierwszy prog to: 
    ;(direction < prog1) || (direction >= prog4 && direction <= prog5)
    vcmpleps ymm4, ymm0, ymm11 ; ymm12 = (direction <= prog1) 
    vcmpgtps ymm5, ymm0, ymm14 ; ymm14 = (direction >= prog4) 
    vcmpleps ymm6, ymm0, ymm15 ; ymm15 = (direction <= prog5)
    vpand ymm5, ymm5, ymm6 ; ymm5 = (direction >= prog4) && (direction <= prog5)
    vpor ymm4, ymm4, ymm5 ; ymm4 = (direction < prog1) || (direction >= prog4 && direction <= prog5)
    
    vcmpleps ymm5, ymm1, ymm11 ; ymm12 = (direction <= prog1) ? 0xFFFFFFFF : 0
    vcmpgtps ymm6, ymm1, ymm14 ; ymm14 = (direction >= prog4)
    vcmpleps ymm7, ymm1, ymm15 ; ymm15 = (direction <= prog5)
    vpand ymm6, ymm6, ymm7 ; ymm6 = (direction >= prog4) && (direction <= prog5)
    vpor ymm5, ymm5, ymm6 ; ymm5 = (direction < prog1) || (direction >= prog4 && direction <= prog5)

    vcmpleps ymm6, ymm2, ymm11 ; ymm12 = (direction <= prog1) ? 0xFFFFFFFF : 0
    vcmpgtps ymm7, ymm2, ymm14 ; ymm14 = (direction >= prog4)
    vcmpleps ymm8, ymm2, ymm15 ; ymm15 = (direction <= prog5)
    vpand ymm7, ymm7, ymm8 ; ymm7 = (direction >= prog4) && (direction <= prog5)
    vpor ymm6, ymm6, ymm7 ; ymm6 = (direction < prog1) || (direction >= prog4 && direction <= prog5)
    
    vcmpleps ymm7, ymm3, ymm11 ; ymm12 = (direction <= prog1) ? 0xFFFFFFFF : 0
    vcmpgtps ymm8, ymm3, ymm14 ; ymm14 = (direction >= prog4)
    vcmpleps ymm9, ymm3, ymm15 ; ymm15 = (direction <= prog5)
    vpand ymm8, ymm8, ymm9 ; ymm8 = (direction >= prog4) && (direction <= prog5)
    vpor ymm7, ymm7, ymm8 ; ymm7 = (direction < prog1) || (direction >= prog4 && direction <= prog5)

   
    ;Kolejnym etapem jest pakowanie masek do jednego rejestru.

    vpackssdw ymm4, ymm4, ymm5 ; Pakowanie pierwszych 16 bitów
    vpermq ymm4, ymm4, 11011000b ; Przestawienie bajtów w odpowiedniej kolejnoœci
    vpackssdw ymm6, ymm6, ymm7 ; Pakowanie kolejnych 16 bitów
    vpermq ymm6, ymm6, 11011000b ; Przestawienie bajtów w odpowiedniej kolejnoœci
    vpacksswb ymm4, ymm4, ymm6 ; Pakowanie do jednego rejestru
    vpermq ymm4, ymm4, 11011000b ; Przestawienie bajtów w odpowiedniej kolejnoœci
    ; Teraz mamy maskê dla pierwszego progu w ymm4
    ; Powtarzamy proces dla drugiego progu

    vcmpgtps ymm5, ymm0, ymm11 ; ymm12 = (direction > prog1)
    vcmpleps ymm6, ymm0, ymm12 ; ymm12 = (direction <= prog2)
    vpand ymm5, ymm5, ymm6 ; ymm5 = (direction > prog1) && (direction <= prog2)

    vcmpgtps ymm6, ymm1, ymm11 ; ymm12 = (direction > prog1)
    vcmpleps ymm7, ymm1, ymm12 ; ymm12 = (direction <= prog2)
    vpand ymm6, ymm6, ymm7 ; ymm6 = (direction > prog1) && (direction <= prog2)

    vcmpgtps ymm7, ymm2, ymm11 ; ymm12 = (direction > prog1)
    vcmpleps ymm8, ymm2, ymm12 ; ymm12 = (direction <= prog2)
    vpand ymm7, ymm7, ymm8 ; ymm7 = (direction > prog1) && (direction <= prog2)

    vcmpgtps ymm8, ymm3, ymm11 ; ymm12 = (direction > prog1)
    vcmpleps ymm9, ymm3, ymm12 ; ymm12 = (direction <= prog2)
    vpand ymm8, ymm8, ymm9 ; ymm8 = (direction > prog1) && (direction <= prog2)

    vpackssdw ymm5, ymm5, ymm6 ; Pakowanie pierwszych 16 bitów
    vpermq ymm5, ymm5, 11011000b ; Przestawienie bajtów w odpowiedniej kolejnoœci
    vpackssdw ymm7, ymm7, ymm8 ; Pakowanie kolejnych 16 bitów
    vpermq ymm7, ymm7, 11011000b ; Przestawienie bajtów w odpowiedniej kolejnoœci
    vpacksswb ymm5, ymm5, ymm7 ; Pakowanie do jednego rejestru
    vpermq ymm5, ymm5, 11011000b ; Przestawienie bajtów w odpowiedniej kolejnoœci
    ; Teraz mamy maskê dla drugiego progu w ymm5
    ; Powtarzamy proces dla trzeciego progu

    vcmpgtps ymm6, ymm0, ymm12 ; ymm13 = (direction > prog2)
    vcmpleps ymm7, ymm0, ymm13 ; ymm14 = (direction <= prog3)
    vpand ymm6, ymm6, ymm7 ; ymm6 = (direction > prog2) && (direction <= prog3)

    vcmpgtps ymm7, ymm1, ymm12 ; ymm13 = (direction > prog2)
    vcmpleps ymm8, ymm1, ymm13 ; ymm14 = (direction <= prog3)
    vpand ymm7, ymm7, ymm8 ; ymm7 = (direction > prog2) && (direction <= prog3)

    vcmpgtps ymm8, ymm2, ymm12 ; ymm13 = (direction > prog2)
    vcmpleps ymm9, ymm2, ymm13 ; ymm14 = (direction <= prog3)
    vpand ymm8, ymm8, ymm9 ; ymm8 = (direction > prog2) && (direction <= prog3)

    vcmpgtps ymm9, ymm3, ymm12 ; ymm13 = (direction > prog2)
    vcmpleps ymm10, ymm3, ymm13 ; ymm14 = (direction <= prog3)
    vpand ymm9, ymm9, ymm10 ; ymm9 = (direction > prog2) && (direction <= prog3)

    vpackssdw ymm6, ymm6, ymm7 ; Pakowanie pierwszych 16 bitów
    vpermq ymm6, ymm6, 11011000b ; Przestawienie bajtów w odpowiedniej kolejnoœci
    vpackssdw ymm8, ymm8, ymm9 ; Pakowanie kolejnych 16 bitów
    vpermq ymm8, ymm8, 11011000b ; Przestawienie bajtów w odpowiedniej kolejnoœci
    vpacksswb ymm6, ymm6, ymm8 ; Pakowanie do jednego rejestru
    vpermq ymm6, ymm6, 11011000b ; Przestawienie bajtów w odpowiedniej kolejnoœci
    ; Teraz mamy maskê dla drugiego progu w ymm5
    ; Trzeci próg

    vcmpgtps ymm7, ymm0, ymm13 ; ymm12 = (direction > prog3)
    vcmpleps ymm8, ymm0, ymm14 ; ymm13 = (direction <= prog4)
    vpand ymm7, ymm7, ymm8 ; ymm7 = (direction > prog3) && (direction <= prog4)

    vcmpgtps ymm8, ymm1, ymm13 ; ymm12 = (direction > prog3)
    vcmpleps ymm9, ymm1, ymm14 ; ymm13 = (direction <= prog4)
    vpand ymm8, ymm8, ymm9 ; ymm8 = (direction > prog3) && (direction <= prog4)

    vcmpgtps ymm9, ymm2, ymm13 ; ymm12 = (direction > prog3)
    vcmpleps ymm10, ymm2, ymm14 ; ymm13 = (direction <= prog4)
    vpand ymm9, ymm9, ymm10 ; ymm9 = (direction > prog3) && (direction <= prog4)

    vcmpgtps ymm10, ymm3, ymm13 ; ymm12 = (direction > prog3)
    vcmpleps ymm11, ymm3, ymm14 ; ymm13 = (direction <= prog4)
    vpand ymm10, ymm10, ymm11 ; ymm10 = (direction > prog3) && (direction <= prog4)

    vpackssdw ymm7, ymm7, ymm8 ; Pakowanie pierwszych 16 bitów
    vpermq ymm7, ymm7, 11011000b ; Przestawienie bajtów w odpowiedniej kolejnoœci
    vpackssdw ymm9, ymm9, ymm10 ; Pakowanie kolejnych 16 bitów
    vpermq ymm9, ymm9, 11011000b ; Przestawienie bajtów w odpowiedniej kolejnoœci
    vpacksswb ymm7, ymm7, ymm9 ; Pakowanie do jednego rejestru
    vpermq ymm7, ymm7, 11011000b ; Przestawienie bajtów w odpowiedniej kolejnoœci
    ; Teraz mamy maskê dla trzeciego progu w ymm7
    ; Wszystkie maski sa gotowe.
    ; AKTUALNE PRZETWARZANIE PIXELI
    vmovdqu ymm0,YMMWORD PTR [r12 + rax]     ;currA
    ; Teraz pobieramy wartosc pixeli sasiadujacych w poziomie
    vmovdqu ymm1, YMMWORD PTR [r12 + rax-1] ;befC
    vmovdqu ymm2, YMMWORD PTR [r12 + rax+1] ;nextC

    ;Sprawdzamy czy s¹siedzi s¹ mniejsi od currA
    vpmaxub  ymm9, ymm0, ymm1 ; currA > befC
    vpcmpeqb ymm9, ymm9, ymm0 ; Porównanie maksymalnej wartoœci z currA
    vpmaxub ymm10, ymm0, ymm2 ; currA > nextC
    vpcmpeqb ymm10, ymm10, ymm0 ; Porównanie maksymalnej wartoœci z currA

    vpand ymm9, ymm9, ymm10 ;  £¹czenie wyników porównañ
    vpand ymm9, ymm9, ymm4 ;  Zastosowanie maski pierwszego progu
   
    ;Pobieramy s¹siadów pod k¹tem 45 stopni
    mov rsi, rbx
    add rsi, rax
    vmovdqu ymm1, YMMWORD PTR [r12 + rsi -1] ;befC
    mov rsi, rax
    sub rsi, rbx
    vmovdqu ymm2, YMMWORD PTR [r12 + rsi +1] ;nextC
    vpmaxub ymm10, ymm0, ymm1 ; currA > befC
    vpcmpeqb ymm10, ymm10, ymm0 ; Porównanie maksymalnej wartoœci z currA
    vpmaxub ymm11, ymm0, ymm2 ; currA > nextC
    vpcmpeqb ymm11, ymm11, ymm0 ; Porównanie maksymalnej wartoœci z currA
    vpand ymm10, ymm10, ymm11 ;  £¹czenie wyników porównañ
    vpand ymm10, ymm10, ymm5 ;  Zastosowanie maski drugiego progu
    
    ; Pobieramy s¹siadów pod k¹tem 90 stopni
    mov rsi, rax
    sub rsi, rbx
    vmovdqu ymm1, YMMWORD PTR [r12 + rsi] ;befC
    mov rsi, rbx
    add rsi, rax
    vmovdqu ymm2, YMMWORD PTR [r12 + rsi] ;nextC
    vpmaxub ymm11, ymm0, ymm1 ; currA > befC
    vpcmpeqb ymm11, ymm11, ymm0 ; Porównanie maksymalnej wartoœci z currA
    vpmaxub ymm12, ymm0, ymm2 ; currA > nextC
    vpcmpeqb ymm12, ymm12, ymm0 ; Porównanie maksymalnej wartoœci z currA
    vpand ymm11, ymm11, ymm12 ;  £¹czenie wyników porównañ
    vpand ymm11, ymm11, ymm6 ;  Zastosowanie maski trzeciego progu

    ; Pobieramy s¹siadów pod k¹tem 135 stopni
    mov rsi, rax
    sub rsi, rbx
    vmovdqu ymm1, YMMWORD PTR [r12 + rsi -1] ;befC
    mov rsi, rbx
    add rsi, rax
    vmovdqu ymm2, YMMWORD PTR [r12 + rsi +1] ;nextC
    vpmaxub ymm12, ymm0, ymm1 ; currA > befC
    vpcmpeqb ymm12, ymm12, ymm0 ; Porównanie maksymalnej wartoœci z currA   
    vpmaxub ymm13, ymm0, ymm2 ; currA > nextC
    vpcmpeqb ymm13, ymm13, ymm0 ; Porównanie maksymalnej wartoœci z currA
    vpand ymm12, ymm12, ymm13 ;  £¹czenie wyników porównañ
    vpand ymm12, ymm12, ymm7 ;  Zastosowanie maski czwartego progu
    ;£¹czenie wszystkich wyników

    vpor ymm9, ymm9, ymm10 ; £¹czenie wyników pierwszego i drugiego progu
    vpor ymm9, ymm9, ymm11 ; £¹czenie z wynikiem trzeciego progu
    vpor ymm9, ymm9, ymm12 ; £¹czenie z wynikiem czwartego progu
    ; w ymm9 mamy ostateczna maske pixeli do zachowania
    ;NAK£ADANIE MASKI NA PIXELE
    vpand ymm9, ymm9, ymm0 ; Zastosowanie maski do oryginalnych wartoœci pixeli
    ; Teraz zapisujemy wynik do output
    vmovdqu YMMWORD PTR [r13 + rax], ymm9

    add r15d, 32                 ; X += 32, bo przetwarzamy 32 pixele na raz                 
    jmp X_LOOP

    NEXT_Y:
    inc r10d                ; currentY++
    jmp Y_LOOP
    
    ;Przetwarzanie skalarne dla pozostalych pixeli w wierszu
    SKALAR_X_LOOP:
    cmp r15d, ebx           ; X < width?
    jae NEXT_Y              ; jesli nie, przejdz do nastepnego Y
    ;Sprawdzamy czy pixel brzegowy
    mov rax, r10           ; Y
    imul rax, rbx           ; Y * width
    add rax, r15           ; index = Y * width + X
    test r15d, r15d      ; X = 0?
    jz SET_0
    mov r8d, ebx
    dec r8d                  ; width-1
    cmp r15d, r8d           ; X = width-1?
    je SET_0
    ;Przetwarzanie skalarne pixeli wewnetrznych.
    movss xmm0, dword ptr [r14 + rax*4] ;Pobieranie wartosci kierunku
    ;Sprawdzanie progów
    mov dil, byte ptr [r12 + rax] ;currA
   
    ;Sprawdzamy progi
    ;Pierwszy próg
    movss xmm1, [prog1]
    ucomiss xmm0, xmm1
    jbe TEST_0
    movss xmm1, [prog4]
    ucomiss xmm0, xmm1
    jb CHECK_SECOND_THRESHOLD
    
    movss xmm1, [prog5]
    ucomiss xmm0, xmm1
    jbe TEST_0
    jmp SET_0
 
    CHECK_SECOND_THRESHOLD:
    ;Drugi próg (45 stopni)
    movss xmm1, [prog2]
    ucomiss xmm0, xmm1
    jbe TEST_45
    ;Trzeci próg (90 stopni)
    movss xmm1, [prog3]
    ucomiss xmm0, xmm1
    jbe TEST_90
    ;Czwarty próg (135 stopni)
    movss xmm1, [prog4]
    ucomiss xmm0, xmm1
    jbe TEST_135
    
    TEST_0:
    ;Pobieramy s¹siadów w poziomie
    mov cl, byte ptr [r12 + rax -1] ;befC
    cmp cl, dil 
    ja SET_0
    mov dl,  byte ptr [r12 + rax +1] ;nextC
    cmp dl, dil
    ja SET_0
    jmp KEEP_PIXEL

    TEST_45:
    ;Pobieramy s¹siadów pod k¹tem 45 stopni
    mov rsi, rax
    add rsi, rbx
    mov cl, byte ptr [r12 + rsi -1] ;befC
    mov rsi, rax
    sub rsi, rbx
    mov dl, byte ptr [r12 + rsi +1] ;nextC
    cmp cl, dil
    ja SET_0
    cmp dl, dil
    ja SET_0
    jmp KEEP_PIXEL

    TEST_90:
    ;Pobieramy s¹siadów pod k¹tem 90 stopni
    mov rsi, rax
    sub rsi, rbx
    mov cl, [r12+rsi]
    mov rsi, rax
    add rsi, rbx
    mov dl, [r12+rsi]
     cmp cl, dil
    ja SET_0
    cmp dl, dil
    ja SET_0
    jmp KEEP_PIXEL

    TEST_135:
    mov rsi,rax
    sub rsi, rbx
    mov cl, [r12+rsi-1]
    mov rsi, rax
    add rsi, rbx
    mov dl, [r12+rsi+1]
    cmp cl, dil
    ja SET_0
    cmp dl, dil
    ja SET_0
    jmp KEEP_PIXEL


    FIRST_X_PIXEL:
    mov rax, r10           ; Y
    imul rax, rbx           ; Y * width
    add rax, r15           ; index = Y * width + X
    mov byte ptr [r13 + rax], 0 ; output[index] = 0
    inc r15d                 ; X++
    cmp r15d, 1
    je X_LOOP
    jmp SKALAR_X_LOOP

    Y_EDGE:
     mov rax, r10           ; Y
    imul rax, rbx           ; Y * width
    add rax, r15           ; index = Y * width + X
    mov byte ptr [r13 + rax], 0 ; output[index] = 0
    inc r15d                 ; X++
    cmp r15d, ebx           ; X < width?
    jb  Y_EDGE
    jmp NEXT_Y
    PROCESS_0:
    ;Przetwarzanie pixeli brzegowych. 
    ;Wszystkie ustawiamy na 0, tak jak to robilismy w C++.
    mov rax, r10           ; Y
    imul rax, rbx           ; Y * width
    add rax, r15           ; index = Y * width + X
    mov byte ptr [r13 + rax], 0 ; output[index] = 0]
    add r15d, 32                 ; X += 32, bo przetwarzamy 32 pixele na raz
    jmp X_LOOP

    SET_0:
    mov rax, r10
    imul rax, rbx
    add rax, r15
    mov byte ptr [r13+rax], 0
    inc r15d
    jmp SKALAR_X_LOOP

    KEEP_PIXEL:
    mov r8, r10
    imul r8, rbx
    add r8, r15
    mov byte ptr [r13 + r8], dil ; output[index] = currA
    inc r15d                 
    jmp SKALAR_X_LOOP
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
    
NonMaximumSuspensionAsm endp
end