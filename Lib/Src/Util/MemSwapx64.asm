; Assembler routine for swapping arbitrary sized buffers
;
.CODE

;void memSwap(void *p1, void *p2, size_t w);
; p1 in rcx, p2 in rdx, w in r8, use rax, r9 as extra registers
memSwap PROC
    cmp r8, 8
    jb SwapDword 
LoopQword:
    mov	rax, QWORD PTR [rcx]
    mov	r9 , QWORD PTR [rdx]
    mov	QWORD PTR [rdx], rax
    mov	QWORD PTR [rcx], r9
    sub r8, 8
    cmp r8, 8
    jb EndLoopQword
    add	rcx, 8
    add	rdx, 8
    jmp LoopQword
EndLoopQword:
    test r8,r8
    jz Epilog
    add	rcx, 8
    add	rdx, 8
SwapDword:
    mov	rax, QWORD PTR [rcx]
    mov	r9 , QWORD PTR [rdx]
    cmp r8, 4
    jb SwapWord
    mov DWORD PTR [rdx], eax
    mov DWORD PTR [rcx], r9d
    sub r8, 4
    jz Epilog
    add rcx, 4
    add rdx, 4
    shr rax, 32
    shr r9 , 32
SwapWord:
    cmp r8, 2
    jb SwapByte
    mov	WORD PTR [rdx], ax
    mov	WORD PTR [rcx], r9w
    sub r8, 2
    jz Epilog
    add	rcx, 2
    add	rdx, 2
    shr eax, 16
    shr r9 , 16
SwapByte:
    mov	BYTE PTR [rdx], al
    mov	BYTE PTR [rcx], r9b
Epilog:
    ret
memSwap ENDP

END
