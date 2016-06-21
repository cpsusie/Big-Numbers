; To be used in x64 mode when using class Double80
;
.xlist
include fpu.inc
.list

.DATA

maxI32          dword 7fffffffh
maxI32P1        dword 80000000h
maxI64          qword 7fffffffffffffffh
maxI64P1        qword 8000000000000000h
d80Epsilon      tbyte 0000000000000080c03fh  ; 1.08420217248550443e-019
d80Minimum      tbyte 00000000000000800100h  ; 3.36210314311209209e-4932
d80Maximum      tbyte 0fffffffffffffffffe7fh ; 1.18973149535723227e+4932
m2pi2pow60      tbyte 403dc90fdaa22168c235h  ; 2*pi*pow2(60) (=7.244019458077122e+018)
m1e18           tbyte 403ade0b6b3a76400000h  ; 1e18
m1e18m1         tbyte 403ade0b6b3a763ffff0h  ; m1e18 - 1
m10             tbyte 4002a000000000000000h  ; 10

.CODE


;---------------------------------------------- FPU control function ----------------------------------------

;void FPUinit();
FPUinit PROC
    fninit
    ret
FPUinit ENDP

;void FPUgetStatusWord(WORD &dst);
FPUgetStatusWord PROC
    fnstsw WORD PTR[rcx]
    ret
FPUgetStatusWord ENDP

;void FPUgetControlWord(WORD &dst);
FPUgetControlWord PROC
    fnstcw WORD PTR[rcx]
    ret
FPUgetControlWord ENDP

;void FPUsetControlWord(WORD &flags);
FPUsetControlWord PROC
    fldcw WORD PTR[rcx]
    ret
FPUsetControlWord ENDP

;FPUgetTagsWord(WORD *dst);
FPUgetTagsWord PROC
    fstenv WORD PTR[rcx]
    ret
FPUgetTagsWord ENDP

;void FPUclearExceptions();
FPUclearExceptions PROC
    fclex
    ret
FPUclearExceptions ENDP

;----------------------------------------------Double80 Constructors ------------------------------------------------

;void D80consLong(Double80 &dst, long &x);
D80consLong PROC
    fild    DWORD PTR [rdx]
    fstp    TBYTE PTR [rcx]
    ret
D80consLong ENDP


;void D80consULong(Double80 &dst, unsigned long x);
D80consULong PROC
    cmp     edx, maxI32
    jbe     DoSmallInt32

    and     edx, maxI32
    push    rdx
    fild    DWORD PTR [rsp]
    push    7fffffffh
    fild    DWORD PTR [rsp]
    faddp
    fld1
    faddp
    fstp    TBYTE PTR [rcx]
    add     rsp, 16
    ret

DoSmallInt32:
    push    rdx
    fild    DWORD PTR [rsp]
    add     rsp, 8
    fstp    TBYTE PTR [rcx]
    ret
D80consULong ENDP

; void D80consLongLong(Double80 &dst, const __int64 &x);
D80consLongLong PROC
    fild    QWORD PTR [rdx]
    fstp    TBYTE PTR [rcx]
    ret
D80consLongLong ENDP

; void D80consULongLong(Double80 &dst, const unsigned __int64 x);
D80consULongLong PROC
    cmp     rdx,0
    jg      DoSmallInt64

    and     rdx, maxI64
    push    rdx
    fild    QWORD PTR [rsp]
    push    maxI64
    fild    QWORD PTR [rsp]
    faddp
    fld1
    faddp
    fstp    TBYTE PTR [rcx]
    add     rsp, 16
    ret

DoSmallInt64:
    push    rdx
    fild    QWORD PTR [rsp]
    add     rsp, 8
    fstp    TBYTE PTR [rcx]
    ret
D80consULongLong ENDP

; void D80consFloat(Double80 &dst, float &x);
D80consFloat PROC
    fld     DWORD PTR[rdx]
    fstp    TBYTE PTR[rcx]
    ret
D80consFloat ENDP

;void D80consDouble(Double80 &dst, const double &x);
D80consDouble PROC
    fld     QWORD PTR[rdx]
    fstp    TBYTE PTR[rcx]
    ret
D80consDouble ENDP

; ---------------------- Conversion functions from Double80 -> long,ulong,longlong ulonglong,float,double

;void D80ToLong(long &dst, const Double80 &src);
D80ToLong PROC
    fld     TBYTE PTR[rdx]
    fistp   DWORD PTR[rcx]
    ret
D80ToLong ENDP

;void D80ToULong(unsigned long &dst, const Double80 &src);
D80ToULong PROC
    fld     TBYTE PTR[rdx]
    ficom   maxI32
    fnstsw  ax
    sahf
    jbe     GetSmallInt32

    fild    maxI32P1                 ; x >= maxInt32
    fchs
    fsub
    fistp   DWORD PTR[rcx]
    mov     eax, DWORD PTR[rcx]
    add     eax, maxI32P1
    mov     DWORD PTR[rcx], eax
    ret
GetSmallInt32:
    fistp   DWORD PTR[rcx]
    ret
D80ToULong ENDP

;void D80ToLongLong(long long &dst, const Double80 &src);
D80ToLongLong PROC
    fld     TBYTE PTR[rdx]
    fistp   QWORD PTR[rcx]
    ret
D80ToLongLong ENDP

;void D80ToULongLong(unsigned long long &dst, const Double80 &src);
D80ToULongLong PROC
    fld     TBYTE PTR[rdx]
    fild    maxI64
    fcomip  st, st(1)
    jae     GetSmallInt64

    fild    maxI64P1                 ; x >= maxInt64
    fchs
    fsub
    fistp   QWORD PTR[rcx]
    mov     rax, QWORD PTR[rcx]
    add     rax, maxI64P1
    mov     QWORD PTR[rcx], rax
    ret
GetSmallInt64:
    fistp   QWORD PTR[rcx]
    ret
D80ToULongLong ENDP

;void D80ToFloat(float &dst, const Double80 &src);
D80ToFloat PROC
    fld     TBYTE PTR [rdx]
    fstp    DWORD PTR[rcx]
    ret
D80ToFloat ENDP

;void D80ToDouble(double &dst, const Double80 &src);
D80ToDouble PROC
    fld     TBYTE PTR[rdx]
    fstp    QWORD PTR[rcx]
    ret
D80ToDouble ENDP

; ---------------------- Binary operators +,-,*,- (and unary minus) and compare ------------------------------------------

;void D80D80sum(TenByteClass &dst, const Double80 &x, const Double80 &y);
D80D80sum PROC
    fld     TBYTE PTR[rdx]  ; load x
    fld     TBYTE PTR[r8 ]  ; load y
    fadd
    fstp    TBYTE PTR[rcx]  ; pop result
    ret
D80D80sum ENDP

;void D80D80dif(TenByteClass &dst, const Double80 &x, const Double80 &y);
D80D80dif PROC
    fld     TBYTE PTR[rdx]  ; load x
    fld     TBYTE PTR[r8 ]  ; load y
    fsub
    fstp    TBYTE PTR[rcx]  ; pop result
    ret
D80D80dif ENDP

;void D80neg(TenByteClass &dst, const Double80 &x);
D80neg PROC
    fld     TBYTE PTR[rdx]  ; load x
    fchs
    fstp    TBYTE PTR[rcx]  ; pop result
    ret
D80neg ENDP

;void D80D80mul(TenByteClass &dst, const Double80 &x, const Double80 &y);
D80D80mul PROC
    fld     TBYTE PTR[rdx]  ; load x
    fld     TBYTE PTR[r8 ]  ; load y
    fmul
    fstp    TBYTE PTR[rcx]  ; pop result
    ret
D80D80mul ENDP

;void D80D80div(TenByteClass &dst, const Double80 &x, const Double80 &y);
D80D80div PROC
    fld     TBYTE PTR[rdx]  ; load x
    fld     TBYTE PTR[r8 ]  ; load y
    fdiv
    fstp    TBYTE PTR[rcx]  ; pop result
    ret
D80D80div ENDP

;int D80D80Compare(const Double80 &x, const Double80 &y);
D80D80Compare PROC
    fld     TBYTE PTR[rdx]  ; load y
    fld     TBYTE PTR[rcx]  ; load x
    fcomip  st, st(1)                   ; st(0)=x, st(1)=y
    ja      XAboveY
    jb      XBelowY
    xor     rax,rax
    fstp    st(0)
    ret
XAboveY:
    fstp    st(0)
    mov     rax, 1
    ret
XBelowY:
    fstp    st(0)
    mov     rax, -1
    ret
D80D80Compare ENDP

;int D80isZero(const Double80 &x);
D80isZero PROC
    fld     TBYTE PTR [rcx]
    fldz
    fcomip  st, st(1)            ; compare and pop zero
    je      IsZero               ; if st(0) == st(1) (this != zero) goto IsZero
    xor     rax, rax             ; rax = 0 (false)
    fstp    st(0)
    ret
IsZero:
    mov     rax, 1               ; rax = 1 (true)
    fstp    st(0)                ; pop x
    ret
D80isZero ENDP

; ---------------------------------------- assign operators +=,-=,*=,/=,++,-- -------------------------------------

;void D80assignAdd(Double80 &dst, const Double80 &x);
D80assignAdd PROC
    fld     TBYTE PTR[rcx]  ; load dst
    fld     TBYTE PTR[rdx]  ; load x
    fadd
    fstp    TBYTE PTR[rcx]  ; pop to dst
    ret
D80assignAdd ENDP

;void D80assignSub(Double80 &dst, const Double80 &x);
D80assignSub PROC
    fld     TBYTE PTR[rcx]  ; load dst
    fld     TBYTE PTR[rdx]  ; load x
    fsub
    fstp    TBYTE PTR[rcx]  ; pop to dst
    ret
D80assignSub ENDP

;void D80assignMul(Double80 &dst, const Double80 &x);
D80assignMul PROC
    fld     TBYTE PTR[rcx]  ; load dst
    fld     TBYTE PTR[rdx]  ; load x
    fmul
    fstp    TBYTE PTR[rcx]  ; pop to dst
    ret
D80assignMul ENDP

;void D80assignDiv(Double80 &dst, const Double80 &x);
D80assignDiv PROC
    fld     TBYTE PTR[rcx]  ; load dst
    fld     TBYTE PTR[rdx]  ; load x
    fdiv
    fstp    TBYTE PTR[rcx]  ; pop to dst
    ret
D80assignDiv ENDP

;void D80increment(Double80 &dst);
D80increment PROC
    fld     TBYTE PTR[rcx]  ; load dst
    fld1
    fadd
    fstp    TBYTE PTR[rcx]  ; pop to dst
    ret
D80increment ENDP

;void D80decrement(Double80 &dst);
D80decrement PROC
    fld     TBYTE PTR[rcx]  ; load dst
    fld1
    fsub
    fstp    TBYTE PTR[rcx]  ; pop to dst
    ret
D80decrement ENDP

; ------------------------------------------Misc functions ---------------------------------------------------------
;void D80getPi(Double80 &dst);
D80getPi PROC
    fldpi
    fstp    TBYTE PTR[rcx]
    ret
D80getPi ENDP

;void D80getEps(Double80 &dst);
D80getEps PROC
    fld     d80Epsilon
    fstp    TBYTE PTR[rcx]
    ret
D80getEps ENDP

;void D80getMin(Double80 &dst);
D80getMin PROC
    fld     d80Minimum
    fstp    TBYTE PTR[rcx]
    ret
D80getMin ENDP

;void D80getMax(Double80 &dst);
D80getMax PROC
    fld     d80Maximum
    fstp    TBYTE PTR[rcx]
    ret
D80getMax ENDP

; -------------------------------------------------- Double80 Functions ----------------------------------------

;void D80getExpo2(int &dst, const Double80 &x);
D80getExpo2 PROC
    fld     TBYTE PTR [rdx]
    fxtract
    fstp    st(0)
    fistp   DWORD PTR[rcx]
    ret
D80getExpo2 ENDP

;void D80getExpo10(int &dst, const Double80 &x);
D80getExpo10 PROC
    fld     TBYTE PTR [rdx]
    fldz
    fcomip  st, st(1)            ; compare x and pop 0 
    jne     x_not_zero           ; if(x != 0) goto x_not_zero
    fstp    st(0)                ; pop x
    mov     DWORD PTR[rcx], 0    ; x == 0 => result = 0
    ret
x_not_zero:
    fld1
    fxch    st(1)
    fabs
    fyl2x
    fldlg2
    fmul
    pushRoundMode ROUNDDOWN
    frndint
    popRoundMode
    fistp   DWORD PTR[rcx]
    ret
D80getExpo10 ENDP

; void D80fabs(TenByteClass &dst, const Double80 &x);
D80fabs PROC
    fld     TBYTE PTR[rdx]
    fabs
    fstp    TBYTE PTR[rcx]
    ret
D80fabs ENDP

;void D80sqr(TenByteClass &dst, const Double80 &x);
D80sqr PROC
    fld     TBYTE PTR[rdx]
    fld     st(0)
    fmul
    fstp    TBYTE PTR[rcx]
    ret
D80sqr ENDP

;void D80sqrt(TenByteClass &dst, const Double80 &x);
D80sqrt PROC
    fld     TBYTE PTR[rdx]
    fsqrt
    fstp    TBYTE PTR[rcx]
    ret
D80sqrt ENDP

;void D80modulus(TenByteClass &dst, const Double80 &x, const Double80 &y);
D80modulus PROC
    fld     TBYTE PTR [r8]      ;                                                     st0=y
    fabs                        ; y = abs(y)                                          st0=|y|
    fld     TBYTE PTR [rdx]     ;                                                     st0=x,st1=|y|
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
    fstp    TBYTE PTR[rcx]      ; pop result
    ret                         ; return

Repeat_Negative_x:              ; do {                                                st0=x,st=|y|, x < 0
    fprem                       ;    st0 %= y
    fnstsw  ax
    sahf
    jpe     Repeat_Negative_x   ; } while(statusword.c2 != 0)
    fldz
    fcomip  st, st(1)           ; compare and pop zero
    jae     pop2                ; if(st(0) >= st(1) (0 >= remainder) goto pop2
    fsubr                       ; remainder -= y
    fstp    TBYTE PTR[rcx]      ; pop result
    ret                         ; return

pop2:                           ;                                                     st0=x%y,st1=y
    fstp    TBYTE PTR[rcx]      ; pop result
    fstp    st(0)               ; pop y
    ret
D80modulus ENDP

; ----------------------------------------- Double80 trigonometric functions ----------------------------------------

;void D80sin(TenByteClass &dst, const Double80 &x);
D80sin PROC
    lea     r8, m2pi2pow60
    call    D80modulus
    fld     TBYTE PTR[rcx]
    fsin
    fstp    TBYTE PTR[rcx]
    ret
D80sin ENDP

;void D80cos(TenByteClass &dst, const Double80 &x);
D80cos PROC
    lea     r8, m2pi2pow60
    call    D80modulus
    fld     TBYTE PTR[rcx]
    fcos
    fstp    TBYTE PTR[rcx]
    ret
D80cos ENDP

;void D80tan(TenByteClass &dst, const Double80 &x);
D80tan PROC
    lea     r8, m2pi2pow60
    call    D80modulus
    fld     TBYTE PTR[rcx]
    fptan
    fstp    st
    fstp    TBYTE PTR[rcx]
    ret
D80tan ENDP

;void D80atan(TenByteClass &dst, const Double80 &x);
D80atan PROC
    fld     TBYTE PTR[rdx]
    fld1
    fpatan
    fstp    TBYTE PTR[rcx]
    ret
D80atan ENDP

;void D80atan2(TenByteClass &dst, const Double80 &y, const Double80 &x);
D80atan2 PROC
    fld     TBYTE PTR[rdx]
    fld     TBYTE PTR[r8]
    fpatan
    fstp    TBYTE PTR[rcx]
    ret
D80atan2 ENDP

;void D80sincos(Double80 &c, Double80 &s);
D80sincos PROC
    push    rdx
    mov     rdx, rcx
    lea     r8, m2pi2pow60
    call    D80modulus
    pop     rdx
    fld     TBYTE PTR[rcx]
    fsincos
    fstp    TBYTE PTR [rcx]
    fstp    TBYTE PTR [rdx]
    ret
D80sincos ENDP

; --------------------------------------------------- Double80 Exponential and Logarithmic functions ------------------

;void D80exp(TenByteClass &dst, const Double80 &x);
D80exp PROC
    fld     TBYTE PTR [rdx]
    fldl2e
    fmul
    fld     st(0)
    pushRoundMode ROUNDDOWN
    frndint
    popRoundMode
    fsub    st(1), st(0)
    fxch    st(1)
    f2xm1
    fld1
    fadd
    fscale
    fstp    st(1)
    fstp    TBYTE PTR[rcx]
    ret
D80exp ENDP

;void D80log(TenByteClass &dst, const Double80 &x);
D80log PROC
    fld1
    fld     TBYTE PTR[rdx]
    fyl2x
    fldln2
    fmul
    fstp    TBYTE PTR[rcx]
    ret
D80log ENDP

;void D80log10(TenByteClass &dst, const Double80 &x);
D80log10 PROC
    fld1
    fld     TBYTE PTR[rdx]
    fyl2x
    fldlg2
    fmul
    fstp    TBYTE PTR[rcx]
    ret
D80log10 ENDP

;void D80log2(TenByteClass &dst, const Double80 &x);
D80log2 PROC
    fld1
    fld     TBYTE PTR[rdx]
    fyl2x
    fstp    TBYTE PTR[rcx]
    ret
D80log2 ENDP

;void D80pow(TenByteClass &dst, const Double80 &x, const Double80 &y);
D80pow PROC
    fld     TBYTE PTR[r8]
    fldz
    fcomip  st, st(1)
    je ZeroExponent

    fld     TBYTE PTR[rdx]
    fldz
    fcomip  st, st(1)
    je      ZeroBase
                            ; st(0)=x, st(1)=y
    fyl2x
    fld     st(0)
    pushRoundMode ROUNDDOWN
    frndint
    popRoundMode
    fsub    st(1), st(0)
    fxch    st(1)
    f2xm1
    fld1
    fadd
    fscale
    fstp    st(1)
    fstp    TBYTE PTR[rcx]
    ret
ZeroExponent:               ; st(0)=y
    fstp    st(0)           ; pop y
    fld1
    fstp    TBYTE PTR[rcx]
    ret
ZeroBase:                   ; st(0)=x, st(1)=y. x = 0. so st(0) = 0
    fcomip  st, st(1)
    ja      ZeroBaseNegativeExponent    
    fstp    st(0)           ; pop y
    fldz
    fstp    TBYTE PTR[rcx]  ; return 0
    ret
ZeroBaseNegativeExponent:   ; st(0)=y
    fstp    st(0)
    fld1
    fldz
    fdiv
    fstp    TBYTE PTR[rcx]  ; return 1/0 - error
    ret
D80pow ENDP

;void D80pow10(TenByteClass &dst, const Double80 &x);
D80pow10 PROC
    fld     TBYTE PTR[rdx]
    fldz
    fcomip  st, st(1)
    je      ZeroExponent

    fldl2t
    fmul
    fld     st(0)
    pushRoundMode ROUNDDOWN
    frndint
    popRoundMode
    fsub    st(1), st(0)
    fxch    st(1)
    f2xm1
    fld1
    fadd
    fscale
    fstp    st(1)
    fstp    TBYTE PTR[rcx]
    ret

ZeroExponent:               ; st(0)=x
    fstp    st(0)           ; pop x
    fld1
    fstp    TBYTE PTR[rcx]  ; return 1
    ret
D80pow10 ENDP

;void D80pow2(TenByteClass &dst, const Double80 &x);
D80pow2 PROC
    fld     TBYTE PTR[rdx]
    fldz
    fcomip  st, st(1)
    je      ZeroExponent

    fld     st(0)
    pushRoundMode ROUNDDOWN
    frndint
    popRoundMode
    fsub    st(1), st(0)
    fxch    st(1)
    f2xm1
    fld1
    fadd
    fscale
    fstp    st(1)
    fstp    TBYTE PTR[rcx]
    ret

ZeroExponent:               ; st(0)=x
    fstp    st(0)           ; pop x
    fld1
    fstp    TBYTE PTR[rcx]
    ret
D80pow2 ENDP

; ------------------------------------------------- Double80 floor,ceil --------------------------------

;void D80floor(TenByteClass &dst, const Double80 &x);
D80floor PROC
    fld     TBYTE PTR [rdx]
    pushRoundMode ROUNDDOWN
    frndint
    popRoundMode
    fstp    TBYTE PTR[rcx]
    ret
D80floor ENDP

;void D80ceil(TenByteClass &dst, const Double80 &x);
D80ceil PROC
    fld     TBYTE PTR [rdx]
    pushRoundMode ROUNDUP
    frndint
    popRoundMode
    fstp    TBYTE PTR[rcx]
    ret
D80ceil ENDP

; ------------------------------------------------ Double80 String functions -----------------------------------------

;void D80ToBCD(BYTE bcd[10], const TenByteClass &src);
D80ToBCD PROC
    fld     TBYTE PTR[rdx]
    fbstp   TBYTE PTR[rcx]
    ret
D80ToBCD ENDP

;void D80ToBCDAutoScale(BYTE bcd[10], const Double80 &x, int &expo10);
D80ToBCDAutoScale PROC
    mov     eax, DWORD PTR[r8]  ;
    cmp     eax, 0              ;
    jne     scale_x             ;
                                ;
    fld     TBYTE PTR[rdx]      ; 
    jmp     rescale             ;
                                ;
scale_x:                        ; Find m = x / 10^abs(expo10)
    fild    DWORD PTR[r8]       ;                                       st0=expo10
    fldl2t                      ;                                       st0=log2(10)         , st1=expo10
    fmul                        ;                                       st0=expo10*log2(10)
    fld     st(0)               ;                                       st0=expo10*log2(10)  , st1=st0
    pushRoundMode ROUNDDOWN     ;
    frndint                     ; Round down
    popRoundMode                ; Restore control word
    fsub    st(1), st(0)        ;
    fxch    st(1)               ;
    f2xm1                       ;
    fld1                        ;
    fadd                        ;
    fscale                      ;
    fstp    st(1)               ;                                       st0=10^expo10
                                ;
    fld     TBYTE PTR[rdx]      ;                                       st0=x          , st1=10^expo10
    fdivr                       ;                                       st0=x/10^expo10
                                ;
Rescale:                        ;                                       st0=m
    fld     m1e18               ;                                       st0=1e18       , st1=m
    fmul                        ; m *= 1e18                             st0=m
    mov     eax, DWORD PTR[r8]  ;                                       eax=expo10
    fld     m1e18m1             ;                                       st0=1e18-1     , st1=m
WhileLoop:                      ; while(|m| >= 1e18-1) {                st0=1e18-1     , st1=m
    fld     st(1)               ;                                       st0=m          , st1=1e18-1     , st2=m
    fabs                        ;                                       st0=|m|        , st1=1e18-1     , st2=m
    fcomip  st, st(1)           ;   compare |m| and 1e18-1 and pop |m|  st0=1e18-1     , st1=m
    jb      ExitLoop            ;   if(|m| < 1e18-1) break;             st0=1e18-1     , st1=m
    fld     m10                 ;                                       st0=10         , st1=1e18-1     , st2=m
    fdivp   st(2), st(0)        ;   m /= 10 and pop st0                 st0=1e18-1     , st1=m
    inc     eax                 ;   expo10++
    jmp     WhileLoop           ; }
ExitLoop:                            ;
    fstp    st(0)               ; Pop st(0)                             st0=m
    fbstp   TBYTE PTR[rcx]      ; Pop m into bcd                        Assertion: 1 <= |st0| < 1e18-1 and x = st0 * 10^(eax-18)
    mov     DWORD PTR[r8], eax  ; Restore expo10
    ret
D80ToBCDAutoScale ENDP

END
