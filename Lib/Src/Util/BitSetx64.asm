; Fast routines for BitSet x64
;

.CODE

;bool bitSetIsEmpty(const void *p, size_t atomCount);
bitSetIsEmpty PROC
    pushf
    mov rdi, rcx
    mov rcx, rdx
    xor rax, rax
    cld
    repe scasq
    jnz NonZeroFound
    mov rax, 1
    jmp PopEnd
NonZeroFound:
    xor rax, rax
PopEnd:
    popf
    ret
bitSetIsEmpty ENDP


;intptr_t bitSetIteratorFirst(const BitSet::Atom *first, size_t start, size_t end);
; return lowest index of first 1-bit where start <= index <= end. -1 if none exist
; RBX = start
; R8  = end
; R9  = firstAtom
; R10 = p            = firstAtom + ATOMINDEX(start)
; R11 = endAtom      = firstAtom + ATOMINDEX(end  )

bitSetIteratorFirst PROC
    pushf
    push rbx

    mov r9 , rcx              ; r9 = firstAtom (set.m_p)
    mov r10, rcx
    mov r11, rcx
    mov rbx, rdx              ; rbx = start
    mov rax, rdx
    sar rax, 3
    and al , 0f8h
    add r10, rax              ; r10 = p = firstAtom + ATOMINDEX(start)
    mov rax, r8
    sar rax, 3
    and al , 0f8h
    add r11, rax              ; r11 = endAtom = firstAtom + ATOMINDEX(end)
    
    mov rdi, r10              ; rdi = p
    cld
SearchNonZeroAtom:
    mov rcx, r11              ; rcx = endAtom
    sub rcx, rdi
    add rcx, 8
    sar rcx, 3                ; rcx = endAtom - p + 1
    xor rax, rax
    repe scasq
    je NonFound
    sub rdi, 8
    cmp rdi, r11              ; cmp rdi, endAtom
    ja NonFound
    mov rdx, QWORD PTR[rdi]
    cmp rdi, r10              ; cmp rdi, p
    ja  ScanBitsForward

    mov rcx, rbx              ; mask rdx &= ~MASKATOM(start % BITSINATOM)
    and rcx, 63
    mov rax, 1
    shl rax, cl
    dec rax
    not rax
    and rdx, rax              ; rdx = highend bits of first nonzero QWORD
    jne ScanBitsForward
    add rdi, 8                ; search for next non-zero atom
    jmp SearchNonZeroAtom
ScanBitsForward:
    bsf rax, rdx
    sub rdi, r9               ; sub rdi, firstAtom
    shl rdi, 3
    add rdi, rax
    cmp rdi, r8               ; cmp rdi, end
    jbe Success
NonFound:
    mov rax, -1
    jmp PopEnd
Success:
    mov rax, rdi
PopEnd:
    pop rbx
    popf
    ret
bitSetIteratorFirst ENDP


;intptr_t bitSetIteratorNext(const BitSet::Atom *first, size_t next, size_t end);
; return lowest index of first 1-bit where start <= index <= end. -1 if none exist
; RBX = firstAtom
; R8  = end
; R9  = i            = ATOMINDEX(next) = next / BITSINATOM
; R10 = j            = next % BITSINATOM
; R11 = endAtom      = firstAtom + ATOMINDEX(end  )
bitSetIteratorNext PROC
    push rbx

    mov rbx, rcx                    ; rbx = firstAtom (set.m_p)
    mov r11, rcx
    mov rax, r8
    sar rax, 3
    and al , 0f8h
    add r11, rax                    ; r11 = endAtom = firstAtom + ATOMINDEX(end)
    mov r9 , rdx
    sar r9 , 6                      ; r9  = i = next / BITSINATOM
    and dl , 63
    mov r10, rdx                    ; r10 = j = next % BITSINATOM

    mov rcx, r10                    ; rcx = j
    or  rcx, rcx
    je  InitRDIAndSearchNonZeroAtom ; if(j==0) goto InitRDIAndSearchNonZeroAtom
    mov rdi, rbx
    mov rax, r9
    shl rax, 3
    add rdi, rax
    mov rdx, qword ptr[rdi]
    mov rax, 1
    shl rax, cl
    dec rax
    not rax
    and rdx, rax
    jz SearchNextNonZeroAtom

    bsf rax, rdx
    sub rdi, rbx
    shl rdi, 3
    add rdi, rax                    ; rdi = next
    cmp rdi, r8                     ; cmp rdi, end
    ja  NoNextNoPop
    mov rax, rdi
    jmp Epilog
NoNextNoPop:
    mov rax, -1
    jmp Epilog
NoNextPop:
    mov rax, -1
    jmp PopEnd
InitRDIAndSearchNonZeroAtom:
    mov rdi, rbx
    mov rax, r9
    shl rax, 3
    add rdi, rax
    jmp SearchNonZeroAtom
SearchNextNonZeroAtom:
    add rdi, 8
SearchNonZeroAtom:
    pushf
    mov rcx, r11                    ; mov rcx, endAtom
    sub rcx, rdi
    add rcx, 8
    sar rcx, 3
    xor rax, rax
    cld
    repe scasq
    jz NoNextPop

    sub rdi, 8
    mov rdx, qword ptr[rdi]
    bsf rax, rdx
    sub rdi, rbx                    ; sub rdi, firstAtom
    shl rdi, 3
    add rdi, rax
    cmp rdi, r8                     ; cmp rdi, end
    ja  NoNextPop
    mov rax, rdi
PopEnd:
    popf
Epilog:
    pop rbx
    ret
bitSetIteratorNext ENDP


;intptr_t bitSetReverseIteratorFirst(const BitSet::Atom *first, size_t start, size_t end);
; return higest index of 1-bit where start >= index >= end. -1 if none exist
; RBX = start
; R8  = end
; R9  = firstAtom
; R10 = p            = firstAtom + ATOMINDEX(start)
; R11 = endAtom      = firstAtom + ATOMINDEX(end  )
bitSetReverseIteratorFirst PROC
    pushf
    push rbx

    mov r9 , rcx              ; r9 = firstAtom (set.m_p)
    mov r10, rcx
    mov r11, rcx
    mov rbx, rdx              ; rbx = start
    mov rax, rdx
    sar rax, 3
    and al , 0f8h
    add r10, rax              ; r10 = p = firstAtom + ATOMINDEX(start)
    mov rax, r8
    sar rax, 3
    and al , 0f8h
    add r11, rax              ; r11 = endAtom = firstAtom + ATOMINDEX(end)

    mov rdi, r10              ; rdi = p
    std
SearchRNonZeroAtom:
    mov rcx, rdi
    sub rcx, r11
    add rcx, 8
    sar rcx, 3                ; rcx = rdi - endAtom + 1
    xor rax, rax
    repe scasq
    je NonFound
    add rdi, 8
    cmp rdi, r11              ; cmp rdi, endAtom
    jb NonFound
    mov rdx, QWORD PTR [rdi]
    cmp rdi, r10              ; cmp rdi, p
    jb ScanBitsBackward
                    
    mov rcx, rbx              ; mask edx &= MASKATOM(start % BITSINATOM + 1)
    and rcx, 63
    cmp rcx, 63
    je  ScanBitsBackward      ; if start%64+1==64 no masking, all bits should be searched
    inc rcx
    mov rax, 1
    shl rax, cl
    dec rax
    and rdx, rax              ; rdx = lowend bits of first nonzero QWORD
    jne ScanBitsBackward
    sub rdi, 8                ; search for next non-zero atom
    jmp SearchRNonZeroAtom
ScanBitsBackward:
    bsr rax, rdx
    sub rdi, r9               ; sub rdi, firstAtom
    shl rdi, 3
    add rdi, rax
    cmp rdi, r8               ; cmp rdi, end
    jae Success
NonFound:
    mov rax, -1
    jmp PopEnd
Success:
    mov rax, rdi
PopEnd:
    pop rbx
    popf
    ret
bitSetReverseIteratorFirst ENDP


;intptr_t bitSetReverseIteratorNext(const BitSet::Atom *first, size_t next, size_t end);
; return higest index of 1-bit where start >= index >= end. -1 if none exist
; RBX = firstAtom
; R8  = end
; R9  = i            = ATOMINDEX(next) = next / BITSINATOM
; R10 = j            = next % BITSINATOM + 1
; R11 = endAtom      = firstAtom + ATOMINDEX(end)
bitSetReverseIteratorNext PROC
    push rbx

    mov rbx, rcx                    ; rbx = firstAtom (set.m_p)
    mov r11, rcx
    mov rax, r8
    sar rax, 3
    and al , 0f8h
    add r11, rax                    ; r11 = endAtom = firstAtom + ATOMINDEX(end)
    mov r9 , rdx
    sar r9 , 6                      ; r9  = i = next / BITSINATOM
    and dl , 63
    mov r10, rdx                    
    inc r10                         ; r10 = j = next % BITSINATOM + 1

    mov rcx, r10
    cmp rcx, 64
    je InitRDIAndSearchRNonZeroAtom
    mov rdi, rbx
    mov rax, r9
    shl rax, 3
    add rdi, rax
    mov rdx, qword ptr[rdi]
    mov rax, 1
    shl rax, cl
    dec rax
    and rdx, rax
    jz SearchRNextNonZeroAtom

    bsr rax, rdx
    sub rdi, rbx                      ; sub rdi, firstAtom
    shl rdi, 3
    add rdi, rax
    cmp rdi, r8                       ; cmp rdi, end
    jb NoNextNoPop
    mov rax, rdi
    jmp Epilog
NoNextNoPop:
    mov rax, -1
    jmp Epilog
NoNextPop:
    mov rax, -1
    jmp PopEnd
InitRDIAndSearchRNonZeroAtom:
    mov rdi, rbx                      ; mov rdi, firstAtom
    mov rax, r9                       ; mov rax, i
    shl rax, 3
    add rdi, rax
    jmp SearchRNonZeroAtom
SearchRNextNonZeroAtom:
    sub rdi, 8
SearchRNonZeroAtom:
    pushf
    mov rcx, rdi
    sub rcx, r11
    add rcx, 8
    sar rcx, 3                          ; rcx = rdi - endAtom + 1
    xor rax, rax
    std
    repe scasq
    jz NoNextPop

    add rdi, 8
    mov rdx, qword ptr[rdi]
    bsr rax, rdx
    sub rdi, rbx                        ; sub rdi, firstAtom
    shl rdi, 3
    add rdi, rax
    cmp rdi, r8                         ; cmp rdi, end
    jb NoNextPop
    mov rax, rdi
PopEnd:
    popf
Epilog:
    pop rbx
    ret
bitSetReverseIteratorNext ENDP

END
