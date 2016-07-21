.CODE

;void uint128div(_uint128 &dst, const _uint128 &x); do assignop dst /= x;
uint128div PROC
     push       rbx                             ; same as signed division
     push       rsi                             ; but without sign check on arguments
     mov        r8, rcx                         ; r8 = &dst
     mov        r9, rdx                         ; r9 = &x
     mov        rax, qword ptr[r9+8]            ;
     or         rax, rax                        ;
     jne        L1                              ;
     mov        rcx, qword ptr[r9]
     mov        rax, qword ptr[r8+8]
     xor        rdx, rdx
     div        rcx
     mov        rbx, rax
     mov        rax, qword ptr[r8]
     div        rcx
     mov        rdx, rbx
     jmp        L2
L1:
     mov        rcx, rax
     mov        rbx, qword ptr[r9]
     mov        rdx, qword ptr[r8+8]
     mov        rax, qword ptr[r8]
L3:
     shr        rcx, 1
     rcr        rbx, 1
     shr        rdx, 1
     rcr        rax, 1
     or         rcx, rcx
     jne        L3
     div        rbx
     mov        rsi, rax
     mul        qword ptr[r9+8]
     mov        rcx, rax
     mov        rax, qword ptr[r9]
     mul        rsi
     add        rdx, rcx
     jb         L4
     cmp        rdx, qword ptr[r8+8]
     ja         L4
     jb         L5
     cmp        rax, qword ptr[r8]
     jbe        L5
L4:
     dec        rsi
L5:
     xor        rdx, rdx
     mov        rax, rsi
L2:
     pop        rsi
     pop        rbx
     mov        qword ptr[r8], rax
     mov        qword ptr[r8+8], rdx
     ret
uint128div ENDP

;void uint128rem(_uint128 &dst, const _uint128 &x); do assignop dst %= x;
uint128rem PROC
     push       rbx
     mov        r8, rcx
     mov        r9, rdx
     mov        rax, qword ptr[r9+8]
     or         rax, rax
     jne        L1
     mov        rcx, qword ptr[r9]
     mov        rax, qword ptr[r8+8]
     xor        rdx, rdx
     div        rcx
     mov        rax, qword ptr[r8]
     div        rcx
     mov        rax, rdx
     xor        rdx, rdx
     jmp        L2
L1:
     mov        rcx, rax
     mov        rbx, qword ptr[r9]
     mov        rdx, qword ptr[r8+8]
     mov        rax, qword ptr[r8]
L3:
     shr        rcx, 1
     rcr        rbx, 1
     shr        rdx, 1
     rcr        rax, 1
     or         rcx, rcx
     jne        L3
     div        rbx
     mov        rcx, rax
     mul        qword ptr[r9+8]
     xchg       rax, rcx
     mul        qword ptr[r9]
     add        rdx, rcx
     jb         L4
     cmp        rdx, qword ptr[r8+8]
     ja         L4
     jb         L5
     cmp        rax, qword ptr[r8]
     jbe        L5
L4:
     sub        rax, qword ptr[r9]
     sbb        rdx, qword ptr[r9+8]
L5:
     sub        rax, qword ptr[r8]
     sbb        rdx, qword ptr[r8+8]
     neg        rdx
     neg        rax
     sbb        rdx, 0
L2:
     pop        rbx
     mov        qword ptr[r8], rax
     mov        qword ptr[r8+8], rdx
     ret
uint128rem ENDP

;void uint128shr(void *x, int shft); do assignop x >>= shft. always shift 0-bits in from left
uint128shr PROC
    mov         rax, rcx                    ; rax = &x; need cl to the shift instruction
    mov         rcx, rdx                    ; rcx = shift amount
    mov         rdx, qword ptr[rax+8]       ; rdx = x.hi 
    cmp         cl, 80h
    jae         RetZero
    cmp         cl, 40h
    jae         More64
    shrd        qword ptr[eax], rdx, cl     ; shift x.lo taking new bits from x.hi (rdx)
    shr         qword ptr[rax+8], cl        ; shift x.hi
    ret
More64:
    and         cl, 3Fh                     ; cl %= 64
    shr         rdx, cl                     ; rdx >>= cl   (x.hi)
    mov         qword ptr[rax], rdx         ; x.lo = rdx
    xor         rdx, rdx
    mov         qword ptr[rax+8], rdx       ; x.hi = 0
    ret
RetZero:
    xor         rdx, rdx                    ; return 0
    mov         qword ptr[rax], rdx
    mov         qword ptr[rax+8], rdx
    ret
uint128shr ENDP

;int uint128cmp(const _uint128 &x1, const _uint128 &x2); return sign(x1 - x2);
uint128cmp PROC
     mov        rax, qword ptr[rcx+8]      ; x1.hi
     cmp        rax, qword ptr[rdx+8]      ; x2.hi
     jb         lessthan                   ; unsigned compare of x1.hi and x2.hi
     ja         greaterthan
     mov        rax, qword ptr[rcx]        ; x1.lo
     cmp        rax, qword ptr[rdx]        ; x2.lo
     jb         lessthan                   ; unsigned compare of x1.lo and x2.lo
     ja         greaterthan
     mov        rax, 0                     ; they are equal
     ret
greaterthan:
     mov        rax, 1
     ret
lessthan:
     mov        rax, -1
     ret
uint128cmp ENDP

END
