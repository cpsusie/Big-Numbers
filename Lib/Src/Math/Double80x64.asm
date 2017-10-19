; To be used in x64 mode when using class Double80
;
.xlist
include fpu.inc
.list

.DATA

DWmaxI32        dword              7fffffffh
DWmaxI32P1      dword              80000000h
QWmaxI64        qword      7fffffffffffffffh
QWmaxI64P1      qword      8000000000000000h
DmaxI32         qword      41dfffffffc00000h   ;  maxI32   as double
DmaxI32P1       qword      41e0000000000000h   ;  maxI32P1 as double
DNegmaxI32      qword     0c1dfffffffc00000h   ; -maxI32   as double
TBmaxI64        tbyte  403dfffffffffffffffeh   ;  maxI64   as Double80
TBmaxI64P1      tbyte  403e8000000000000000h   ;  maxI64P1 as Double80
TBNegMaxI64     tbyte 0c03dfffffffffffffffeh   ; -maxI64   as Double80
TBEpsilon       tbyte  0000000000000080c03fh   ; 1.08420217248550443e-019
TBMinimum       tbyte  00000000000000800100h   ; 3.36210314311209209e-4932
TBMaximum       tbyte 0fffffffffffffffffe7fh   ; 1.18973149535723227e+4932
TB2Pi2Pow60     tbyte  403dc90fdaa22168c235h   ; 2*pi*pow2(60) (=7.244019458077122e+018)
TB1e18          tbyte  403ade0b6b3a76400000h   ; 1e18
TB1e18M1        tbyte  403ade0b6b3a763ffff0h   ; TB1e18 - 1
TB10            tbyte  4002a000000000000000h   ; 10

.CODE

;---------------------------------------------- FPU control function ----------------------------------------

;void FPUinit();
FPUinit PROC
    fninit
    ret
FPUinit ENDP

;void FPUgetStatusWord(WORD &dst);
FPUgetStatusWord PROC
    fnstsw word ptr[rcx]
    ret
FPUgetStatusWord ENDP

;void FPUgetControlWord(WORD &dst);
FPUgetControlWord PROC
    fnstcw word ptr[rcx]
    ret
FPUgetControlWord ENDP

;void FPUsetControlWord(WORD &flags);
FPUsetControlWord PROC
    fldcw word ptr[rcx]
    ret
FPUsetControlWord ENDP

;FPUgetTagsWord(WORD *dst);
FPUgetTagsWord PROC
    fstenv word ptr[rcx]
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
    fild    dword ptr[rdx]
    fstp    tbyte ptr[rcx]
    ret
D80consLong ENDP


;void D80consULong(Double80 &dst, ULONG x);
D80consULong PROC
    cmp     edx, DWmaxI32
    jbe     DoSmallInt32

    and     edx, DWmaxI32
    push    rdx
    fild    dword ptr[rsp]
    fadd    DmaxI32
    fld1
    faddp
    fstp    tbyte ptr[rcx]
    add     rsp, 8
    ret

DoSmallInt32:
    push    rdx
    fild    dword ptr[rsp]
    add     rsp, 8
    fstp    tbyte ptr[rcx]
    ret
D80consULong ENDP

; void D80consLongLong(Double80 &dst, const INT64 &x);
D80consLongLong PROC
    fild    qword ptr[rdx]
    fstp    tbyte ptr[rcx]
    ret
D80consLongLong ENDP

; void D80consULongLong(Double80 &dst, const UINT64 x);
D80consULongLong PROC
    cmp     rdx, QWmaxI64
    jbe     DoSmallInt64

    and     rdx, QWmaxI64
    push    rdx
    fild    qword ptr[rsp]
    fld     TBmaxI64
    faddp
    fld1
    faddp
    fstp    tbyte ptr[rcx]
    add     rsp, 8
    ret

DoSmallInt64:
    push    rdx
    fild    qword ptr[rsp]
    add     rsp, 8
    fstp    tbyte ptr[rcx]
    ret
D80consULongLong ENDP

;void D80consFloat(Double80 &dst, float &x);
D80consFloat PROC
    fld     dword ptr[rdx]
    fstp    tbyte ptr[rcx]
    ret
D80consFloat ENDP

;void D80consDouble(Double80 &dst, const double &x);
D80consDouble PROC
    fld     qword ptr[rdx]
    fstp    tbyte ptr[rcx]
    ret
D80consDouble ENDP

; ---------------------- Conversion functions from Double80 -> long,ulong,longlong ulonglong,float,double

;void D80ToLong(long &dst, const Double80 &src);
D80ToLong PROC
    fld     tbyte ptr[rdx]
    pushRoundMode TRUNCATE
    fistp   dword ptr[rcx]
    popRoundMode
    ret
D80ToLong ENDP

;void D80ToULong(ULONG &dst, const Double80 &src);
D80ToULong PROC
    fld     tbyte ptr[rdx]
    fld     DmaxI32
    fcomip  st, st(1)
    jb      AboveMaxInt32

    fld     DNegmaxI32                         ; x <= maxInt32
    fcomip  st, st(1)
    ja      BeloveNegMaxInt32

    pushRoundMode TRUNCATE                     ; -maxInt32 <= x <= maxInt32
    fistp   dword ptr[rcx]
    popRoundMode
    ret

  AboveMaxInt32:                               ; x > maxInt32
    fld     DmaxI32P1
    fsub
    pushRoundMode TRUNCATE
    fistp   dword ptr[rcx]
    popRoundMode
    mov     eax, dword ptr[rcx]
    add     eax, DWmaxI32P1
    mov     dword ptr[rcx], eax
    ret

  BeloveNegMaxInt32:                           ; x < -maxInt32
    fchs
    fsub    DmaxI32P1
    pushRoundMode TRUNCATE
    fistp   dword ptr[rcx]
    popRoundMode
    mov     eax, dword ptr[rcx]
    neg     eax
    sub     eax, DWmaxI32P1
    mov     dword ptr[rcx], eax
    ret
D80ToULong ENDP

;void D80ToLongLong(INT64 &dst, const Double80 &src);
D80ToLongLong PROC
    fld     tbyte ptr[rdx]
    pushRoundMode TRUNCATE
    fistp   qword ptr[rcx]
    popRoundMode
    ret
D80ToLongLong ENDP

;void D80ToULongLong(UINT64 &dst, const Double80 &src);
D80ToULongLong PROC
    fld     tbyte ptr[rdx]
    fld     TBmaxI64
    fcomip  st, st(1)
    jb      AboveMaxInt64

    pushRoundMode TRUNCATE                     ; -maxInt64 <= x <= maxInt64
    fistp   qword ptr[rcx]
    popRoundMode
    ret

  AboveMaxInt64:
    fld     TBmaxI64P1                         ; x > maxInt64
    fsub
    pushRoundMode TRUNCATE
    fistp   qword ptr[rcx]
    popRoundMode
    mov     rax, qword ptr[rcx]
    cmp     rax, QWmaxI64
    ja      Return     
    add     rax, QWmaxI64P1
    mov     qword ptr[rcx], rax
Return:
    ret
D80ToULongLong ENDP

;void D80ToFloat(float &dst, const Double80 &src);
D80ToFloat PROC
    fld     tbyte ptr[rdx]
    fstp    dword ptr[rcx]
    ret
D80ToFloat ENDP

;void D80ToDouble(double &dst, const Double80 &src);
D80ToDouble PROC
    fld     tbyte ptr[rdx]
    fstp    qword ptr[rcx]
    ret
D80ToDouble ENDP

; ---------------------- Binary operators +,-,*,- (and unary minus) and compare -----------------
; ---------------------- assign operators +=,-=,*=,/=,++,-- -------------------------------------

;void D80add(Double80 &dst, const Double80 &x);
D80add PROC
    fld     tbyte ptr[rcx]                     ; load dst
    fld     tbyte ptr[rdx]                     ; load x
    fadd
    fstp    tbyte ptr[rcx]                     ; pop to dst
    ret
D80add ENDP

;void D80sub(Double80 &dst, const Double80 &x);
D80sub PROC
    fld     tbyte ptr[rcx]                     ; load dst
    fld     tbyte ptr[rdx]                     ; load x
    fsub
    fstp    tbyte ptr[rcx]                     ; pop to dst
    ret
D80sub ENDP

;void D80mul(Double80 &dst, const Double80 &x);
D80mul PROC
    fld     tbyte ptr[rcx]                     ; load dst
    fld     tbyte ptr[rdx]                     ; load x
    fmul
    fstp    tbyte ptr[rcx]                     ; pop to dst
    ret
D80mul ENDP

;void D80div(Double80 &dst, const Double80 &x);
D80div PROC
    fld     tbyte ptr[rcx]                     ; load dst
    fld     tbyte ptr[rdx]                     ; load x
    fdiv
    fstp    tbyte ptr[rcx]                     ; pop to dst
    ret
D80div ENDP

;void D80inc(Double80 &x);
D80inc PROC
    fld     tbyte ptr[rcx]                     ; load x
    fld1
    fadd
    fstp    tbyte ptr[rcx]                     ; pop to x
    ret
D80inc ENDP

;void D80dec(Double80 &x);
D80dec PROC
    fld     tbyte ptr[rcx]                     ; load x
    fld1
    fsub
    fstp    tbyte ptr[rcx]                     ; pop to x
    ret
D80dec ENDP

;void D80neg(Double80 &x);
D80neg PROC
    fld     tbyte ptr[rcx]                     ; load x
    fchs
    fstp    tbyte ptr[rcx]                     ; pop to x
    ret
D80neg ENDP

;int D80cmp(const Double80 &x, const Double80 &y);
D80cmp PROC
    fld     tbyte ptr[rdx]                     ; load y
    fld     tbyte ptr[rcx]                     ; load x
    fcomip  st, st(1)                          ; st(0)=x, st(1)=y
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
D80cmp ENDP

;int D80isZero(const Double80 &x);
D80isZero PROC
    fld     tbyte ptr[rcx]
    ftst
    fnstsw  ax
    sahf
    fstp    st(0)
    je      IsZero                             ; if st(0) == st(1) (this != zero) goto IsZero
    xor     rax, rax                           ; rax = 0 (false)
    ret
IsZero:
    mov     rax, 1                             ; rax = 1 (true)
    ret
D80isZero ENDP

; ------------------------------------------Misc functions ---------------------------------------------------------
;void D80getPi(Double80 &dst);
D80getPi PROC
    fldpi
    fstp    tbyte ptr[rcx]
    ret
D80getPi ENDP

;void D80getEps(Double80 &dst);
D80getEps PROC
    fld     TBEpsilon
    fstp    tbyte ptr[rcx]
    ret
D80getEps ENDP

;void D80getMin(Double80 &dst);
D80getMin PROC
    fld     TBMinimum
    fstp    tbyte ptr[rcx]
    ret
D80getMin ENDP

;void D80getMax(Double80 &dst);
D80getMax PROC
    fld     TBMaximum
    fstp    tbyte ptr[rcx]
    ret
D80getMax ENDP

; -------------------------------------------------- Double80 Functions ----------------------------------------

;void D80getExpo2(int &dst, const Double80 &x);
D80getExpo2 PROC
    fld     tbyte ptr[rdx]
    fxtract
    fstp    st(0)
    fistp   dword ptr[rcx]
    ret
D80getExpo2 ENDP

;void D80getExpo10(int &dst, const Double80 &x);
D80getExpo10 PROC
    fld     tbyte ptr[rdx]
    fldz
    fcomip  st, st(1)                          ; compare x and pop 0
    jne     x_not_zero                         ; if(x != 0) goto x_not_zero
    fstp    st(0)                              ; pop x
    mov     dword ptr[rcx], 0                  ; x == 0 => result = 0
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
    fistp   dword ptr[rcx]
    ret
D80getExpo10 ENDP

;void D80fabs(Double80 &x);
D80fabs PROC
    fld     tbyte ptr[rcx]
    fabs
    fstp    tbyte ptr[rcx]
    ret
D80fabs ENDP

;void D80sqr(Double80 &x);
D80sqr PROC
    fld     tbyte ptr[rcx]
    fld     st(0)
    fmul
    fstp    tbyte ptr[rcx]
    ret
D80sqr ENDP

;void D80sqrt(Double80 &x);
D80sqrt PROC
    fld     tbyte ptr[rcx]
    fsqrt
    fstp    tbyte ptr[rcx]
    ret
D80sqrt ENDP

;void D80rem(Double80 &dst, const Double80 &y);
D80rem PROC
    fld     tbyte ptr[rdx]                     ;                                              st0=y
    fabs                                       ; y = abs(y)                                   st0=|y|
    fld     tbyte ptr[rcx]                     ;                                              st0=dst,st1=|y|
    fldz                                       ;                                              st0=0  ,st1=dst,st2=|y|
    fcomip  st, st(1)                          ; compare and pop zero                         st0=dst,st1=|y|
    ja      RepeatNegativeX                    ; if(st(0) > st(1)) // (0 > x)
                                               ;   goto RepeatNegativeX
RepeatPositiveX:                               ; do {                                         st0=dst,st1=|y|, dst > 0
    fprem                                      ;   st0 %= y
    fstsw   ax
    sahf
    jpe     RepeatPositiveX                    ; } while(statusword.c2 != 0);
    fldz                                       ;                                              st0=0  ,st1=x,st2=|y|
    fcomip  st, st(1)                          ; compare and pop zero
    jbe     pop2                               ; if(st(0) <= st(1)) (0 <= remainder) goto pop2
    fadd                                       ; remainder += y
    fstp    tbyte ptr[rcx]                     ; pop dst
    ret                                        ; return

RepeatNegativeX:                               ; do {                                          st0=x  ,st=|y|, x < 0
    fprem                                      ;   st0 %= y
    fstsw   ax
    sahf
    jpe     RepeatNegativeX                    ; } while(statusword.c2 != 0)
    fldz
    fcomip  st, st(1)                          ; compare and pop zero
    jae     pop2                               ; if(st(0) >= st(1)) (0 >= remainder) goto pop2
    fsubr                                      ; remainder -= y
    fstp    tbyte ptr[rcx]                     ; pop dst
    ret                                        ; return

pop2:                                          ;                                               st0=x%y,st1=y
    fstp    tbyte ptr[rcx]                     ; pop dst
    fstp    st(0)                              ; pop y
    ret
D80rem ENDP

; ----------------------------------------- Double80 trigonometric functions ----------------------------------------
;void D80sin(Double80 &x);
D80sin PROC
    lea     rdx, TB2Pi2Pow60
    call    D80rem
    fld     tbyte ptr[rcx]
    fsin
    fstp    tbyte ptr[rcx]
    ret
D80sin ENDP

;void D80cos(Double80 &x);
D80cos PROC
    lea     rdx, TB2Pi2Pow60
    call    D80rem
    fld     tbyte ptr[rcx]
    fcos
    fstp    tbyte ptr[rcx]
    ret
D80cos ENDP

;void D80tan(Double80 &x);
D80tan PROC
    lea     rdx, TB2Pi2Pow60
    call    D80rem
    fld     tbyte ptr[rcx]
    fptan
    fstp    st
    fstp    tbyte ptr[rcx]
    ret
D80tan ENDP

;void D80atan(Double80 &x);
D80atan PROC
    fld     tbyte ptr[rcx]
    fld1
    fpatan
    fstp    tbyte ptr[rcx]
    ret
D80atan ENDP

;void D80atan2(Double80 &y, const Double80 &x);
D80atan2 PROC
    fld     tbyte ptr[rcx]
    fld     tbyte ptr[rdx]
    fpatan
    fstp    tbyte ptr[rcx]
    ret
D80atan2 ENDP

; inout c, out s
;void D80sincos(Double80 &c, Double80 &s);
D80sincos PROC
    mov     r8, rdx
    lea     rdx, TB2Pi2Pow60
    call    D80rem
    fld     tbyte ptr[rcx]
    fsincos
    fstp    tbyte ptr[rcx]
    fstp    tbyte ptr[r8]
    ret
D80sincos ENDP

; --------------------------------------------------- Double80 Exponential and Logarithmic functions ------------------

;void D80exp(Double80 &x);
D80exp PROC
    pushRoundMode ROUNDDOWN
    fld     tbyte ptr[rcx]
    fldl2e
    fmul
    fld     st(0)
    frndint
    fsub    st(1), st(0)
    fxch    st(1)
    f2xm1
    fld1
    fadd
    fscale
    fstp    st(1)
    fstp    tbyte ptr[rcx]
    popRoundMode
    ret
D80exp ENDP

;void D80log(Double80 &x);
D80log PROC
    fld1
    fld     tbyte ptr[rcx]
    fyl2x
    fldln2
    fmul
    fstp    tbyte ptr[rcx]
    ret
D80log ENDP

;void D80log10(Double80 &x);
D80log10 PROC
    fld1
    fld     tbyte ptr[rcx]
    fyl2x
    fldlg2
    fmul
    fstp    tbyte ptr[rcx]
    ret
D80log10 ENDP

;void D80log2(Double80 &x);
D80log2 PROC
    fld1
    fld     tbyte ptr[rcx]
    fyl2x
    fstp    tbyte ptr[rcx]
    ret
D80log2 ENDP

;void D80pow(Double80 &x, const Double80 &y);
D80pow PROC
    fld     tbyte ptr[rdx]
    fldz
    fcomip  st, st(1)
    je ZeroExponent                            ; if(y == 0) goto ZeroExponent;

    fld     tbyte ptr[rcx]
    fldz
    fcomip  st, st(1)
    je      ZeroBase                           ; if(x == 0) goto ZeroExponent;
                                               ; st(0)=x, st(1)=y
    pushRoundMode ROUNDDOWN
    fyl2x
    fld     st(0)
    frndint
    fsub    st(1), st(0)
    fxch    st(1)
    f2xm1
    fld1
    fadd
    fscale
    fstp    st(1)
    fstp    tbyte ptr[rcx]
    popRoundMode
    ret
ZeroExponent:                                  ; st(0)=y
    fstp    st(0)                              ; pop y
    fld1
    fstp    tbyte ptr[rcx]
    ret
ZeroBase:                                      ; st(0)=x, st(1)=y. x = 0. so st(0) = 0
    fcomip  st, st(1)
    ja      ZeroBaseNegativeExponent    
    fstp    st(0)                              ; pop y
    fldz
    fstp    tbyte ptr[rcx]                     ; return 0
    ret
ZeroBaseNegativeExponent:                      ; st(0)=y
    fstp    st(0)
    fld1
    fldz
    fdiv
    fstp    tbyte ptr[rcx]                     ; return 1/0 - error
    ret
D80pow ENDP

;void D80pow10(Double80 &x);
D80pow10 PROC
    fld     tbyte ptr[rcx]
    fldz
    fcomip  st, st(1)
    je      ZeroExponent

    pushRoundMode ROUNDDOWN
    fldl2t
    fmul
    fld     st(0)
    frndint
    fsub    st(1), st(0)
    fxch    st(1)
    f2xm1
    fld1
    fadd
    fscale
    fstp    st(1)
    fstp    tbyte ptr[rcx]
    popRoundMode
    ret

ZeroExponent:                                  ; st(0)=x
    fstp    st(0)                              ; pop x
    fld1
    fstp    tbyte ptr[rcx]                     ; return 1
    ret
D80pow10 ENDP

;void D80pow2(Double80 &x);
D80pow2 PROC
    fld     tbyte ptr[rcx]
    fldz
    fcomip  st, st(1)
    je      ZeroExponent

    fld     st(0)
    pushRoundMode ROUNDDOWN
    frndint
    fsub    st(1), st(0)
    fxch    st(1)
    f2xm1
    fld1
    fadd
    fscale
    fstp    st(1)
    fstp    tbyte ptr[rcx]
    popRoundMode
    ret

ZeroExponent:                                  ; st(0)=x
    fstp    st(0)                              ; pop x
    fld1
    fstp    tbyte ptr[rcx]
    ret
D80pow2 ENDP

; ------------------------------------------------- Double80 floor,ceil --------------------------------

;void D80floor(Double80 &x);
D80floor PROC
    fld     tbyte ptr[rcx]
    pushRoundMode ROUNDDOWN
    frndint
    fstp    tbyte ptr[rcx]
    popRoundMode
    ret
D80floor ENDP

;void D80ceil(Double80 &x);
D80ceil PROC
    fld     tbyte ptr[rcx]
    pushRoundMode ROUNDUP
    frndint
    fstp    tbyte ptr[rcx]
    popRoundMode
    ret
D80ceil ENDP

; ------------------------------------------------ Double80 String functions -----------------------------------------

;void D80ToBCD(BYTE bcd[10], const TenByteClass &src);
D80ToBCD PROC
    fld     tbyte ptr[rdx]
    fbstp   tbyte ptr[rcx]
    ret
D80ToBCD ENDP

;void D80ToBCDAutoScale(BYTE bcd[10], const Double80 &x, int &expo10);
D80ToBCDAutoScale PROC
    pushRoundMode ROUND                        ;
    mov     eax, dword ptr[r8]                 ;
    cmp     eax, 0                             ;
    jne     ScaleX                             ;
                                               ;
    fld     tbyte ptr[rdx]                     ;
    jmp     Rescale                            ;
                                               ;
ScaleX:                                        ; Find m = x / 10^abs(expo10)
    fild    dword ptr[r8]                      ;                                       st0=expo10
    fldl2t                                     ;                                       st0=log2(10)       , st1=expo10
    fmul                                       ;                                       st0=expo10*log2(10)
    fld     st(0)                              ;                                       st0=expo10*log2(10), st1=st0
    pushRoundMode ROUNDDOWN                    ;
    frndint                                    ; Round down
    popRoundMode                               ; Restore control word
    fsub    st(1), st(0)                       ;
    fxch    st(1)                              ;
    f2xm1                                      ;
    fld1                                       ;
    fadd                                       ;
    fscale                                     ;
    fstp    st(1)                              ;                                       st0=10^expo10
                                               ;
    fld     tbyte ptr[rdx]                     ;                                       st0=x          , st1=10^expo10
    fdivr                                      ;                                       st0=x/10^expo10
                                               ;
Rescale:                                       ;                                       st0=m
    fld     TB1e18                             ;                                       st0=1e18       , st1=m
    fmul                                       ; m *= 1e18                             st0=m
    mov     eax, dword ptr[r8]                 ;                                       eax=expo10
    fld     TB1e18M1                           ;                                       st0=1e18-1     , st1=m
WhileLoop:                                     ; while(|m| >= 1e18-1) {                st0=1e18-1     , st1=m
    fld     st(1)                              ;                                       st0=m          , st1=1e18-1     , st2=m
    fabs                                       ;                                       st0=|m|        , st1=1e18-1     , st2=m
    fcomip  st, st(1)                          ;   compare |m| and 1e18-1 and pop |m|  st0=1e18-1     , st1=m
    jb      ExitLoop                           ;   if(|m| < 1e18-1) break;             st0=1e18-1     , st1=m
    fld     TB10                               ;                                       st0=10         , st1=1e18-1     , st2=m
    fdivp   st(2), st(0)                       ;   m /= 10 and pop st0                 st0=1e18-1     , st1=m
    inc     eax                                ;   expo10++
    jmp     WhileLoop                          ; }
ExitLoop:                                      ;
    fstp    st(0)                              ; Pop st(0)                             st0=m
    fbstp   tbyte ptr[rcx]                     ; Pop m into bcd                        Assertion: 1 <= |st0| < 1e18-1 and x = st0 * 10^(eax-18)
    mov     dword ptr[r8], eax                 ; Restore expo10
    popRoundMode                               ; Restore control word
    ret
D80ToBCDAutoScale ENDP

END
