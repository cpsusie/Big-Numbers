; These functions implements the basic operations for _int128 type
; running on 64-bit intel CPU.
; They are (almost) identical to MS's workhorse-functions that do
; __int64-math using 32-bit registers. Every memory reference that uses
; dword ptr(4 byte) are replaced by qword ptr(8 byte), and registernames
; eax, ebx, ecx... are replaced by rax, rbx, rcx etc.
; Parameters are passed in rcx(1st argument) and rdx(2nd argument) and
; return value is in rax, where the runtime-stack is used in x86 mode.
; So the code is modified to reflect this environment
;

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
    mov         rax, qword ptr[rdx+8]          ; rax  = x.hi
    mov         rbx, qword ptr[rcx+8]          ; rbx  = dst.hi
    or          rbx, rax                       ; rbx |= x.hi
    mov         rbx, qword ptr[rcx]            ; rbx  = dst.lo
    jne         Hard                           ; if(x.hi|dst.hi) goto Hard
                                               ; else simple int64 multiplication
    mov         rax, qword ptr[rdx]            ; rax  = x.lo
    mul         rbx                            ; rdx:rax = rax * rbx
    mov         qword ptr[rcx]  , rax          ; dst.lo = rax
    mov         qword ptr[rcx+8], rdx          ; dst.hi = rdx
    pop         rbx
    ret
Hard:                                          ; assume rax = x.hi, rbx = dst.lo
    push        rsi
    mov         rsi, rdx                       ; need rdx for highend of mul, so rsi = &x
    mul         rbx                            ; rdx:rax = x.hi*dst.lo
    mov         r9 , rax                       ; r9      = lo(x.hi*dst.lo)
    mov         rax, qword ptr[rsi]            ; rax     = x.lo
    mul         qword ptr[rcx+8]               ; rdx:rax = x.lo*dst.hi
    add         r9, rax                        ; r9     += lo(x.lo*dst.hi);
    mov         rax, qword ptr[rsi]            ; rax     = x.lo
    mul         rbx                            ; rdx:rax = x.lo*dst.lo
    add         rdx, r9                        ; rdx    += lo(x.hi*dst.lo) + lo(x.lo*dst.hi)
    mov         qword ptr[rcx]  , rax
    mov         qword ptr[rcx+8], rdx
    pop         rsi
    pop         rbx
    ret
int128mul ENDP

;void int128div(_int128 &dst, _int128 &x); do assignop dst /= x; if (x < 0) x = -x; !!
int128div PROC
    push        rbx                            ;
    push        rdi                            ;
    push        rsi                            ;
    mov         r8, rcx                        ; r8 = &dst
    mov         r9, rdx                        ; r9 = &x
    xor         rdi, rdi                       ; rdi = 0
    mov         rax, qword ptr[r8+8]           ; check sign of dst
    or          rax, rax                       ;
    jge         L1                             ; if(dst >= 0) goto L1
    inc         rdi                            ; rdi++
    mov         rdx, qword ptr[r8]             ; dst is negative. Change sign
    neg         rax                            ;
    neg         rdx                            ;
    sbb         rax, 0                         ;
    mov         qword ptr[r8+8], rax           ; dst = -original dst
    mov         qword ptr[r8], rdx             ;
L1:                                            ; Assume dst >= 0, rdi = (original dst < 0)1:0
    mov         rax, qword ptr[r9+8]           ; check sign of x
    or          rax, rax                       ;
    jge         L2                             ;
    inc         rdi                            ; x is negative. change sign, and increment rdi
    mov         rdx, qword ptr[r9]             ;
    neg         rax                            ;
    neg         rdx                            ;
    sbb         rax, 0                         ;
    mov         qword ptr[r9+8], rax           ; x = -original x
    mov         qword ptr[r9], rdx             ;
L2:                                            ; Assume dst >= 0, x > 0, rdi = #original negative arguments
    or          rax, rax                       ;
    jne         L3                             ;
    mov         rcx, qword ptr[r9]             ;
    mov         rax, qword ptr[r8+8]           ;
    xor         rdx, rdx                       ;
    div         rcx                            ;
    mov         rbx, rax                       ;
    mov         rax, qword ptr[r8]             ;
    div         rcx                            ;
    mov         rdx, rbx                       ;
    jmp         L4                             ;
L3:                                            ;
    mov         rbx, rax                       ;
    mov         rcx, qword ptr[r9]             ;
    mov         rdx, qword ptr[r8+8]           ;
    mov         rax, qword ptr[r8]             ;
L5:                                            ;
    shr         rbx, 1                         ;
    rcr         rcx, 1                         ;
    shr         rdx, 1                         ;
    rcr         rax, 1                         ;
    or          rbx, rbx                       ;
    jne         L5                             ;
    div         rcx                            ;
    mov         rsi, rax                       ;
    mul         qword ptr[r9+8]                ;
    mov         rcx, rax                       ;
    mov         rax, qword ptr[r9]             ;
    mul         rsi                            ;
    add         rdx, rcx                       ;
    jb          L6                             ;
    cmp         rdx, qword ptr[r8+8]           ;
    ja          L6                             ;
    jb          L7                             ;
    cmp         rax, qword ptr[r8]             ;
    jbe         L7                             ;
L6:                                            ;
    dec         rsi                            ;
L7:                                            ;
    xor         rdx, rdx                       ;
    mov         rax, rsi                       ;
L4:                                            ;
    dec         rdi                            ;
    jne         L8                             ;
    neg         rdx                            ;
    neg         rax                            ;
    sbb         rdx, 0                         ;
L8:                                            ;
    pop         rsi                            ;
    pop         rdi                            ;
    pop         rbx                            ;
    mov         qword ptr[r8], rax             ;
    mov         qword ptr[r8+8], rdx           ;
    ret                                        ;
int128div ENDP

;void int128rem(_int128 &dst, _int128 &x); do assignop dst %= x; if (x < 0) x = -x; !!
int128rem PROC
    push        rbx                            ;
    push        rdi                            ;
    mov         r8, rcx                        ; r8 = &dst
    mov         r9, rdx                        ; r9 = &x
    xor         rdi, rdi                       ;
    mov         rax, qword ptr[r8+8]           ; check sign of dst
    or          rax, rax                       ;
    jge         L1                             ;
    inc         rdi                            ;
    mov         rdx, qword ptr[r8]             ; dst is negative. change sign
    neg         rax                            ;
    neg         rdx                            ;
    sbb         rax, 0                         ;
    mov         qword ptr[r8+8], rax           ; dst = -original dst
    mov         qword ptr[r8], rdx             ;
L1:                                            ; Assume dst>=0, rdi = (original dst < 0)1:0
    mov         rax, qword ptr[r9+8]           ; check sign of x
    or          rax, rax                       ;
    jge         L2                             ; if(x >= 0) goto L2
    mov         rdx, qword ptr[r9]             ; x is negative. Change sign
    neg         rax                            ;
    neg         rdx                            ;
    sbb         rax, 0                         ;
    mov         qword ptr[r9+8], rax           ;
    mov         qword ptr[r9], rdx             ;
L2:                                            ; Assume dst>=0 and x > 0, (original dst < 0)1:0. dont care about orignal sign of x
    or          rax, rax                       ;
    jne         L3                             ;
    mov         rcx, qword ptr[r9]             ;
    mov         rax, qword ptr[r8+8]           ;
    xor         rdx, rdx                       ;
    div         rcx                            ;
    mov         rax, qword ptr[r8]             ;
    div         rcx                            ;
    mov         rax, rdx                       ;
    xor         rdx, rdx                       ;
    dec         rdi                            ;
    jns         L4                             ;
    jmp         L8                             ;
L3:                                            ;
    mov         rbx, rax                       ;
    mov         rcx, qword ptr[r9]             ;
    mov         rdx, qword ptr[r8+8]           ;
    mov         rax, qword ptr[r8]             ;
L5:                                            ;
    shr         rbx, 1                         ;
    rcr         rcx, 1                         ;
    shr         rdx, 1                         ;
    rcr         rax, 1                         ;
    or          rbx, rbx                       ;
    jne         L5                             ;
    div         rcx                            ;
    mov         rcx, rax                       ;
    mul         qword ptr[r9+8]                ;
    xchg        rax, rcx                       ;
    mul         qword ptr[r9]                  ;
    add         rdx, rcx                       ;
    jb          L6                             ;
    cmp         rdx, qword ptr[r8+8]           ;
    ja          L6                             ;
    jb          L7                             ;
    cmp         rax, qword ptr[r8]             ;
    jbe         L7                             ;
L6:                                            ;
    sub         rax, qword ptr[r9]             ;
    sbb         rdx, qword ptr[r9+8]           ;
L7:                                            ;
    sub         rax, qword ptr[r8]             ;
    sbb         rdx, qword ptr[r8+8]           ;
    dec         rdi                            ;
    jns         L8                             ;
L4:                                            ;
    neg         rdx                            ;
    neg         rax                            ;
    sbb         rdx, 0                         ;
L8:                                            ;
    pop         rdi                            ;
    pop         rbx                            ;
    mov         qword ptr[r8], rax             ;
    mov         qword ptr[r8+8], rdx           ;
    ret                                        ;
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

;void int128dec(_int128 &x); set x = x - 1;
int128dec PROC
    sub         qword ptr[rcx], 1
    sbb         qword ptr[rcx+8], 0
    ret
int128dec ENDP

;void int128shr(int shft, _int128 &x); do assignop x >>= shft; (if(x<0) shift 1-bits in from left, else 0-bits)
int128shr PROC
    mov         rax, qword ptr[rdx+8]          ; rax = x.hi
    cmp         cl, 40h
    jae         More64                         ; if(cl >= 64) goto More64;
    sar         qword ptr[rdx+8], cl           ; shift x.hi
    shrd        qword ptr[rdx], rax, cl        ; shift x.lo taking new bits from x.hi (rax)
    ret
More64:                                        ; assume rax = x.hi
    cmp         cl, 80h
    jae         RetSign                        ; if(cl >= 128) goto RetSign;
    sar         qword ptr[rdx+8], 3Fh          ; set all bits in x.hi to sign-bit
    and         cl, 3Fh                        ; cl %= 64
    sar         rax, cl                        ; rax = x.hi >> cl
    mov         qword ptr[rdx], rax            ; x.lo = rax
    ret
RetSign:                                       ; assume rax = x.hi
    sar         rax,3Fh                        ; set all bits in rax to sign-bit
    mov         qword ptr[rdx], rax
    mov         qword ptr[rdx+8], rax
    ret
int128shr ENDP

;void int128shl(int shft, _int128 &x); do assignop x <<= shft;
int128shl PROC
    cmp         cl, 40h
    jae         More64                         ; if(cl >= 64) goto More64;
    mov         rax, qword ptr[rdx]            ; rax = x.lo
    shl         qword ptr[rdx], cl             ; shift x.lo
    shld        qword ptr[rdx+8], rax, cl      ; shift x.hi taking new bits from x.lo (rax)
    ret
More64:
    cmp         cl, 80h
    jae         RetZero                        ; if(cl >= 128) goto RetZero;
    and         cl, 3Fh                        ; cl %= 64
    mov         rax, qword ptr[rdx]            ; rax = x.lo
    shl         rax, cl                        ; shift rax (x.lo)
    mov         qword ptr[rdx+8], rax          ; x.hi = rax
    xor         rax, rax
    mov         qword ptr[rdx], rax            ; x.lo = 0
    ret
RetZero:
    xor         rax, rax                       ; return 0
    mov         qword ptr[rdx], rax
    mov         qword ptr[rdx+8], rax
    ret
int128shl ENDP

;int int128cmp(const _int128 &x1, const _int128 &x2); return sign(x1 - x2);
int128cmp PROC
    mov         rax, qword ptr[rcx+8]          ; x1.hi
    cmp         rax, qword ptr[rdx+8]          ; x2.hi
    jl          lessthan                       ; signed compare of x1.hi and x2.hi
    jg          greaterthan
    mov         rax, qword ptr[rcx]            ; x1.lo
    cmp         rax, qword ptr[rdx]            ; x2.lo
    jb          lessthan                       ; unsigned compare of x1.lo and x2.lo
    ja          greaterthan
    xor         eax, eax                       ; they are equal
    ret
greaterthan:
    mov         eax, 1
    ret
lessthan:
    mov         eax, -1
    ret
int128cmp ENDP

;void uint128div(_uint128 &dst, const _uint128 &x); do assignop dst /= x;
uint128div PROC
     push       rbx                            ; same as signed division
     push       rsi                            ; but without sign check on arguments
     mov        r8, rcx                        ; r8 = &dst
     mov        r9, rdx                        ; r9 = &x
     mov        rax, qword ptr[r9+8]           ; rax = x.hi
     or         rax, rax                       ;
     jne        L1                             ; if(x.hi == 0) {
     mov        rcx, qword ptr[r9]             ;   load x.lo
     mov        rax, qword ptr[r8+8]           ;   load dst.hi
     xor        rdx, rdx                       ;   rax:rdx == dst.hi:0
     div        rcx                            ;   rax = q.hi. rdx = remainder.hi
     mov        rbx, rax                       ;   rbx = q.hi
     mov        rax, qword ptr[r8]             ;   rdx:rax <- remainder:lo word of dividend
     div        rcx                            ;   rax:rdx = q.lo:remainder
     mov        rdx, rbx                       ;   rdx:rax <- q hi:q lo
     jmp        L2                             ;   restore stack and return
                                               ; }
L1:                                            ; Assume rax = x.hi
     mov        rcx, rax                       ;
     mov        rbx, qword ptr[r9]             ; rcx:rbx == x (divisor)
     mov        rdx, qword ptr[r8+8]           ;
     mov        rax, qword ptr[r8]             ; rdx:rax == dst (dividend)
L3:                                            ; do {
     shr        rcx, 1                         ;   rshift divisor 1; hi bit:=0
     rcr        rbx, 1                         ;
     shr        rdx, 1                         ;   rshift dividend 1; hi bit:=0
     rcr        rax, 1                         ;
     or         rcx, rcx                       ;
     jne        L3                             ; } while(divisor.hi != 0);
     div        rbx                            ; rax = quotiet. rdx = remainder (ignore)
     mov        rsi, rax                       ; rsi = quotient (q)
                                               ; q maybe 1 too big, so calculate t = q * x
                                               ; and compare t and dst (original dividend)
                                               ; Note that we must also check for overflow, which can occur if
                                               ; dst is close to 2**128 and the q is 1 too big
     mul        qword ptr[r9+8]                ; rax:rdx = q *= x.hi
     mov        rcx, rax                       ; rcx = (q*x.hi)
     mov        rax, qword ptr[r9]             ; rax = dst.lo
     mul        rsi                            ; rax;rdx = q * x.lo
     add        rdx, rcx                       ; rdx:rax = t = quotient * x
     jc         L4                             ; carry means q is 1 too big
     cmp        rdx, qword ptr[r8+8]           ; compare t.hi and dst.hi
     ja         L4                             ; if(t.hi > dst.hi) q--
     jb         L5                             ; if(t.hi < dst.hi) we are done
     cmp        rax, qword ptr[r8]             ; t.hi == dst.hi. now compare lo words
     jbe        L5                             ; if(t.lo <= dst.lo) we are done, else q--
L4:
     dec        rsi                            ; quotient--
L5:
     xor        rdx, rdx
     mov        rax, rsi                       ; rdx:rax <- quotient
L2:
     pop        rsi
     pop        rbx
     mov        qword ptr[r8], rax
     mov        qword ptr[r8+8], rdx
     ret
uint128div ENDP

; calculates unsigned remainder
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
     div        rcx                            ; rdx <- remainder, rax <- quotient
     mov        rax, qword ptr[r8]             ; rdx:rax <- remainder:lo word of dividend
     div        rcx                            ; rdx <- final remainder
     mov        rax, rdx                       ; rdx:rax <- remainder
     xor        rdx, rdx
     jmp        L2                             ; restore stack and return
L1:
     mov        rcx, rax
     mov        rbx, qword ptr[r9]
     mov        rdx, qword ptr[r8+8]
     mov        rax, qword ptr[r8]
L3:                                            ;
     shr        rcx, 1                         ;
     rcr        rbx, 1                         ;
     shr        rdx, 1                         ;
     rcr        rax, 1                         ;
     or         rcx, rcx                       ;
     jne        L3                             ;
     div        rbx                            ;
     mov        rcx, rax                       ; save quotient
     mul        qword ptr[r9+8]                ;
     xchg       rax, rcx                       ; put partial product in rcx, get quotient in rax
     mul        qword ptr[r9]                  ;
     add        rdx, rcx                       ;
     jb         L4                             ;
     cmp        rdx, qword ptr[r8+8]           ;
     ja         L4                             ;
     jb         L5                             ;
     cmp        rax, qword ptr[r8]             ;
     jbe        L5                             ;
L4:                                            ;
     sub        rax, qword ptr[r9]             ; subtract divisor from result
     sbb        rdx, qword ptr[r9+8]           ;
L5:                                            ;
                                               ; Calculate remainder by subtracting the result from the original dividend.
                                               ; Since the result is already in a register, we will perform the subtract in
                                               ; the opposite direction and negate the result to make it positive.
     sub        rax, qword ptr[r8]             ;
     sbb        rdx, qword ptr[r8+8]           ;
     neg        rdx                            ;
     neg        rax                            ;
     sbb        rdx, 0                         ;
L2:                                            ;
     pop        rbx                            ;
     mov        qword ptr[r8], rax             ;
     mov        qword ptr[r8+8], rdx           ;
     ret                                       ;
uint128rem ENDP

;typedef struct {
;  _uint128 quot;
;  _uint128 rem;
;} _ui128div_t;
;
;void uint128quotrem(const _uint128 &numer, const _uint128 &denom, _ui128div_t *qr);
uint128quotrem PROC
     push       rbx                            ; same as signed division
     push       rsi                            ; but without sign check on arguments
     push       rdi
     mov        rdi, r8                        ; rdi = qr
     mov        r8, rcx                        ; r8  = &num
     mov        r9, rdx                        ; r9  = &denom
     mov        rax, qword ptr[r9+8]           ; rax = denom.hi
     or         rax, rax                       ;
     jne        L1                             ; if(denom.hi == 0) {
     mov        rcx, qword ptr[r9]             ;   rcx = denom.lo
     mov        rax, qword ptr[r8+8]           ;   rax = numer.hi
     xor        rdx, rdx                       ;   rax:rdx == numer.hi:0
     div        rcx                            ;   rax = q.hi. rdx = remainder.hi
     mov        qword ptr[rdi+8], rax          ;   save q.hi
     mov        rax, qword ptr[r8]             ;   rdx:rax <- remainder:lo word of dividend
     div        rcx                            ;   rax:rdx = q.lo:remainder
     mov        qword ptr[rdi], rax            ;   save q.lo
     mov        qword ptr[rdi+10h], rdx        ;   save remainder.lo
     xor        rax, rax                       ;
     mov        qword ptr[rdi+18h], rax        ;   remainder.hi = 0
     jmp        L2                             ;   restore stack and return
                                               ; }
L1:                                            ; Assume rax = denom.hi
     mov        rcx, rax                       ;
     mov        rbx, qword ptr[r9]             ; rcx:rbx == denom
     mov        rdx, qword ptr[r8+8]           ;
     mov        rax, qword ptr[r8]             ; rdx:rax == numer
L3:                                            ; do {
     shr        rcx, 1                         ;   rshift denom 1; hi bit:=0
     rcr        rbx, 1                         ;
     shr        rdx, 1                         ;   rshift numer 1; hi bit:=0
     rcr        rax, 1                         ;
     or         rcx, rcx                       ;
     jne        L3                             ; } while(denom.hi != 0);
     div        rbx                            ; rax = qquotient. rdx = remainder (ignore)
     mov        rsi, rax                       ; rsi = quotient (q)
                                               ; q maybe 1 too big, so calculate t = q * denom
                                               ; and compare t and numer (original dividend)
                                               ; Note that we must also check for overflow, which can occur if
                                               ; numer is close to 2**128 and the q is 1 too big
     mul        qword ptr[r9+8]                ; rax:rdx = q *= denom.hi
     mov        rcx, rax                       ; rcx = (q*denom.hi).hi
     mov        rax, qword ptr[r9]             ;
     mul        rsi                            ; rdx:rax *= q
     add        rdx, rcx                       ; rdx:rax = t = q * denom
     jc         L4                             ; carry means q is 1 too big
     cmp        rdx, qword ptr[r8+8]           ; compare t.hi and numer.hi
     ja         L4                             ; if(t.hi > numer.hi) q--
     jb         L5                             ; if(t.hi < numer.hi) we are done
     cmp        rax, qword ptr[r8]             ; t.hi == numer.hi. now compare lo words
     jbe        L5                             ; if(t.lo <= numer.lo) we are done, else q--
L4:                                            ; Assume: rsi = q, rdx:rax = q * denom and q is 1 too big
     dec        rsi                            ; q--
     sub        rax, qword ptr[r9]             ; t -= denom
     sbb        rdx, qword ptr[r9+8]
L5:
     sub        rax, qword ptr[r8]             ; t -= numer
     sbb        rdx, qword ptr[r8+8]
     neg        rdx                            ; remainder = -t
     neg        rax                            ;
     sbb        rdx, 0                         ; rsi = final quotient, rdx:rax = final remainder
     mov        qword ptr[rdi], rsi            ; rq->quot.lo = rsi
     xor        rbx, rbx
     mov        qword ptr[rdi+8], rbx          ; rq->quot.hi = 0
     mov        qword ptr[rdi+10h], rax        ; rq->rem.lo  = rax
     mov        qword ptr[rdi+18h], rdx        ; rq->rem.hi  = rdx
L2:
     pop        rdi
     pop        rsi
     pop        rbx
     ret
uint128quotrem ENDP


;void uint128shr(int shft, void *x); do assignop x >>= shft. always shift 0-bits in from left
uint128shr PROC
    cmp         cl, 40h
    jae         More64                         ; if(cl >= 64) goto More64;
    mov         rax, qword ptr[rdx+8]          ; rax = x.hi
    shr         qword ptr[rdx+8], cl           ; shift x.hi
    shrd        qword ptr[rdx], rax, cl        ; shift x.lo taking new bits from x.hi (rax)
    ret
More64:
    cmp         cl, 80h
    jae         RetZero                        ; if(cl >= 128) goto RetZero;
    and         cl, 3Fh                        ; cl %= 64
    mov         rax, qword ptr[rdx+8]          ; rax = x.hi
    shr         rax, cl                        ; rax >>= cl
    mov         qword ptr[rdx], rax            ; x.lo = rax
    xor         rax, rax
    mov         qword ptr[rdx+8], rax          ; x.hi = 0
    ret
RetZero:
    xor         rax, rax                       ; return 0
    mov         qword ptr[rdx], rax
    mov         qword ptr[rdx+8], rax
    ret
uint128shr ENDP

;int uint128cmp(const _uint128 &x1, const _uint128 &x2); return sign(x1 - x2);
uint128cmp PROC
     mov        rax, qword ptr[rcx+8]          ; x1.hi
     cmp        rax, qword ptr[rdx+8]          ; x2.hi
     jb         lessthan                       ; unsigned compare of x1.hi and x2.hi
     ja         greaterthan
     mov        rax, qword ptr[rcx]            ; x1.lo
     cmp        rax, qword ptr[rdx]            ; x2.lo
     jb         lessthan                       ; unsigned compare of x1.lo and x2.lo
     ja         greaterthan
     xor        eax, eax                       ; they are equal
     ret
greaterthan:
     mov        eax, 1
     ret
lessthan:
     mov        eax, -1
     ret
uint128cmp ENDP

END
