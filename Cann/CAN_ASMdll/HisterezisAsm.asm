.data
 align 16

 const_255 db 32 dup(255) 
.code
public HisterezisAsm

HisterezisAsm proc
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
     
            ; r9 = adres struktury size
    mov ecx, dword ptr [r8]     ; width
    mov r10d, dword ptr [r8+4]   ; height
    mov r8d, ecx
    ;currentY = r9
    mov r11d, dword ptr [rbp+48] ; endY

    LoopY:
    cmp r9d, r11d
    jae Done
    xor r15d, r15d
    LoopX:
    cmp r15d, r8d
    jae NextY
     cmp r9d, 0 
    je Set_0
    mov eax, r10d
    dec eax
    cmp r9d, eax
    je Set_0
    cmp r15d, 0
    je Set_0
    mov eax, r8d
    dec eax
    cmp r15d, eax
    je Set_0
    ;-----------------------------
    ; Sprawdzenie czy pixel jest silny, jeœli tak, zostawiamy go bez zmian,
    ; Je¿eli jest s³aby, czy któryœ z s¹siadów jest silny, 
    ; jeœli tak ustawiamu jego wartoœæ na 255
    ;-----------------------------
    mov eax, r9d
    imul rax, r8
    add eax, r15d
    mov r12b, byte ptr[r13+rax]

    cmp r12b, 255 ;Je¿eli pixel ma wartoœæ 255, zostawiamy go
    je Set_Strong

    cmp r12b, 100 ;Je¿eli pixel ma wartoœæ 100, sprawdzamy jego s¹siadów
    je Check_Neigh
    mov byte ptr[r14+rax],0
    jmp NextX       ;Je¿eli pixel ma wartoœæ 0, zostawiamy go

    Check_Neigh:
    vpxor xmm0, xmm0, xmm0   ;Pobieramy wszystkich s¹siadów pixela
    vpinsrb xmm0, xmm0, byte ptr[r13+rax-1],0   ;Pixel na lewo od wybranego
    vpinsrb xmm0, xmm0, byte ptr[r13+rax+1],1   ;Pixel na prawo od wybranego
    mov rsi, rax    ;dodajemy width, aby sprawdziæ wiersz poni¿ej
    add esi, r8d
    vpinsrb xmm0, xmm0, byte ptr[r13+rsi],2       ;Pixel na dole na œrodku
    vpinsrb xmm0, xmm0, byte ptr[r13+rsi-1],3     ;Pixel na dole na lewo
    vpinsrb xmm0, xmm0, byte ptr[r13+rsi+1],4     ;Pixel na dole na prawo
    mov rsi, rax
    sub esi, r8d    ;Odejmujemy width, aby sprawdziæ wiersz powy¿ej
    vpinsrb xmm0, xmm0, byte ptr[r13+rsi],5         ;Pixel na górze na œrodku
    vpinsrb xmm0, xmm0, byte ptr[r13+rsi-1],6      ;Pixel na górze na lewo
    vpinsrb xmm0, xmm0, byte ptr[r13+rsi+1],7       ;Pixel na górze na prawo
    vpbroadcastb xmm1, byte ptr [const_255]     ;Przygotowanie rejestru do porównañ
    vpcmpeqb xmm2, xmm0, xmm1                   ;Porównanie
    vpmovmskb edx, xmm2
    test edx, edx
    jnz Set_Strong
    mov byte ptr[r14+rax], 0                     ;Je¿eli nie znaleziono mocnego s¹siada ustawiamy na zero
    jmp NextX
    Set_Strong:
    mov byte ptr[r14+rax], 255
    jmp NextX

    Set_0:
    mov rax, r9
    imul rax, r8
    add eax, r15d
    mov byte ptr[r14+rax],0
    inc r15d
    jmp LoopX

    NextX:
    inc r15d
    jmp LoopX

    NextY:
    inc r9d
    jmp LoopY


Done:
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
HisterezisAsm endp
END