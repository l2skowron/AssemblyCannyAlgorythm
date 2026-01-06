.data
    align 16

    const_255 db 32 dup(255) 
    const_100  db 32 dup(100)
.code
public DoubleThresholdingAsm

DoubleThresholdingAsm proc
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

    mov r13, rcx             ; input
    mov r14, rdx             ; output
    mov ecx, r8d             ;ecx = strong pixel
    mov edx, r9d             ;edx = weak pixel
     
    mov r8, [rbp+48]          ; r8 = adres struktury size
    mov r9d, dword ptr [r8]     ; width
    mov r10d, dword ptr [r8+4]   ; height
    mov r11d, dword ptr [rbp+56] ; currentY (startY)
    mov r12d, dword ptr [rbp+64] ; endY

    ;Przygotowanie progów strong i weak do obliczeñ
    
    vmovd xmm4, ecx         ; przeniesienie do rejestru xmm4
    vpbroadcastb ymm4, xmm4 ; przygotowanie maski 32 bajty

   
    vmovd xmm5, edx         ; przeniesienie do rejestru xmm5
    vpbroadcastb ymm5, xmm5 ; przygotowanie maski 32 bajty

    vpbroadcastb ymm8,byte ptr[const_255] ;Ustawienie wartoœci 255 w rejestrze ymm8
    vpbroadcastb ymm9, byte ptr[const_100];Ustaiwenie wartoœci 100 w rejestrze ymm9 
    
    LoopY:
    cmp r11d, r12d          ;Sprawdzenie czy currentY = endY
    jge Done                ;Je¿eli tak, skaczymy do etykiety done
    xor r15d, r15d          ; currentX = 0
    LoopX:              
    cmp r15d, r9d           ; Sprawdzenie czy currentX = width
    jge NextY               ; Jeœli tak skaczemy do nastêpnego wiersza
    mov eax, r9d            
    sub eax, 32             ; Test dla koñca wiersza, je¿eli zosta³o mniej niz 32 pixele
    cmp r15d, eax           ; Nastêpne przetwarzamy skalarnie, jeœli nie, wektorowo
    jae ScalarX   
    ;---------------------------------------
    ;Obliczenia wektorowe
    ;---------------------------------------
    ; obliczanie indexu pixeli
    mov eax, r11d           ; eax = y
    imul eax, r9d           ; eax = y*size.x
    add eax, r15d           ;eax = y*size.x+x
    mov rdi, rax            ; rdi = index pixeli
    ; ³adowanie pixeli
    vmovdqu ymm0, ymmword ptr[r13+rdi] ; Pobranie 32 pixeli do rejestru ymm0
    ; Porównanie i ustawienie masek bitowych dla progów 
    vpmaxub ymm1, ymm0, ymm4   ;Ustawienie maski dla strong pixel
    vpcmpeqb ymm1, ymm1, ymm0   ; Je¿eli max(pixel, strongPixel) == pixel to p>=próg
    vpmaxub ymm2,  ymm0, ymm5   ;Ustawienie maski dla weak pixel
    vpcmpeqb ymm2, ymm2, ymm0   ;Je¿eli max(pixel, weakPixel)== pixel to p>próg


    vpand ymm7, ymm2, ymm9      ;ymm7= ustawia 100 tam gdzie maska 1 i 0 w pozosta³e miejsca
    
    vpand ymm6, ymm1, ymm8      ;ymm6 = ustawia 255 tam gdzie mask strong

    vpor ymm10, ymm7, ymm6      ; wynik koñcowy(0,100 lub 255)

    vmovdqu ymmword ptr [r14+rdi], ymm10 ; zapis wyniku

    add r15d, 32        ; dodanie 32 do licznika pêtli LoopX
    jmp LoopX           ;skok do pêtli LoopX

    NextY:
    inc r11d            ;Inkrementacja licznika pêtli LoopY
    jmp LoopY           ;Skok do pêtli LoopY
    ;-------------------------------------
    ;Obliczenia skalarne
    ;-------------------------------------
    ScalarX:
    cmp r15d, r9d   ;sprawdzamy czy CurrentX = width
    jae NextY       ;Jeœli tak przechodzimy do kolejnego wiersza
    ; obliczanie indexu pixeli
    mov eax, r11d   ;eax = y
    imul eax, r9d   ;eax = y*size.x
    add eax, r15d   ;eax = y*size.x+x
    mov rdi, rax             ; rdi = index pixeli
    movzx esi, byte ptr[r13+rdi] ; pobranie wartoœci pixela
    cmp esi, ecx    ;porównanie z wartoœci¹ StrongPixel
    jae Set_Strong  ;jeœli jest wiêksza lub równa skok do Set_Strong
    cmp esi, edx    ;porównanie z wartoœci¹ WeakPixel
    jae Set_Weak    ;jeœli jest wiêksza lub równa skok do Set_Weak
    mov byte ptr[r14+rdi],0 ;jeœli pixel jest za ma³y to 0
    inc r15d        ;inkrementacja pêtli LoopX
    jmp ScalarX     ;skok do pêtli skalarnej
    Set_Strong:
    mov byte ptr[r14+rdi],255   ;zapisanie mocnego pixela
    inc r15d                    ;inkrementacja pêtli LoopX
    jmp ScalarX                 ;Skok do pêtli skalarnej
    Set_Weak:
    mov byte ptr[r14+rdi],100   ;zapisanie s³abego pixela
    inc r15d                    ;inkrementacja pêtli LoopX
    jmp ScalarX                 ;skok do pêtli skalarnej
    



    Done:
    add rsp, 96
    pop r15
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbx
    mov rsp, rbp
    pop rbp
    ret
DoubleThresholdingAsm endp
END