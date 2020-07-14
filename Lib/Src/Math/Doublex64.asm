; To be used in x64 mode when using class MathFunctions

include fpu.inc

.DATA

maxI32       dword 7fffffffh
maxI32P1     dword 80000000h
maxI64       qword 7fffffffffffffffh
maxI64P1     qword 8000000000000000h
m2pi2pow60   tbyte 403dc90fdaa22168c235h  ; 2*pi*pow2(60) (=7.244019458077122e+018)

.CODE

;void d64rem(double &dst, const double &x, const Double80 &y);
d64rem PROC
    fld     tbyte ptr[r8]       ;                                                     st0=y
    fabs                        ; y = abs(y)                                          st0=|y|
    fld     qword ptr[rdx]      ;                                                     st0=x,st1=|y|
    fldz                        ;                                                     st0=0,st1=x,st2=|y|
    fcomip  st, st(1)           ; compare and pop zero                                st0=x,st1=|y|
    ja      Repeat_Negative_x   ; if st(0) > st(1) (0 > x) goto repeat_negative_x

Repeat_Positive_x:              ; do {                                                st0=x,st1=|y|, x > 0
    fprem                       ;   st0 %= y
    fnstsw  ax
    sahf
    jpe     Repeat_Positive_x   ; } while(statusword.c2 != 0);
    fldz                        ;                                                     st0=0,st1=x,st2=|y|
    fcomip  st, st(1)           ; compare and pop zero
    jbe     pop2                ; if(st(0) <= st(1) (0 <= remainder) goto pop2
    fadd                        ; remainder += y
    fstp    qword ptr[rcx]      ; pop result
    ret                         ; return

Repeat_Negative_x:              ; do {                                                st0=x,st=|y|, x < 0
    fprem                       ;   st0 %= y
    fnstsw  ax
    sahf
    jpe     Repeat_Negative_x   ; } while(statusword.c2 != 0)
    fldz
    fcomip  st, st(1)           ; compare and pop zero
    jae     pop2                ; if(st(0) >= st(1) (0 >= remainder) goto pop2
    fsubr                       ; remainder -= y
    fstp    qword ptr[rcx]      ; pop result
    ret                         ; return

pop2:                           ;                                                     st0=x%y,st1=y
    fstp    qword ptr[rcx]      ; pop result
    fstp    st(0)               ; pop y
    ret
d64rem ENDP

;double exp10(double x)
exp10 PROC
    movsd QWORD PTR[rsp-8], xmm0
    fld QWORD PTR[rsp-8]
    fldl2t
    fmul
    fld st(0)
    pushRoundMode ROUNDDOWN
    frndint
    popRoundMode
    fsub st(1),st(0)
    fxch st(1)
    f2xm1
    fld1
    fadd
    fscale
    fstp st(1)
    fstp QWORD PTR[rsp-8]
    movsd xmm0, QWORD PTR[rsp-8]
    ret
exp10 ENDP

; ----------------------------------------- Double80 trigonometric functions ----------------------------------------

;void sincos(double &c, double &s);
sincos PROC
    push    rdx
    mov     rdx, rcx
    lea     r8, m2pi2pow60
    call    d64rem              ; d64rem(c, c, 2pi*2^60)
    pop     rdx
    fld     qword ptr[rcx]      ; st0=c
    fsincos                     ; st0=cos(c), st1=sin(c)
    fstp    qword ptr[rcx]      ; pop c
    fstp    qword ptr[rdx]      ; pop s
    ret
sincos ENDP

END
