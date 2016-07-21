.CODE

;void int128add(_int128 &dst, const _int128 &x); do assignop dst += x;
int128add PROC
    mov         rax, qword ptr[rdx]
    add         qword ptr[rcx], rax
    mov         rax, qword ptr[rdx+8]
    adc         qword ptr[rcx+8], rax
    ret
int128add ENDP

;void int128sub(_int128 &dst, const _int128 &x); do assignop dst -= x;
int128sub PROC
    mov         rax, qword ptr[rdx]
    sub         qword ptr[rcx], rax
    mov         rax, qword ptr[rdx+8]
    sbb         qword ptr[rcx+8], rax
    ret
int128sub ENDP

;void int128mul(_int128 &dst, const _int128 &x); do assignop dst *= x;
int128mul PROC
    push        rbx
    mov         rax, qword ptr[rdx+8]           ; rax = x.hi
    mov         rbx, qword ptr[rcx+8]           ; rbx = dst.hi
    or          rbx, rax                        ; rbx = x.hi | y.hi
    mov         rbx, qword ptr[rcx]             ; rbx = dst.lo
    jne         Hard                            ; if(x.hi|dst.hi) goto Hard
                                                ; else simple int64 multiplication
    mov         rax, qword ptr[rdx]             ; rax = x.lo
    mul         rbx                             ; rdx:rax = rax * rbx
    mov         qword ptr[rcx]  , rax           ; dst.lo = rax
    mov         qword ptr[rcx+8], rdx           ; dst.hi = rdx
    pop         rbx
    ret
Hard:                                           ; assume rax = x.hi, rbx = dst.lo
    push        rsi
    mov         rsi, rdx                        ; need rdx for highend of mul, so rsi = &x
    mul         rbx                             ; rdx:rax = x.hi * y.lo
    mov         r9 , rax                        ; 
    mov         rax, qword ptr[rsi]             ; rax     = x.lo
    mul         qword ptr[rcx+8]                ; rdx:rax = x.lo * dst.hi
    add         r9, rax                         ; r9      = lo(x.hi*dst.lo+x.lo*dst.hi); 
    mov         rax, qword ptr[rsi]             ; rax     = x.lo
    mul         rbx                             ; rdx:rax = x.lo * y.lo
    add         rdx, r9
    mov         qword ptr[rcx]  , rax
    mov         qword ptr[rcx+8], rdx
    pop         rsi
    pop         rbx
    ret
int128mul ENDP

;void int128div(_int128 &dst, _int128 &x); do assignop dst /= x; if (x < 0) x = -x; !!
int128div PROC
    push        rbx                             ;
    push        rdi                             ;
    push        rsi                             ;
    mov         r8, rcx                         ; r8 = &dst
    mov         r9, rdx                         ; r9 = &x
    xor         rdi, rdi                        ; rdi = 0
    mov         rax, qword ptr[r8+8]            ; check sign of dst
    or          rax, rax                        ;
    jge         L1                              ; if(dst >= 0) goto L1
    inc         rdi                             ; rdi++
    mov         rdx, qword ptr[r8]              ; dst is negative. Change sign
    neg         rax                             ; 
    neg         rdx                             ; 
    sbb         rax, 0                          ; 
    mov         qword ptr[r8+8], rax            ; dst = -original dst
    mov         qword ptr[r8], rdx              ;
L1:                                             ; Assume dst >= 0, rdi = (original dst < 0)1:0
    mov         rax, qword ptr[r9+8]            ; check sign of x
    or          rax, rax                        ;
    jge         L2                              ; 
    inc         rdi                             ; x is negative. change sign, and increment rdi
    mov         rdx, qword ptr[r9]              ;
    neg         rax                             ;
    neg         rdx                             ;
    sbb         rax, 0                          ;
    mov         qword ptr[r9+8], rax            ; x = -original x
    mov         qword ptr[r9], rdx              ;
L2:                                             ; Assume dst >= 0, x > 0, rdi = #original negative arguments
    or          rax, rax                        ;
    jne         L3                              ;
    mov         rcx, qword ptr[r9]              ;
    mov         rax, qword ptr[r8+8]            ;
    xor         rdx, rdx                        ;
    div         rcx                             ;
    mov         rbx, rax                        ;
    mov         rax, qword ptr[r8]              ;
    div         rcx                             ;
    mov         rdx, rbx                        ;
    jmp         L4                              ;
L3:                                             ;
    mov         rbx, rax                        ;
    mov         rcx, qword ptr[r9]              ;
    mov         rdx, qword ptr[r8+8]            ;
    mov         rax, qword ptr[r8]              ;
L5:                                             ;
    shr         rbx, 1                          ;
    rcr         rcx, 1                          ;
    shr         rdx, 1                          ;
    rcr         rax, 1                          ;
    or          rbx, rbx                        ;
    jne         L5                              ;
    div         rcx                             ;
    mov         rsi, rax                        ;
    mul         qword ptr[r9+8]                 ;
    mov         rcx, rax                        ;
    mov         rax, qword ptr[r9]              ;
    mul         rsi                             ;
    add         rdx, rcx                        ;
    jb          L6                              ;
    cmp         rdx, qword ptr[r8+8]            ;
    ja          L6                              ;
    jb          L7                              ;
    cmp         rax, qword ptr[r8]              ;
    jbe         L7                              ;
L6:                                             ;
    dec         rsi                             ;
L7:                                             ;
    xor         rdx, rdx                        ;
    mov         rax, rsi                        ;
L4:                                             ;
    dec         rdi                             ;
    jne         L8                              ;
    neg         rdx                             ;
    neg         rax                             ;
    sbb         rdx, 0                          ;
L8:                                             ;
    pop         rsi                             ;
    pop         rdi                             ;
    pop         rbx                             ;
    mov         qword ptr[r8], rax              ;
    mov         qword ptr[r8+8], rdx            ;
    ret                                         ;
int128div ENDP

;void int128rem(_int128 &dst, _int128 &x); do assignop dst %= x; if (x < 0) x = -x; !!
int128rem PROC
    push        rbx                             ;
    push        rdi                             ;
    mov         r8, rcx                         ; r8 = &dst
    mov         r9, rdx                         ; r9 = &x
    xor         rdi, rdi                        ;
    mov         rax, qword ptr[r8+8]            ; check sign of dst
    or          rax, rax                        ;
    jge         L1                              ;
    inc         rdi                             ;
    mov         rdx, qword ptr[r8]              ; dst is negative. change sign
    neg         rax                             ;
    neg         rdx                             ;
    sbb         rax, 0                          ;
    mov         qword ptr[r8+8], rax            ; dst = -original dst
    mov         qword ptr[r8], rdx              ;
L1:                                             ; Assume dst>=0, rdi = (original dst < 0)1:0
    mov         rax, qword ptr[r9+8]            ; check sign of x
    or          rax, rax                        ;
    jge         L2                              ; if(x >= 0) goto L2
    mov         rdx, qword ptr[r9]              ; x is negative. Change sign
    neg         rax                             ;
    neg         rdx                             ;
    sbb         rax, 0                          ;
    mov         qword ptr[r9+8], rax            ;
    mov         qword ptr[r9], rdx              ;
L2:                                             ; Assume dst>=0 and x > 0, (original dst < 0)1:0. dont care about orignal sign of x
    or          rax, rax                        ;
    jne         L3                              ;
    mov         rcx, qword ptr[r9]              ;
    mov         rax, qword ptr[r8+8]            ;
    xor         rdx, rdx                        ;
    div         rcx                             ;
    mov         rax, qword ptr[r8]              ;
    div         rcx                             ;
    mov         rax, rdx                        ;
    xor         rdx, rdx                        ;
    dec         rdi                             ;
    jns         L4                              ;
    jmp         L8                              ;
L3:                                             ;
    mov         rbx, rax                        ;
    mov         rcx, qword ptr[r9]              ;
    mov         rdx, qword ptr[r8+8]            ;
    mov         rax, qword ptr[r8]              ;
L5:                                             ;
    shr         rbx, 1                          ;
    rcr         rcx, 1                          ;
    shr         rdx, 1                          ;
    rcr         rax, 1                          ;
    or          rbx, rbx                        ;
    jne         L5                              ;
    div         rcx                             ;
    mov         rcx, rax                        ;
    mul         qword ptr[r9+8]                 ;
    xchg        rax, rcx                        ;
    mul         qword ptr[r9]                   ;
    add         rdx, rcx                        ;
    jb          L6                              ;
    cmp         rdx, qword ptr[r8+8]            ;
    ja          L6                              ;
    jb          L7                              ;
    cmp         rax, qword ptr[r8]              ;
    jbe         L7                              ;
L6:                                             ;
    sub         rax, qword ptr[r9]              ;
    sbb         rdx, qword ptr[r9+8]            ;
L7:                                             ;
    sub         rax, qword ptr[r8]              ;
    sbb         rdx, qword ptr[r8+8]            ;
    dec         rdi                             ;
    jns         L8                              ;
L4:                                             ;
    neg         rdx                             ;
    neg         rax                             ;
    sbb         rdx, 0                          ;
L8:                                             ;
    pop         rdi                             ;
    pop         rbx                             ;
    mov         qword ptr[r8], rax              ;
    mov         qword ptr[r8+8], rdx            ;
    ret                                         ;
int128rem ENDP

;void int128neg(_int128 &x); set x = -x;
int128neg PROC
mov         rax, qword ptr[rcx]
neg         rax
mov         qword ptr[rcx], rax
mov         rax, qword ptr[rcx + 8]
adc         rax, 0
neg         rax
mov         qword ptr[rcx + 8], rax
ret
int128neg ENDP

;void int128inc(_int128 &x); set x = x + 1;
int128inc PROC
    add         qword ptr[rcx], 1
    adc         qword ptr[rcx+8], 0
    ret
int128inc ENDP

;void int128dec(_int128 &x); set xz = x - 1;
int128dec PROC
    sub         qword ptr[rcx], 1
    sbb         qword ptr[rcx+8], 0
    ret
int128dec ENDP

;void int128shr(_int128 &x, int shft); do assignop x >>= shft; (if(x<0) shift 1-bits in from left, else 0-bits)
int128shr PROC
    mov         rax, rcx                    ; rax = &x; need cl to the shift instruction
    mov         rcx, rdx                    ; rcx = shift amount
    mov         rdx, qword ptr[rax+8]       ; rdx = x.hi 
    cmp         cl, 80h
    jae         RetSign
    cmp         cl, 40h
    jae         More64
    shrd        qword ptr[rax], rdx, cl     ; shift x.lo taking new bits from x.hi (rdx)
    sar         qword ptr[rax+8], cl        ; shift x.hi
    ret
More64:
    mov         qword ptr[rax], rdx
    sar         qword ptr[rax+8], 3Fh
    and         cl, 3Fh
    sar         qword ptr[rax], cl
    ret
RetSign:
    sar         rdx,3Fh
    mov         qword ptr[rax], rdx
    mov         qword ptr[rax+8], rdx
    ret
int128shr ENDP

;void int128shl(_int128 &x, int shft); do assignop x <<= shft;
int128shl PROC
    mov         rax, rcx                    ; rax = &x; need cl to the shift instruction
    mov         rcx, rdx                    ; rcx = shift amount
    mov         rdx, qword ptr[rax]         ; rdx = x.lo
    cmp         cl, 80h
    jae         RetZero
    cmp         cl, 40h
    jae         More64
    shld        qword ptr[rax+8], rdx, cl   ; shift x.hi taking new bits from x.lo (rdx)
    shl         qword ptr[rax], cl          ; shift x.lo
    ret
More64:
    and         cl, 3Fh                     ; cl %= 64
    shl         rdx, cl                     ; shift rdx (x.lo)
    mov         qword ptr[rax+8], rdx       ; x.hi = rdx
    xor         rdx, rdx
    mov         qword ptr[rax], rdx         ; x.lo = 0
    ret
RetZero:
    xor         rdx, rdx                    ; return 0
    mov         qword ptr[rax], rdx
    mov         qword ptr[rax+8], rdx
    ret
int128shl ENDP

;int int128cmp(const _int128 &x1, const _int128 &x2); return sign(x1 - x2);
int128cmp PROC
    mov         rax, qword ptr[rcx+8]       ; x1.hi
    cmp         rax, qword ptr[rdx+8]       ; x2.hi
    jl          lessthan                    ; signed compare of x1.hi and x2.hi
    jg          greaterthan
    mov         rax, qword ptr[rcx]         ; x2.lo
    cmp         rax, qword ptr[rdx]         ; x2.lo
    jb          lessthan                    ; unsigned compare of x1.lo and x2.lo
    ja          greaterthan
    mov         rax, 0                      ; they are equal
    ret
greaterthan:
    mov         rax, 1
    ret
lessthan:
    mov         rax, -1
    ret
int128cmp ENDP

END
