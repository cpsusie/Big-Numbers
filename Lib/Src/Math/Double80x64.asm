; To be used in x64 mode when using class Double80
;
.xlist
include fpu.inc
.list

.DATA

WmaxI16P1       WORD                   8000h
DWmaxI32P1      DWORD              80000000h
QWmaxI64        QWORD      7fffffffffffffffh
QWmaxI64P1      QWORD      8000000000000000h
DmaxI16P1       QWORD      40e0000000000000h   ;  maxI16P1 as double
DmaxI32P1       QWORD      41e0000000000000h   ;  maxI32P1 as double
TBmaxI64P1      TBYTE  403e8000000000000000h   ;  maxI64P1 as Double80
TBEpsilon       TBYTE  0000000000000080c03fh   ; 1.08420217248550443e-019
TBMinimum       TBYTE  00000000000000800100h   ; 3.36210314311209209e-4932
TBMaximum       TBYTE 0fffffffffffffffffe7fh   ; 1.18973149535723227e+4932
TB2Pi2Pow60     TBYTE  403dc90fdaa22168c235h   ; 2*pi*pow2(60) (=7.244019458077122e+018)
TB1e18          TBYTE  403ade0b6b3a76400000h   ; 1e18
TB1e18M1        TBYTE  403ade0b6b3a763ffff0h   ; TB1e18 - 1
TB10            TBYTE  4002a000000000000000h   ; 10

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

; ----------------- Conversion functions from short,USHORT,int,UINT,INT64 UINT64,float,double -> Double80 

;void D80FromI16(Double80 &s, const short &x);
D80FromI16 PROC
    fild    WORD PTR[rdx]
    fstp    TBYTE PTR[rcx]
    ret
D80FromI16 ENDP
;void D80FromUI16(Double80 &s, USHORT x);
D80FromUI16 PROC
    fildUI16 rdx
    fstp     TBYTE PTR[rcx]
    ret
D80FromUI16 ENDP
;void D80FromI32(Double80 &dst, const int &x);
D80FromI32 PROC
    fild    DWORD PTR[rdx]
    fstp    TBYTE PTR[rcx]
    ret
D80FromI32 ENDP
;void D80FromUI32(Double80 &dst, UINT x);
D80FromUI32 PROC
    fildUI32 rdx
    fstp     TBYTE PTR[rcx]
    ret
D80FromUI32 ENDP
;void D80FromI64(Double80 &dst, const INT64 &x);
D80FromI64 PROC
    fild    QWORD PTR[rdx]
    fstp    TBYTE PTR[rcx]
    ret
D80FromI64 ENDP
;void D80FromUI64(Double80 &dst, const UINT64 x);
D80FromUI64 PROC
    fildUI64 rdx
    fstp     TBYTE PTR[rcx]
    ret
D80FromUI64 ENDP
;void D80FromFlt(Double80 &dst, float &x);
D80FromFlt PROC
    fld     DWORD PTR[rdx]
    fstp    TBYTE PTR[rcx]
    ret
D80FromFlt ENDP
;void D80FromDbl(Double80 &dst, const double &x);
D80FromDbl PROC
    fld     QWORD PTR[rdx]
    fstp    TBYTE PTR[rcx]
    ret
D80FromDbl ENDP

; ---------------------- Conversion functions from Double80 -> int,UINT,INT64 UINT64,float,double

;int D80ToI32(const Double80 &x);
D80ToI32 PROC
    fld     TBYTE PTR[rcx]                     ; load x
    fisttp  DWORD PTR[rsp-4]
    mov     eax, DWORD PTR[rsp-4]
    ret
D80ToI32 ENDP
;UINT D80ToUI32(const Double80 &x);
D80ToUI32 PROC
    fld     TBYTE PTR[rcx]                    ; load x
    fld     DmaxI32P1
    fcomi   st, st(1)
    jbe     AboveMaxInt32

    fchs                                       ; x <= maxInt32
    fcomi   st, st(1)
    jae     BeloveNegMaxInt32

    fstp    st(0)
                                                ; -maxInt32 <= x <= maxInt32
    fisttp  DWORD PTR[rsp-4]
    mov     eax, DWORD PTR[rsp-4]
    ret

  AboveMaxInt32:                               ; x > maxInt32
    fsub
    fisttp  DWORD PTR[rsp-4]
    mov     eax, DWORD PTR[rsp-4]
    add     eax, DWmaxI32P1
    ret

  BeloveNegMaxInt32:                           ; x < -maxInt32
    fsub
    fisttp  DWORD PTR[rsp-4]
    mov     eax, DWORD PTR[rsp-4]
    sub     eax, DWmaxI32P1
    ret
D80ToUI32 ENDP
;INT64 D80ToI64(const Double80 &x);
D80ToI64 PROC
    fld     TBYTE PTR[rcx]
    fisttp  QWORD PTR[rsp-8]
    mov     rax,  QWORD PTR[rsp-8]
    ret
D80ToI64 ENDP
;UINT64 D80ToUI64(const Double80 &x);
D80ToUI64 PROC
    fld     TBYTE PTR[rcx]
    fld     TBmaxI64P1
    fcomi   st, st(1)
    jbe     AboveMaxInt64

    fstp    st(0)
                                               ; -maxInt64 <= x <= maxInt64
    fisttp  QWORD PTR[rsp-8]
    mov     rax,  QWORD PTR[rsp-8]
    ret

  AboveMaxInt64:                                ; x > maxInt64
    fsub
    fisttp  QWORD PTR[rsp-8]
    mov     rax,  QWORD PTR[rsp-8]
    cmp     rax, QWmaxI64P1
    jae     Return
    add     rax, QWmaxI64P1
Return:
    ret
D80ToUI64 ENDP
;float D80ToFlt(const Double80 &src);
D80ToFlt PROC
    fld     TBYTE PTR[rcx]
    fstp    DWORD PTR[rsp-4]
    movss   xmm0, DWORD PTR[rsp-4]
    ret
D80ToFlt ENDP
;double D80ToDbl(const Double80 &x);
D80ToDbl PROC
    fld     TBYTE PTR[rcx]
    fstp    QWORD PTR[rsp-8]
    movsd   xmm0, QWORD PTR[rsp-8]
    ret
D80ToDbl ENDP

; ---------------------- Binary operators +,-,*,- (and unary minus) -----------------
; ---------------------- assign operators ++,--,-------------------------------------

;void D80addI16(Double80 &dst, const short &x);
D80addI16 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fiadd   WORD PTR[rdx]                      ; add  x
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80addI16 ENDP
;void D80subI16(Double80 &dst, const short &x);
D80subI16 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fisub   WORD PTR[rdx]                      ; sub  x
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80subI16 ENDP
;void D80subrI16(Double80 &dst, const short &x);
D80subrI16 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fisubr  WORD PTR[rdx]                      ; st0 = x - st0
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80subrI16 ENDP
;void D80mulI16(Double80 &dst, const short &x);
D80mulI16 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fimul   WORD PTR[rdx]                      ; mul  x
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80mulI16 ENDP
;void D80divI16(Double80 &dst, const short &x);
D80divI16 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fidiv   WORD PTR[rdx]                      ; div  x
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80divI16 ENDP
;void D80divrI16(Double80 &dst, const short &x);
D80divrI16 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fidivr  WORD PTR[rdx]                      ; st0 = x / st0
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80divrI16 ENDP
;void D80addUI16(Double80 &dst, USHORT x);
D80addUI16 PROC
    fld      TBYTE PTR[rcx]                    ; load dst
    fildUI16 rdx                               ; load x
    fadd
    fstp     TBYTE PTR[rcx]                    ; pop to dst
    ret
D80addUI16 ENDP
;void D80subUI16(Double80 &dst, USHORT x);
D80subUI16 PROC
    fld      TBYTE PTR[rcx]                    ; load dst
    fildUI16 rdx                               ; load x
    fsub
    fstp     TBYTE PTR[rcx]                    ; pop to dst
    ret
D80subUI16 ENDP
;void D80subrUI16(Double80 &dst, USHORT x);
D80subrUI16 PROC
    fildUI16 rdx                               ; load x
    fld      TBYTE PTR[rcx]                    ; load dst
    fsub
    fstp     TBYTE PTR[rcx]                    ; pop to dst
    ret
D80subrUI16 ENDP
;void D80mulUI16(Double80 &dst, USHORT x);
D80mulUI16 PROC
    fld      TBYTE PTR[rcx]                    ; load dst
    fildUI16 rdx                               ; load x
    fmul
    fstp     TBYTE PTR[rcx]                    ; pop to dst
    ret
D80mulUI16 ENDP
;void D80divUI16(Double80 &dst, USHORT x);
D80divUI16 PROC
    fld      TBYTE PTR[rcx]                    ; load dst
    fildUI16 rdx                               ; load x
    fdiv
    fstp     TBYTE PTR[rcx]                    ; pop to dst
    ret
D80divUI16 ENDP
;void D80divrUI16(Double80 &dst, USHORT x);
D80divrUI16 PROC
    fildUI16 rdx                               ; load x
    fld      TBYTE PTR[rcx]                    ; load dst
    fdiv
    fstp     TBYTE PTR[rcx]                    ; pop to dst
    ret
D80divrUI16 ENDP
;void D80addI32(Double80 &dst, const int &x);
D80addI32 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fiadd   DWORD PTR[rdx]                     ; add  x
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80addI32 ENDP
;void D80subI32(Double80 &dst, const int &x);
D80subI32 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fisub   DWORD PTR[rdx]                     ; sub  x
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80subI32 ENDP
;void D80subrI32(Double80 &dst, const int &x);
D80subrI32 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fisubr  DWORD PTR[rdx]                     ; st0 = x - st0
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80subrI32 ENDP
;void D80mulI32(Double80 &dst, const int &x);
D80mulI32 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fimul   DWORD PTR[rdx]                     ; mul  x
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80mulI32 ENDP
;void D80divI32(Double80 &dst, const int &x);
D80divI32 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fidiv   DWORD PTR[rdx]                     ; div  x
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80divI32 ENDP
;void D80divrI32(Double80 &dst, const int &x);
D80divrI32 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fidivr  DWORD PTR[rdx]                     ; st0 = x / st0
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80divrI32 ENDP
;void D80addUI32(Double80 &dst, UINT x);
D80addUI32 PROC
    fld      TBYTE PTR[rcx]                    ; load dst
    fildUI32 rdx                               ; load x
    fadd
    fstp     TBYTE PTR[rcx]                    ; pop to dst
    ret
D80addUI32 ENDP
;void D80subUI32(Double80 &dst, UINT x);
D80subUI32 PROC
    fld      TBYTE PTR[rcx]                    ; load dst
    fildUI32 rdx                               ; load x
    fsub
    fstp     TBYTE PTR[rcx]                    ; pop to dst
    ret
D80subUI32 ENDP
;void D80subrUI32(Double80 &dst, UINT x);
D80subrUI32 PROC
    fildUI32 rdx                               ; load x
    fld      TBYTE PTR[rcx]                    ; load dst
    fsub
    fstp     TBYTE PTR[rcx]                    ; pop to dst
    ret
D80subrUI32 ENDP
;void D80mulUI32(Double80 &dst, UINT x);
D80mulUI32 PROC
    fld      TBYTE PTR[rcx]                    ; load dst
    fildUI32 rdx                               ; load x
    fmul
    fstp     TBYTE PTR[rcx]                    ; pop to dst
    ret
D80mulUI32 ENDP
;void D80divUI32(Double80 &dst, UINT x);
D80divUI32 PROC
    fld      TBYTE PTR[rcx]                    ; load dst
    fildUI32 rdx                               ; load x
    fdiv
    fstp     TBYTE PTR[rcx]                    ; pop to dst
    ret
D80divUI32 ENDP
;void D80divrUI32(Double80 &dst, UINT x);
D80divrUI32 PROC
    fildUI32 rdx                               ; load x
    fld      TBYTE PTR[rcx]                    ; load dst
    fdiv
    fstp     TBYTE PTR[rcx]                    ; pop to dst
    ret
D80divrUI32 ENDP
;void D80addI64(Double80 &dst, const INT64 &x);
D80addI64 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fild    QWORD PTR[rdx]                     ; load x
    fadd
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80addI64 ENDP
;void D80subI64(Double80 &dst, const INT64 &x);
D80subI64 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fild    QWORD PTR[rdx]                     ; load x
    fsub
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80subI64 ENDP
;void D80subrI64(Double80 &dst, const INT64 &x);
D80subrI64 PROC
    fild    QWORD PTR[rdx]                     ; load x
    fld     TBYTE PTR[rcx]                     ; load dst
    fsub
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80subrI64 ENDP
;void D80mulI64(Double80 &dst, const INT64 &x);
D80mulI64 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fild    QWORD PTR[rdx]                     ; load x
    fmul
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80mulI64 ENDP
;void D80divI64(Double80 &dst, const INT64 &x);
D80divI64 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fild    QWORD PTR[rdx]                     ; load x
    fdiv
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80divI64 ENDP
;void D80divrI64(Double80 &dst, const INT64 &x);
D80divrI64 PROC
    fild    QWORD PTR[rdx]                     ; load x
    fld     TBYTE PTR[rcx]                     ; load dst
    fdiv
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80divrI64 ENDP
;void D80addUI64(Double80 &dst, UINT64 x);
D80addUI64 PROC
    fld      TBYTE PTR[rcx]                    ; load dst
    fildUI64 rdx                               ; load x
    fadd
    fstp     TBYTE PTR[rcx]                    ; pop to dst
    ret
D80addUI64 ENDP
;void D80subUI64(Double80 &dst, UINT64 x);
D80subUI64 PROC
    fld      TBYTE PTR[rcx]                    ; load dst
    fildUI64 rdx                               ; load x
    fsub
    fstp     TBYTE PTR[rcx]                    ; pop to dst
    ret
D80subUI64 ENDP
;void D80subrUI64(Double80 &dst, UINT64 x);
D80subrUI64 PROC
    fildUI64 rdx                               ; load x
    fld      TBYTE PTR[rcx]                    ; load dst
    fsub
    fstp     TBYTE PTR[rcx]                    ; pop to dst
    ret
D80subrUI64 ENDP
;void D80mulUI64(Double80 &dst, UINT64 x);
D80mulUI64 PROC
    fld      TBYTE PTR[rcx]                    ; load dst
    fildUI64 rdx                               ; load x
    fmul
    fstp     TBYTE PTR[rcx]                    ; pop to dst
    ret
D80mulUI64 ENDP
;void D80divUI64(Double80 &dst, UINT64 x);
D80divUI64 PROC
    fld      TBYTE PTR[rcx]                    ; load dst
    fildUI64 rdx                               ; load x
    fdiv
    fstp     TBYTE PTR[rcx]                    ; pop to dst
    ret
D80divUI64 ENDP
;void D80divrUI64(Double80 &dst, UINT64 x);
D80divrUI64 PROC
    fildUI64 rdx                               ; load x
    fld      TBYTE PTR[rcx]                    ; load dst
    fdiv
    fstp     TBYTE PTR[rcx]                    ; pop to dst
    ret
D80divrUI64 ENDP
;void D80addFlt(Double80 &dst, const float &x);
D80addFlt PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fadd    DWORD PTR[rdx]                     ; add  x
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80addFlt ENDP
;void D80subFlt(Double80 &dst, const float &x);
D80subFlt PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fsub    DWORD PTR[rdx]                     ; sub  x
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80subFlt ENDP
;void D80subrFlt(Double80 &dst, const float &x);
D80subrFlt PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fsubr   DWORD PTR[rdx]                     ; st0 = x - st0
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80subrFlt ENDP
;void D80mulFlt(Double80 &dst, const float &x);
D80mulFlt PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fmul    DWORD PTR[rdx]                     ; mul  x
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80mulFlt ENDP
;void D80divFlt(Double80 &dst, const float &x);
D80divFlt PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fdiv    DWORD PTR[rdx]                     ; div  x
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80divFlt ENDP
;void D80divrFlt(Double80 &dst, const float &x);
D80divrFlt PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fdivr   DWORD PTR[rdx]                     ; st0 = x / st0
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80divrFlt ENDP
;void D80addDbl(Double80 &dst, const double &x);
D80addDbl PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fadd    QWORD PTR[rdx]                     ; add  x
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80addDbl ENDP
;void D80subDbl(Double80 &dst, const double &x);
D80subDbl PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fsub    QWORD PTR[rdx]                     ; sub  x
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80subDbl ENDP
;void D80subrDbl(Double80 &dst, const double &x);
D80subrDbl PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fsubr   QWORD PTR[rdx]                     ; st0 = x - st0
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80subrDbl ENDP
;void D80mulDbl(Double80 &dst, const double &x);
D80mulDbl PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fmul    QWORD PTR[rdx]                     ; mul  x
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80mulDbl ENDP
;void D80divDbl(Double80 &dst, const double &x);
D80divDbl PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fdiv    QWORD PTR[rdx]                     ; div  x
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80divDbl ENDP
;void D80divrDbl(Double80 &dst, const double &x);
D80divrDbl PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fdivr   QWORD PTR[rdx]                     ; st0 = x / st0
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80divrDbl ENDP
;void D80addD80(Double80 &dst, const Double80 &x);
D80addD80 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fld     TBYTE PTR[rdx]                     ; load x
    fadd
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80addD80 ENDP
;void D80subD80(Double80 &dst, const Double80 &x);
D80subD80 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fld     TBYTE PTR[rdx]                     ; load x
    fsub
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80subD80 ENDP
;void D80mulD80(Double80 &dst, const Double80 &x);
D80mulD80 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fld     TBYTE PTR[rdx]                     ; load x
    fmul
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80mulD80 ENDP
;void D80divD80(Double80 &dst, const Double80 &x);
D80divD80 PROC
    fld     TBYTE PTR[rcx]                     ; load dst
    fld     TBYTE PTR[rdx]                     ; load x
    fdiv
    fstp    TBYTE PTR[rcx]                     ; pop to dst
    ret
D80divD80 ENDP

;void D80inc(Double80 &x);
D80inc PROC
    fld     TBYTE PTR[rcx]                     ; load x
    fld1
    fadd
    fstp    TBYTE PTR[rcx]                     ; pop to x
    ret
D80inc ENDP
;void D80dec(Double80 &x);
D80dec PROC
    fld     TBYTE PTR[rcx]                     ; load x
    fld1
    fsub
    fstp    TBYTE PTR[rcx]                     ; pop to x
    ret
D80dec ENDP
;void D80neg(Double80 &x);
D80neg PROC
    fld     TBYTE PTR[rcx]                     ; load x
    fchs
    fstp    TBYTE PTR[rcx]                     ; pop to x
    ret
D80neg ENDP

; -------------------------------- Compare functions -------------------------------------------

;int D80isZero(const Double80 &x);
D80isZero PROC
    fld     TBYTE PTR[rcx]                     ; load x
    ftst
    fnstsw  ax
    sahf
    fstp    st(0)                              ; pop x
    je      IsZero                             ; if(x == 0) goto IsZero
    xor     rax, rax                           ; rax = 0 (false)
    ret
IsZero:
    mov     rax, 1                             ; rax = 1 (true)
    ret
D80isZero ENDP
;int D80cmpI16(const Double80 &x, const short &y);
D80cmpI16 PROC
    fld     TBYTE PTR[rcx]                     ; load x
    ficomp  WORD PTR[rdx]
    fnstsw  ax
    sahf
    ja      XAboveY
    jb      XBelowY
    xor     rax,rax
    ret
XAboveY:
    mov     rax, 1
    ret
XBelowY:
    mov     rax, -1
    ret
D80cmpI16 ENDP
;int D80cmpUI16(const Double80 &x, USHORT y);
D80cmpUI16 PROC
    fildUI16 rdx                               ; load y
    fld     TBYTE PTR[rcx]                     ; load x
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
D80cmpUI16 ENDP
;int D80cmpI32(const Double80 &x, const int &y);
D80cmpI32 PROC
    fld     TBYTE PTR[rcx]                     ; load x
    ficomp  DWORD PTR[rdx]
    fnstsw  ax
    sahf
    ja      XAboveY
    jb      XBelowY
    xor     rax,rax
    ret
XAboveY:
    mov     rax, 1
    ret
XBelowY:
    mov     rax, -1
    ret
D80cmpI32 ENDP
;int D80cmpUI32(const Double80 &x, UINT y);
D80cmpUI32 PROC
    fildUI32 rdx                               ; load y
    fld     TBYTE PTR[rcx]                     ; load x
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
D80cmpUI32 ENDP
;int D80cmpI64(const Double80 &x, const INT64 &y);
D80cmpI64 PROC
    fild    QWORD PTR[rdx]                     ; load y
    fld     TBYTE PTR[rcx]                     ; load x
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
D80cmpI64 ENDP
;int D80cmpUI64(const Double80 &x, UINT64 y);
D80cmpUI64 PROC
    fildUI64 rdx                               ; load y
    fld     TBYTE PTR[rcx]                     ; load x
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
D80cmpUI64 ENDP
;int D80cmpFlt(const Double80 &x, const float &y);
D80cmpFlt PROC
    fld     TBYTE PTR[rcx]                     ; load x
    fcomp   DWORD PTR[rdx]
    fnstsw  ax
    sahf
    ja      XAboveY
    jb      XBelowY
    xor     rax,rax
    ret
XAboveY:
    mov     rax, 1
    ret
XBelowY:
    mov     rax, -1
    ret
D80cmpFlt ENDP
;int D80cmpDbl(const Double80 &x, const double &y);
D80cmpDbl PROC
    fld     TBYTE PTR[rcx]                     ; load x
    fcomp   QWORD PTR[rdx]
    fnstsw  ax
    sahf
    ja      XAboveY
    jb      XBelowY
    xor     rax,rax
    ret
XAboveY:
    mov     rax, 1
    ret
XBelowY:
    mov     rax, -1
    ret
D80cmpDbl ENDP
;int D80cmpD80(const Double80 &x, const Double80 &y);
D80cmpD80 PROC
    fld     TBYTE PTR[rdx]                     ; load y
    fld     TBYTE PTR[rcx]                     ; load x
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
D80cmpD80 ENDP

; ------------------------------------------Misc functions ---------------------------------------------------------
;void D80getPi(Double80 &dst);
D80getPi PROC
    fldpi
    fstp    TBYTE PTR[rcx]
    ret
D80getPi ENDP

;void D80getEps(Double80 &dst);
D80getEps PROC
    fld     TBEpsilon
    fstp    TBYTE PTR[rcx]
    ret
D80getEps ENDP

;void D80getMin(Double80 &dst);
D80getMin PROC
    fld     TBMinimum
    fstp    TBYTE PTR[rcx]
    ret
D80getMin ENDP

;void D80getMax(Double80 &dst);
D80getMax PROC
    fld     TBMaximum
    fstp    TBYTE PTR[rcx]
    ret
D80getMax ENDP

; -------------------------------------------------- Double80 Functions ----------------------------------------

;int D80getExpo2(const Double80 &x);
D80getExpo2 PROC
    fld     TBYTE PTR[rcx]
    fxtract
    fstp    st(0)
    fistp   QWORD PTR[rsp-8]
    mov     rax, QWORD PTR[rsp-8]
    ret
D80getExpo2 ENDP

;int D80getExpo10(const Double80 &x);
D80getExpo10 PROC
    fld     TBYTE PTR[rcx]                     ; st0 = x
    fldz
    fcomip  st, st(1)                          ; compare x and pop 0
    je      xIsZero                            ; if(x == 0) goto xIsZero
    fld1                                       ; st0 = 1       , st1 = x
    fxch    st(1)                              ; st0 = x       , st1 = 1
    fabs                                       ; st0 = |x|     , st1 = 1
    fyl2x                                      ; st0 = st1*log2(st0) = log2(|x|)
    fldlg2                                     ; st0 = log10(2), st1 = log2(|x|)
    fmul                                       ; st0 = log2(|x|)*log10(2) = log10(|x|)
    pushRoundMode ROUNDDOWN
    frndint
    popRoundMode
    fistp   QWORD PTR[rsp-8]
    mov     rax, QWORD PTR[rsp-8]
    ret
xIsZero:
    fstp    st(0)                              ; pop x
    xor     rax, rax                           ; x == 0 => result = 0
    ret
D80getExpo10 ENDP

;void D80fabs(Double80 &x);
D80fabs PROC
    fld     TBYTE PTR[rcx]
    fabs
    fstp    TBYTE PTR[rcx]
    ret
D80fabs ENDP

;void D80sqr(Double80 &x);
D80sqr PROC
    fld     TBYTE PTR[rcx]
    fld     st(0)
    fmul
    fstp    TBYTE PTR[rcx]
    ret
D80sqr ENDP

;void D80sqrt(Double80 &x);
D80sqrt PROC
    fld     TBYTE PTR[rcx]
    fsqrt
    fstp    TBYTE PTR[rcx]
    ret
D80sqrt ENDP

;void D80rem(Double80 &dst, const Double80 &y);
D80rem PROC
    fld     TBYTE PTR[rdx]                     ;                                              st0=y
    fabs                                       ; y = abs(y)                                   st0=|y|
    fld     TBYTE PTR[rcx]                     ;                                              st0=dst,st1=|y|
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
    fstp    TBYTE PTR[rcx]                     ; pop dst
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
    fstp    TBYTE PTR[rcx]                     ; pop dst
    ret                                        ; return

pop2:                                          ;                                               st0=x%y,st1=y
    fstp    TBYTE PTR[rcx]                     ; pop dst
    fstp    st(0)                              ; pop y
    ret
D80rem ENDP

; ----------------------------------------- Double80 trigonometric functions ----------------------------------------
;void D80sin(Double80 &x);
D80sin PROC
    lea     rdx, TB2Pi2Pow60
    call    D80rem
    fld     TBYTE PTR[rcx]
    fsin
    fstp    TBYTE PTR[rcx]
    ret
D80sin ENDP

;void D80cos(Double80 &x);
D80cos PROC
    lea     rdx, TB2Pi2Pow60
    call    D80rem
    fld     TBYTE PTR[rcx]
    fcos
    fstp    TBYTE PTR[rcx]
    ret
D80cos ENDP

;void D80tan(Double80 &x);
D80tan PROC
    lea     rdx, TB2Pi2Pow60
    call    D80rem
    fld     TBYTE PTR[rcx]
    fptan
    fstp    st
    fstp    TBYTE PTR[rcx]
    ret
D80tan ENDP

;void D80atan(Double80 &x);
D80atan PROC
    fld     TBYTE PTR[rcx]
    fld1
    fpatan
    fstp    TBYTE PTR[rcx]
    ret
D80atan ENDP

;void D80atan2(Double80 &y, const Double80 &x);
D80atan2 PROC
    fld     TBYTE PTR[rcx]
    fld     TBYTE PTR[rdx]
    fpatan
    fstp    TBYTE PTR[rcx]
    ret
D80atan2 ENDP

; inout c, out s
;void D80sincos(Double80 &c, Double80 &s);
D80sincos PROC
    mov     r8, rdx
    lea     rdx, TB2Pi2Pow60
    call    D80rem
    fld     TBYTE PTR[rcx]
    fsincos
    fstp    TBYTE PTR[rcx]
    fstp    TBYTE PTR[r8]
    ret
D80sincos ENDP

; --------------------------------------------------- Double80 Exponential and Logarithmic functions ------------------

;void D80pow2(Double80 &x);
D80pow2 PROC
    fld     TBYTE PTR[rcx]                     ; st0 = x
    fldz
    fcomip  st, st(1)
    je      ZeroExponent

    pushRoundMode ROUNDDOWN                    ; st0 = x (!= 0)
    fld     st(0)                              ; st0 = x                    , st1 = x
    frndint                                    ; st0 = floor(  x         )  , st1 = x
    fsub    st(1), st(0)                       ; st0 = floor(  x         )  , st1 = frac(x)
    fxch    st(1)                              ; st0 = frac(   x         )  , st1 = floor(x)
    f2xm1                                      ; st0 = 2^ frac(x         )-1, st1 = floor(x)
    fld1
    fadd                                       ; st0 = 2^ frac(x         )  , st1 = floor(x)
    fscale                                     ; st0 = 2^(frac(x         )+floor(x         ))=2 ^x, st1 = floor(x)
    fstp    st(1)                              ; pop st0
    fstp    TBYTE PTR[rcx]                     ; pop to x
    popRoundMode
    ret
ZeroExponent:                                  ; st0 = x
    fstp    st(0)                              ; pop st0
    fld1                                       ; st0 = 1
    fstp    TBYTE PTR[rcx]                     ; pop to x
    ret
D80pow2 ENDP

;void D80pow10(Double80 &x);
D80pow10 PROC
    fld     TBYTE PTR[rcx]                     ; st0 = x
    fldz
    fcomip  st, st(1)
    je      ZeroExponent

    pushRoundMode ROUNDDOWN                    ; st0 = x (!= 0)
    fldl2t                                     ; st0 = log2(10)             , st1 = x
    fmul                                       ; st0 = x*log2(10)
    fld     st(0)
    frndint                                    ; st0 = floor(  x*log2(10))  , st1 = x*log2(10)
    fsub    st(1), st(0)                       ; st0 = floor(  x*log2(10))  , st1 = frac( x*log2(10))
    fxch    st(1)                              ; st0 = frac(   x*log2(10))  , st1 = floor(x*log2(10))
    f2xm1                                      ; st0 = 2^ frac(x*log2(10))-1, st1 = floor(x*log2(10))
    fld1
    fadd                                       ; st0 = 2^ frac(x*log2(10))  , st1 = floor(x*log2(10))
    fscale                                     ; st0 = 2^(frac(x*log2(10))+floor(x*log2(10)))=10^x, st1 = floor(x*log2(10))
    fstp    st(1)                              ; pop st1
    fstp    TBYTE PTR[rcx]                     ; pop to x
    popRoundMode
    ret
ZeroExponent:                                  ; st0 = x
    fstp    st(0)                              ; pop st0
    fld1                                       ; st0 = 1
    fstp    TBYTE PTR[rcx]                     ; pop to x
    ret
D80pow10 ENDP

;void D80exp(Double80 &x);
D80exp PROC
    fld     TBYTE PTR[rcx]                     ; st0 = x

    pushRoundMode ROUNDDOWN
    fldl2e                                     ; st0 = log2(e)              , st1 = x
    fmul                                       ; st0 = x/ln(2)
    fld     st(0)                              ; st0 = x/ln(2)              , st1 = x/ln(2)
    frndint                                    ; st0 = floor(  x/ln(2   ))  , st1 = x/ln(2)
    fsub    st(1), st(0)                       ; st0 = floor(  x/ln(2   ))  , st1 = frac( x/ln(2))
    fxch    st(1)                              ; st0 = frac(   x/ln(2   ))  , st1 = floor(x/ln(2))
    f2xm1                                      ; st0 = 2^ frac(x/ln(2   ))-1, st1 = floor(x/ln(2))
    fld1
    fadd                                       ; st0 = 2^ frac(x/ln(2   ))   , st1 = floor(x/ln(2))
    fscale                                     ; st0 = 2^(frac(x/ln(2   ))+floor(x/ln(2   )))=e ^x, st1 = floor(x/ln(2))
    fstp    st(1)                              ; pop st1
    fstp    TBYTE PTR[rcx]                     ; pop to x
    popRoundMode
    ret
D80exp ENDP

;void D80pow(Double80 &x, const Double80 &y);
D80pow PROC
    fld     TBYTE PTR[rdx]                     ; st0 = y
    fldz
    fcomip  st, st(1)
    je ZeroExponent                            ; if(y == 0) goto ZeroExponent;

    fld     TBYTE PTR[rcx]                     ; st0 = x                    , st1 = y
    fldz
    fcomip  st, st(1)
    je      ZeroBase                           ; if(x == 0) goto ZeroExponent;

    pushRoundMode ROUNDDOWN                    ; st0 = x (!= 0)             , st1 = y (!= 0)
    fyl2x                                      ; st0 = st1*log2(st0) = y*log2(x)
    fld     st(0)                              ;
    frndint                                    ; st0 = floor(  y*log2(x ))  , st1 = y*log2(x )
    fsub    st(1), st(0)                       ; st0 = floor(  y*log2(x ))  , st1 = frac( y*log2(x ))
    fxch    st(1)                              ; st0 = frac(   y*log2(x ))  , st1 = floor(y*log2(x ))
    f2xm1                                      ; st0 = 2^ frac(y*log2(x ))-1, st1 = floor(y*log2(x ))
    fld1
    fadd                                       ; st0 = 2^ frac(y*log2(x ))  , st1 = floor(y*log2(x ))
    fscale                                     ; st0 = 2^(frac(y*log2(x ))+floor(y*log2(x )))=x ^y, st1 = floor(y*log2(x))
    fstp    st(1)                              ; pop st1
    fstp    TBYTE PTR[rcx]                     ; pop to x
    popRoundMode
    ret
ZeroExponent:                                  ; st0 = y
    fstp    st(0)                              ; pop st0
    fld1                                       ; st0 = 1
    fstp    TBYTE PTR[rcx]                     ; pop to x
    ret
ZeroBase:                                      ; st0 = x, st1 = y. x = 0. so st0 = 0
    fcomip  st, st(1)
    ja      ZeroBaseNegativeExponent    
    fstp    st(0)                              ; pop st0
    fldz                                       ; st0 = 0
    fstp    TBYTE PTR[rcx]                     ; pop to x
    ret
ZeroBaseNegativeExponent:                      ; st0 = y
    fstp    st(0)                              ; pop st0
    fld1                                       ; st0 = 1
    fldz                                       ; st0 = 0, st1 = 1
    fdiv                                       ; st0 = 1/0
    fstp    TBYTE PTR[rcx]                     ; pop to x - error
    ret
D80pow ENDP

;void D80log2(Double80 &x);
D80log2 PROC
    fld1                                       ; st0 = 1
    fld     TBYTE PTR[rcx]                     ; st0 = x     , st1 = 1
    fyl2x                                      ; st0 = st1*log2(st0) = log2(x)
    fstp    TBYTE PTR[rcx]                     ; pop to x
    ret
D80log2 ENDP

;void D80log10(Double80 &x);
D80log10 PROC
    fld1                                       ; st0 = 1
    fld     TBYTE PTR[rcx]                     ; st0 = x       , st1 = 1
    fyl2x                                      ; st0 = st1*log2(st0) = log2(x)
    fldlg2                                     ; st0 = log10(2), st1 = log2(x)
    fmul                                       ; st0 = log2(x)*log10(2) = log10(x)
    fstp    TBYTE PTR[rcx]                     ; pop to x
    ret
D80log10 ENDP

;void D80log(Double80 &x);
D80log PROC
    fld1                                       ; st0 = 1
    fld     TBYTE PTR[rcx]                     ; st0 = x     , st1 = 1
    fyl2x                                      ; st0 = st1*log2(st0) = log2(x)
    fldln2                                     ; st0 = ln(2) , st1 = log2(x)
    fmul                                       ; st0 = ln(x)
    fstp    TBYTE PTR[rcx]                     ; pop to x
    ret
D80log ENDP

; ------------------------------------------------- Double80 floor,ceil --------------------------------

;void D80floor(Double80 &x);
D80floor PROC
    fld     TBYTE PTR[rcx]
    pushRoundMode ROUNDDOWN
    frndint
    fstp    TBYTE PTR[rcx]
    popRoundMode
    ret
D80floor ENDP

;void D80ceil(Double80 &x);
D80ceil PROC
    fld     TBYTE PTR[rcx]
    pushRoundMode ROUNDUP
    frndint
    fstp    TBYTE PTR[rcx]
    popRoundMode
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
    pushRoundMode ROUND                        ;
    mov     eax, DWORD PTR[r8]                 ;
    cmp     eax, 0                             ;
    jne     ScaleX                             ;
                                               ;
    fld     TBYTE PTR[rdx]                     ;
    jmp     Rescale                            ;
                                               ;
ScaleX:                                        ; Find m = x / 10^abs(expo10)
    fild    DWORD PTR[r8]                      ;                                       st0=expo10
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
    fld     TBYTE PTR[rdx]                     ;                                       st0=x          , st1=10^expo10
    fdivr                                      ;                                       st0=x/10^expo10
                                               ;
Rescale:                                       ;                                       st0=m
    fld     TB1e18                             ;                                       st0=1e18       , st1=m
    fmul                                       ; m *= 1e18                             st0=m
    mov     eax, DWORD PTR[r8]                 ;                                       eax=expo10
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
    fbstp   TBYTE PTR[rcx]                     ; Pop m into bcd                        Assertion: 1 <= |st0| < 1e18-1 and x = st0 * 10^(eax-18)
    mov     DWORD PTR[r8], eax                 ; Restore expo10
    popRoundMode                               ; Restore control word
    ret
D80ToBCDAutoScale ENDP

END
