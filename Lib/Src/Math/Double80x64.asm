; To be used in x64 mode when using class Double80
;
.xlist
include fpu.inc
.list

.DATA

_DWmaxI32P1     DWORD              80000000h   ; (UINT)_I32_MAX + 1
_QWmaxI64       QWORD      7fffffffffffffffh   ; (UINT64)_I64_MAX
_QWmaxI64P1     QWORD      8000000000000000h   ; (UINT64)_I64_MAX + 1
_Dmaxi16P1      QWORD      40e0000000000000h   ; (double)_I16_MAX+1
_DmaxI32P1      QWORD      41e0000000000000h   ; (double)_I32_MAX+1
_D80maxi64P1    TBYTE  403e8000000000000000h   ; (Double80)_I64_MAX+1
_D802PiExp260   TBYTE  403dc90fdaa22168c235h   ; (Double80)2pi*exp2(60) (=7.244019458077122e+018)
                                               ; st(0) must be < 2pi*2^60 for fptan,fsin,fcos,fsincos

.CODE

;---------------------------------------------- FPU control function ----------------------------------------

;void FPUinit();
FPUinit PROC
    finit
    ret
FPUinit ENDP

;WORD FPUgetStatusWord();
FPUgetStatusWord PROC
    fstsw WORD PTR[rsp-2]
    mov ax, WORD PTR[rsp-2]
    ret
FPUgetStatusWord ENDP

;WORD FPUgetControlWord();
FPUgetControlWord PROC
    fstcw WORD PTR[rsp-2]
    mov ax, WORD PTR[rsp-2]
    ret
FPUgetControlWord ENDP

;void FPUsetControlWord(WORD *cw);
FPUsetControlWord PROC
    fldcw WORD PTR[rcx]
    ret
FPUsetControlWord ENDP

;void FPUgetEnvironment(void *env);
FPUgetEnvironment PROC
    fstenv QWORD PTR[rcx]
    ret
FPUgetEnvironment ENDP

;void FPUgetState(void *state);
FPUgetState PROC
    fsave QWORD PTR[rcx]
    frstor QWORD PTR[rcx]
    ret
FPUgetState ENDP

;void FPUclearExceptions();
FPUclearExceptions PROC
    fclex
    ret
FPUclearExceptions ENDP
;void FPUclearExceptionsNoWait();
FPUclearExceptionsNoWait PROC
    fnclex
    ret
FPUclearExceptionsNoWait ENDP

; ----------------- Conversion functions from short,USHORT,int,UINT,INT64 UINT64,float,double -> Double80 

;void _D80FromI16(Double80 &dst, const short &x);
 _D80FromI16 PROC
    fild       WORD PTR[rdx]
    fstp       TBYTE PTR[rcx]
    ret
 _D80FromI16 ENDP
;void _D80FromUI16(Double80 &dst, USHORT x);
 _D80FromUI16 PROC
    FILDUINT16 rdx
    fstp       TBYTE PTR[rcx]
    ret
 _D80FromUI16 ENDP
;void _D80FromI32(Double80 &dst, const int &x);
 _D80FromI32 PROC
    fild       DWORD PTR[rdx]
    fstp       TBYTE PTR[rcx]
    ret
 _D80FromI32 ENDP
;void _D80FromUI32(Double80 &dst, UINT x);
 _D80FromUI32 PROC
    FILDUINT32 rdx
    fstp       TBYTE PTR[rcx]
    ret
 _D80FromUI32 ENDP
;void _D80FromI64(Double80 &dst, const INT64 &x);
 _D80FromI64 PROC
    fild       QWORD PTR[rdx]
    fstp       TBYTE PTR[rcx]
    ret
 _D80FromI64 ENDP
;void _D80FromUI64(Double80 &dst, const UINT64 x);
 _D80FromUI64 PROC
    FILDUINT64 rdx
    fstp       TBYTE PTR[rcx]
    ret
 _D80FromUI64 ENDP
;void _D80FromFlt(Double80 &dst, float &x);
 _D80FromFlt PROC
    fld        DWORD PTR[rdx]
    fstp       TBYTE PTR[rcx]
    ret
 _D80FromFlt ENDP
;void _D80FromDbl(Double80 &dst, const double &x);
 _D80FromDbl PROC
    fld        QWORD PTR[rdx]
    fstp       TBYTE PTR[rcx]
    ret
 _D80FromDbl ENDP

; ---------------------- Conversion functions from Double80 -> int,UINT,INT64 UINT64,float,double

;int _D80ToI32(const Double80 &x);
 _D80ToI32 PROC
    fld        TBYTE PTR[rcx]                    ; load x
    fisttp     DWORD PTR[rsp-4]
    mov        eax, DWORD PTR[rsp-4]
    ret
 _D80ToI32 ENDP
;UINT _D80ToUI32(const Double80 &x);
 _D80ToUI32 PROC
    fld        TBYTE PTR[rcx]                    ; load x
    fld        _DmaxI32P1
    fcomi      st, st(1)
    jbe        AboveMaxInt32

    fchs                                         ; x <= maxInt32
    fcomi      st, st(1)
    jae        BeloveNegMaxInt32

    fstp       st(0)
                                                 ; -maxInt32 <= x <= maxInt32
    fisttp     DWORD PTR[rsp-4]
    mov        eax, DWORD PTR[rsp-4]
    ret

  AboveMaxInt32:                                 ; x > maxInt32
    fsub
    fisttp     DWORD PTR[rsp-4]
    mov        eax, DWORD PTR[rsp-4]
    add        eax, _DWmaxI32P1
    ret

  BeloveNegMaxInt32:                             ; x < -maxInt32
    fsub
    fisttp     DWORD PTR[rsp-4]
    mov        eax, DWORD PTR[rsp-4]
    sub        eax, _DWmaxI32P1
    ret
 _D80ToUI32 ENDP
;INT64 _D80ToI64(const Double80 &x);
 _D80ToI64 PROC
    fld        TBYTE PTR[rcx]
    fisttp     QWORD PTR[rsp-8]
    mov        rax, QWORD PTR[rsp-8]
    ret
 _D80ToI64 ENDP
;UINT64 _D80ToUI64(const Double80 &x);
 _D80ToUI64 PROC
    fld        TBYTE PTR[rcx]
    fld        _D80maxi64P1
    fcomi      st, st(1)
    jbe        AboveMaxInt64

    fstp       st(0)
                                                 ; -maxInt64 <= x <= maxInt64
    fisttp     QWORD PTR[rsp-8]
    mov        rax, QWORD PTR[rsp-8]
    ret

  AboveMaxInt64:                                 ; x > maxInt64
    fsub
    fisttp     QWORD PTR[rsp-8]
    mov        rax, QWORD PTR[rsp-8]
    cmp        rax, _QWmaxI64P1
    jae        Return
    add        rax, _QWmaxI64P1
Return:
    ret
 _D80ToUI64 ENDP
;float _D80ToFlt(const Double80 &src);
 _D80ToFlt PROC
    fld        TBYTE PTR[rcx]
    fstp       DWORD PTR[rsp-4]
    movss      xmm0, DWORD PTR[rsp-4]
    ret
 _D80ToFlt ENDP
;double _D80ToDbl(const Double80 &x);
 _D80ToDbl PROC
    fld        TBYTE PTR[rcx]
    fstp       QWORD PTR[rsp-8]
    movsd      xmm0, QWORD PTR[rsp-8]
    ret
 _D80ToDbl ENDP

; -------------------------------- Compare functions -------------------------------------------

;char _D80cmpI16(const Double80 &x, const short &y);
 _D80cmpI16 PROC
    fld        TBYTE PTR[rcx]                    ; load x
    ficomp     WORD PTR[rdx]
    fnstsw     ax
    sahf
    jp         Undef
    jb         XBelowY
    setnz      al
    ret
XBelowY:
    mov        al, -1
    ret
Undef:
    mov        al, 2
    ret
 _D80cmpI16 ENDP
;char _D80cmpUI16(const Double80 &x, USHORT y);
 _D80cmpUI16 PROC
    FILDUINT16 rdx                               ; load y
    fld        TBYTE PTR[rcx]                    ; load x
    fcomip     st, st(1)                         ; st(0)=x, st(1)=y
    jp         Undef
    jb         XBelowY
    setnz      al
    fstp       st(0)
    ret
XBelowY:
    fstp       st(0)
    mov        al, -1
    ret
Undef:
    fstp       st(0)
    mov        al, 2
    ret
 _D80cmpUI16 ENDP
;char _D80cmpI32(const Double80 &x, const int &y);
 _D80cmpI32 PROC
    fld        TBYTE PTR[rcx]                    ; load x
    ficomp     DWORD PTR[rdx]
    fnstsw     ax
    sahf
    jp         Undef
    jb         XBelowY
    setnz      al
    ret
XBelowY:
    mov        al, -1
    ret
Undef:
    mov        al, 2
    ret
 _D80cmpI32 ENDP
;char _D80cmpUI32(const Double80 &x, UINT y);
 _D80cmpUI32 PROC
    FILDUINT32 rdx                               ; load y
    fld        TBYTE PTR[rcx]                    ; load x
    fcomip     st, st(1)                         ; st(0)=x, st(1)=y
    jp         Undef
    jb         XBelowY
    setnz      al
    fstp       st(0)
    ret
XBelowY:
    fstp       st(0)
    mov        al, -1
    ret
Undef:
    fstp       st(0)
    mov        al, 2
    ret
 _D80cmpUI32 ENDP
;char _D80cmpI64(const Double80 &x, const INT64 &y);
 _D80cmpI64 PROC
    fild       QWORD PTR[rdx]                    ; load y
    fld        TBYTE PTR[rcx]                    ; load x
    fcomip     st, st(1)                         ; st(0)=x, st(1)=y
    jp         Undef
    jb         XBelowY
    setnz      al
    fstp       st(0)
    ret
XBelowY:
    fstp       st(0)
    mov        al, -1
    ret
Undef:
    fstp       st(0)
    mov        al, 2
    ret
 _D80cmpI64 ENDP
;char _D80cmpUI64(const Double80 &x, UINT64 y);
 _D80cmpUI64 PROC
    FILDUINT64 rdx                               ; load y
    fld        TBYTE PTR[rcx]                    ; load x
    fcomip     st, st(1)                         ; st(0)=x, st(1)=y
    jp         Undef
    jb         XBelowY
    setnz      al
    fstp       st(0)
    ret
XBelowY:
    fstp       st(0)
    mov        al, -1
    ret
Undef:
    fstp       st(0)
    mov        al, 2
    ret
 _D80cmpUI64 ENDP
;char _D80cmpFlt(const Double80 &x, const float &y);
 _D80cmpFlt PROC
    fld        TBYTE PTR[rcx]                    ; load x
    fcomp      DWORD PTR[rdx]
    fnstsw     ax
    sahf
    jp         Undef
    jb         XBelowY
    setnz      al
    ret
XBelowY:
    mov        al, -1
    ret
Undef:
    mov        al, 2
    ret
 _D80cmpFlt ENDP
;char _D80cmpDbl(const Double80 &x, const double &y);
 _D80cmpDbl PROC
    fld        TBYTE PTR[rcx]                    ; load x
    fcomp      QWORD PTR[rdx]
    fnstsw     ax
    sahf
    jp         Undef
    jb         XBelowY
    setnz      al
    ret
XBelowY:
    mov        al, -1
    ret
Undef:
    mov        al, 2
    ret
 _D80cmpDbl ENDP
;char _D80cmpD80(const Double80 &x, const Double80 &y);
 _D80cmpD80 PROC
    fld        TBYTE PTR[rdx]                    ; load y
    fld        TBYTE PTR[rcx]                    ; load x
    fcomip     st, st(1)                         ; st(0)=x, st(1)=y
    jp         Undef
    jb         XBelowY
    setnz      al
    fstp       st(0)
    ret
XBelowY:
    fstp       st(0)
    mov        al, -1
    ret
Undef:
    fstp       st(0)
    mov        al, 2
    ret
 _D80cmpD80 ENDP

;BYTE _D80isZero(const Double80 &x);
 _D80isZero PROC
    fld        TBYTE PTR[rcx]                    ; load x
    ftst
    fnstsw     ax
    sahf
    fstp       st(0)                             ; pop x
    sete       al
    ret
 _D80isZero ENDP

; ---------------------- Binary operators +,-,*,/ (and unary minus) -----------------
; ---------------------- assign operators ++,--,-------------------------------------

;void _D80addI16(Double80 &dst, const short &x);
 _D80addI16 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fiadd      WORD PTR[rdx]                     ; add  x
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80addI16 ENDP
;void _D80subI16(Double80 &dst, const short &x);
 _D80subI16 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fisub      WORD PTR[rdx]                     ; sub  x
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80subI16 ENDP
;void _D80subrI16(Double80 &dst, const short &x);
 _D80subrI16 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fisubr     WORD PTR[rdx]                     ; st0 = x - st0
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80subrI16 ENDP
;void _D80mulI16(Double80 &dst, const short &x);
 _D80mulI16 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fimul      WORD PTR[rdx]                     ; mul  x
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80mulI16 ENDP
;void _D80divI16(Double80 &dst, const short &x);
 _D80divI16 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fidiv      WORD PTR[rdx]                     ; div  x
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80divI16 ENDP
;void _D80divrI16(Double80 &dst, const short &x);
 _D80divrI16 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fidivr     WORD PTR[rdx]                     ; st0 = x / st0
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80divrI16 ENDP
;void _D80addUI16(Double80 &dst, USHORT x);
 _D80addUI16 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    FILDUINT16 rdx                               ; load x
    fadd
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80addUI16 ENDP
;void _D80subUI16(Double80 &dst, USHORT x);
 _D80subUI16 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    FILDUINT16 rdx                               ; load x
    fsub
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80subUI16 ENDP
;void _D80subrUI16(Double80 &dst, USHORT x);
 _D80subrUI16 PROC
    FILDUINT16 rdx                               ; load x
    fld        TBYTE PTR[rcx]                    ; load dst
    fsub
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80subrUI16 ENDP
;void _D80mulUI16(Double80 &dst, USHORT x);
 _D80mulUI16 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    FILDUINT16 rdx                               ; load x
    fmul
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80mulUI16 ENDP
;void _D80divUI16(Double80 &dst, USHORT x);
 _D80divUI16 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    FILDUINT16 rdx                               ; load x
    fdiv
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80divUI16 ENDP
;void _D80divrUI16(Double80 &dst, USHORT x);
 _D80divrUI16 PROC
    FILDUINT16 rdx                               ; load x
    fld        TBYTE PTR[rcx]                    ; load dst
    fdiv
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80divrUI16 ENDP
;void _D80addI32(Double80 &dst, const int &x);
 _D80addI32 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fiadd      DWORD PTR[rdx]                    ; add  x
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80addI32 ENDP
;void _D80subI32(Double80 &dst, const int &x);
 _D80subI32 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fisub      DWORD PTR[rdx]                    ; sub  x
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80subI32 ENDP
;void _D80subrI32(Double80 &dst, const int &x);
 _D80subrI32 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fisubr     DWORD PTR[rdx]                    ; st0 = x - st0
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80subrI32 ENDP
;void _D80mulI32(Double80 &dst, const int &x);
 _D80mulI32 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fimul      DWORD PTR[rdx]                    ; mul  x
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80mulI32 ENDP
;void _D80divI32(Double80 &dst, const int &x);
 _D80divI32 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fidiv      DWORD PTR[rdx]                    ; div  x
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80divI32 ENDP
;void _D80divrI32(Double80 &dst, const int &x);
 _D80divrI32 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fidivr     DWORD PTR[rdx]                    ; st0 = x / st0
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80divrI32 ENDP
;void _D80addUI32(Double80 &dst, UINT x);
 _D80addUI32 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    FILDUINT32 rdx                               ; load x
    fadd
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80addUI32 ENDP
;void _D80subUI32(Double80 &dst, UINT x);
 _D80subUI32 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    FILDUINT32 rdx                               ; load x
    fsub
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80subUI32 ENDP
;void _D80subrUI32(Double80 &dst, UINT x);
 _D80subrUI32 PROC
    FILDUINT32 rdx                               ; load x
    fld        TBYTE PTR[rcx]                    ; load dst
    fsub
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80subrUI32 ENDP
;void _D80mulUI32(Double80 &dst, UINT x);
 _D80mulUI32 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    FILDUINT32 rdx                               ; load x
    fmul
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80mulUI32 ENDP
;void _D80divUI32(Double80 &dst, UINT x);
 _D80divUI32 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    FILDUINT32 rdx                               ; load x
    fdiv
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80divUI32 ENDP
;void _D80divrUI32(Double80 &dst, UINT x);
 _D80divrUI32 PROC
    FILDUINT32 rdx                               ; load x
    fld        TBYTE PTR[rcx]                    ; load dst
    fdiv
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80divrUI32 ENDP
;void _D80addI64(Double80 &dst, const INT64 &x);
 _D80addI64 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fild       QWORD PTR[rdx]                    ; load x
    fadd
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80addI64 ENDP
;void _D80subI64(Double80 &dst, const INT64 &x);
 _D80subI64 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fild       QWORD PTR[rdx]                    ; load x
    fsub
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80subI64 ENDP
;void _D80subrI64(Double80 &dst, const INT64 &x);
 _D80subrI64 PROC
    fild       QWORD PTR[rdx]                    ; load x
    fld        TBYTE PTR[rcx]                    ; load dst
    fsub
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80subrI64 ENDP
;void _D80mulI64(Double80 &dst, const INT64 &x);
 _D80mulI64 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fild       QWORD PTR[rdx]                    ; load x
    fmul
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80mulI64 ENDP
;void _D80divI64(Double80 &dst, const INT64 &x);
 _D80divI64 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fild       QWORD PTR[rdx]                    ; load x
    fdiv
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80divI64 ENDP
;void _D80divrI64(Double80 &dst, const INT64 &x);
 _D80divrI64 PROC
    fild       QWORD PTR[rdx]                    ; load x
    fld        TBYTE PTR[rcx]                    ; load dst
    fdiv
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80divrI64 ENDP
;void _D80addUI64(Double80 &dst, UINT64 x);
 _D80addUI64 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    FILDUINT64 rdx                               ; load x
    fadd
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80addUI64 ENDP
;void _D80subUI64(Double80 &dst, UINT64 x);
 _D80subUI64 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    FILDUINT64 rdx                               ; load x
    fsub
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80subUI64 ENDP
;void _D80subrUI64(Double80 &dst, UINT64 x);
 _D80subrUI64 PROC
    FILDUINT64 rdx                               ; load x
    fld        TBYTE PTR[rcx]                    ; load dst
    fsub
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80subrUI64 ENDP
;void _D80mulUI64(Double80 &dst, UINT64 x);
 _D80mulUI64 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    FILDUINT64 rdx                               ; load x
    fmul
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80mulUI64 ENDP
;void _D80divUI64(Double80 &dst, UINT64 x);
 _D80divUI64 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    FILDUINT64 rdx                               ; load x
    fdiv
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80divUI64 ENDP
;void _D80divrUI64(Double80 &dst, UINT64 x);
 _D80divrUI64 PROC
    FILDUINT64 rdx                               ; load x
    fld        TBYTE PTR[rcx]                    ; load dst
    fdiv
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80divrUI64 ENDP
;void _D80addFlt(Double80 &dst, const float &x);
 _D80addFlt PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fadd       DWORD PTR[rdx]                    ; add  x
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80addFlt ENDP
;void _D80subFlt(Double80 &dst, const float &x);
 _D80subFlt PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fsub       DWORD PTR[rdx]                    ; sub  x
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80subFlt ENDP
;void _D80subrFlt(Double80 &dst, const float &x);
 _D80subrFlt PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fsubr      DWORD PTR[rdx]                    ; st0 = x - st0
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80subrFlt ENDP
;void _D80mulFlt(Double80 &dst, const float &x);
 _D80mulFlt PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fmul       DWORD PTR[rdx]                    ; mul  x
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80mulFlt ENDP
;void _D80divFlt(Double80 &dst, const float &x);
 _D80divFlt PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fdiv       DWORD PTR[rdx]                    ; div  x
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80divFlt ENDP
;void _D80divrFlt(Double80 &dst, const float &x);
 _D80divrFlt PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fdivr      DWORD PTR[rdx]                    ; st0 = x / st0
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80divrFlt ENDP
;void _D80addDbl(Double80 &dst, const double &x);
 _D80addDbl PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fadd       QWORD PTR[rdx]                    ; add  x
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80addDbl ENDP
;void _D80subDbl(Double80 &dst, const double &x);
 _D80subDbl PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fsub       QWORD PTR[rdx]                    ; sub  x
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80subDbl ENDP
;void _D80subrDbl(Double80 &dst, const double &x);
 _D80subrDbl PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fsubr      QWORD PTR[rdx]                    ; st0 = x - st0
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80subrDbl ENDP
;void _D80mulDbl(Double80 &dst, const double &x);
 _D80mulDbl PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fmul       QWORD PTR[rdx]                    ; mul  x
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80mulDbl ENDP
;void _D80divDbl(Double80 &dst, const double &x);
 _D80divDbl PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fdiv       QWORD PTR[rdx]                    ; div  x
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80divDbl ENDP
;void _D80divrDbl(Double80 &dst, const double &x);
 _D80divrDbl PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fdivr      QWORD PTR[rdx]                    ; st0 = x / st0
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80divrDbl ENDP
;void _D80addD80(Double80 &dst, const Double80 &x);
 _D80addD80 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fld        TBYTE PTR[rdx]                    ; load x
    fadd
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80addD80 ENDP
;void _D80subD80(Double80 &dst, const Double80 &x);
 _D80subD80 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fld        TBYTE PTR[rdx]                    ; load x
    fsub
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80subD80 ENDP
;void _D80mulD80(Double80 &dst, const Double80 &x);
 _D80mulD80 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fld        TBYTE PTR[rdx]                    ; load x
    fmul
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80mulD80 ENDP
;void _D80divD80(Double80 &dst, const Double80 &x);
 _D80divD80 PROC
    fld        TBYTE PTR[rcx]                    ; load dst
    fld        TBYTE PTR[rdx]                    ; load x
    fdiv
    fstp       TBYTE PTR[rcx]                    ; pop to dst
    ret
 _D80divD80 ENDP

;void _D80rem(Double80 &dst, const Double80 &y);
 _D80rem PROC
    fld        TBYTE PTR[rdx]                    ;                                              st0=y
    fabs                                         ; y = abs(y)                                   st0=|y|
    fld        TBYTE PTR[rcx]                    ;                                              st0=dst,st1=|y|
    fldz                                         ;                                              st0=0  ,st1=dst,st2=|y|
    fcomip     st, st(1)                         ; compare and pop zero                         st0=dst,st1=|y|
    ja         RepeatNegativeX                   ; if(st(0) > st(1)) // (0 > x)
                                                 ;   goto RepeatNegativeX
RepeatPositiveX:                                 ; do {                                         st0=dst,st1=|y|, dst > 0
    fprem                                        ;   st0 %= y
    fstsw      ax
    sahf
    jpe        RepeatPositiveX                   ; } while(statusword.c2 != 0);
    fldz                                         ;                                              st0=0  ,st1=x,st2=|y|
    fcomip     st, st(1)                         ; compare and pop zero
    jbe        pop2                              ; if(st(0) <= st(1)) (0 <= remainder) goto pop2
    fadd                                         ; remainder += y
    fstp       TBYTE PTR[rcx]                    ; pop dst
    ret                                          ; return

RepeatNegativeX:                                 ; do {                                          st0=x  ,st=|y|, x < 0
    fprem                                        ;   st0 %= y
    fstsw      ax
    sahf
    jpe        RepeatNegativeX                   ; } while(statusword.c2 != 0)
    fldz
    fcomip     st, st(1)                         ; compare and pop zero
    jae        pop2                              ; if(st(0) >= st(1)) (0 >= remainder) goto pop2
    fsubr                                        ; remainder -= y
    fstp       TBYTE PTR[rcx]                    ; pop dst
    ret                                          ; return

pop2:                                            ;                                               st0=x%y,st1=y
    fstp       TBYTE PTR[rcx]                    ; pop dst
    fstp       st(0)                             ; pop y
    ret
 _D80rem ENDP
;void _D80neg(Double80 &x);
 _D80neg PROC
    fld        TBYTE PTR[rcx]                    ; load x
    fchs
    fstp       TBYTE PTR[rcx]                    ; pop to x
    ret
 _D80neg ENDP

;void _D80inc(Double80 &x);
 _D80inc PROC
    fld        TBYTE PTR[rcx]                    ; load x
    fld1
    fadd
    fstp       TBYTE PTR[rcx]                    ; pop to x
    ret
 _D80inc ENDP
;void _D80dec(Double80 &x);
 _D80dec PROC
    fld        TBYTE PTR[rcx]                    ; load x
    fld1
    fsub
    fstp       TBYTE PTR[rcx]                    ; pop to x
    ret
 _D80dec ENDP

; -------------------------------------------------- Double80 Functions ----------------------------------------

;int _D80getExpo10(const Double80 &x);
 _D80getExpo10 PROC
    fld        TBYTE PTR[rcx]                    ; st0 = x
    fldz
    fcomip     st, st(1)                         ; compare x and pop 0
    je         xIsZero                           ; if(x == 0) goto xIsZero
    fabs                                         ; st0 = |x|
    fldlg2                                       ; st0 = log10(2), st1 = |x|
    fxch       st(1)                             ; st0 = |x|     , st1 = log10(2)=ln(2)/ln(10)
    fyl2x                                        ; st0 = st1*log2(st0) = ln(2)/ln(10)*ln(x)/ln(2) = log10(|x|)
    pushRoundMode ROUNDDOWN
    fistp      DWORD PTR[rsp-4]
    mov        eax, DWORD PTR[rsp-4]
    popRoundMode
    ret
xIsZero:
    fstp       st(0)                             ; pop x
    xor        eax, eax                          ; x == 0 => result = 0
    ret
 _D80getExpo10 ENDP
;void _D80fabs(Double80 &x);
 _D80fabs PROC
    fld        TBYTE PTR[rcx]
    fabs
    fstp       TBYTE PTR[rcx]
    ret
 _D80fabs ENDP
;void _D80sqr(Double80 &x);
 _D80sqr PROC
    fld        TBYTE PTR[rcx]
    fmul       st(0), st(0)
    fstp       TBYTE PTR[rcx]
    ret
 _D80sqr ENDP
;void _D80sqrt(Double80 &x);
 _D80sqrt PROC
    fld        TBYTE PTR[rcx]
    fsqrt
    fstp       TBYTE PTR[rcx]
    ret
 _D80sqrt ENDP

; ----------------------------------------- Double80 trigonometric functions ----------------------------------------
;void _D80sin(Double80 &x);
 _D80sin PROC
    lea        rdx, _D802PiExp260
    call       _D80rem
    fld        TBYTE PTR[rcx]
    fsin
    fstp       TBYTE PTR[rcx]
    ret
 _D80sin ENDP
;void _D80cos(Double80 &x);
 _D80cos PROC
    lea        rdx, _D802PiExp260
    call       _D80rem
    fld        TBYTE PTR[rcx]
    fcos
    fstp       TBYTE PTR[rcx]
    ret
 _D80cos ENDP
;void _D80tan(Double80 &x);
 _D80tan PROC
    lea        rdx, _D802PiExp260
    call       _D80rem
    fld        TBYTE PTR[rcx]
    fptan
    fstp       st
    fstp       TBYTE PTR[rcx]
    ret
 _D80tan ENDP
;void _D80atan(Double80 &x);
 _D80atan PROC
    fld        TBYTE PTR[rcx]
    fld1
    fpatan
    fstp       TBYTE PTR[rcx]
    ret
 _D80atan ENDP
;void _D80atan2(Double80 &y, const Double80 &x);
 _D80atan2 PROC
    fld        TBYTE PTR[rcx]
    fld        TBYTE PTR[rdx]
    fpatan
    fstp       TBYTE PTR[rcx]
    ret
 _D80atan2 ENDP
; inout c, out s
;void _D80sincos(Double80 &c, Double80 &s);
 _D80sincos PROC
    mov        r8, rdx
    lea        rdx, _D802PiExp260
    call       _D80rem
    fld        TBYTE PTR[rcx]
    fsincos
    fstp       TBYTE PTR[rcx]
    fstp       TBYTE PTR[r8]
    ret
 _D80sincos ENDP

; --------------------------------------------------- Double80 Exponential and Logarithmic functions ------------------

;void _D80exp(Double80 &x);
 _D80exp PROC
    fld        TBYTE PTR[rcx]                    ; st0 = x
    fldl2e                                       ; st0 = log2(e)              , st1 = x
    fmul                                         ; st0 = x/ln(2)
    fld        st(0)                             ; st0 = x/ln(2)              , st1 = x/ln(2)
    frndint                                      ; st0 = floor(  x/ln(2   ))  , st1 = x/ln(2)
    fsub       st(1), st(0)                      ; st0 = floor(  x/ln(2   ))  , st1 = frac( x/ln(2))
    fxch       st(1)                             ; st0 = frac(   x/ln(2   ))  , st1 = floor(x/ln(2))
    f2xm1                                        ; st0 = 2^ frac(x/ln(2   ))-1, st1 = floor(x/ln(2))
    fld1
    fadd                                         ; st0 = 2^ frac(x/ln(2   ))   , st1 = floor(x/ln(2))
    fscale                                       ; st0 = 2^(frac(x/ln(2   ))+floor(x/ln(2   )))=e ^x, st1 = floor(x/ln(2))
    fstp       st(1)                             ; pop st1
    fstp       TBYTE PTR[rcx]                    ; pop to x
    ret
 _D80exp ENDP
;void _D80exp10(Double80 &x);
 _D80exp10 PROC
    fld        TBYTE PTR[rcx]                    ; st0 = x
    fldz
    fcomip     st, st(1)
    je         ZeroExponent

    fldl2t                                       ; st0 = log2(10)             , st1 = x
    fmul                                         ; st0 = x*log2(10)
    fld        st(0)
    frndint                                      ; st0 = floor(  x*log2(10))  , st1 = x*log2(10)
    fsub       st(1), st(0)                      ; st0 = floor(  x*log2(10))  , st1 = frac( x*log2(10))
    fxch       st(1)                             ; st0 = frac(   x*log2(10))  , st1 = floor(x*log2(10))
    f2xm1                                        ; st0 = 2^ frac(x*log2(10))-1, st1 = floor(x*log2(10))
    fld1
    fadd                                         ; st0 = 2^ frac(x*log2(10))  , st1 = floor(x*log2(10))
    fscale                                       ; st0 = 2^(frac(x*log2(10))+floor(x*log2(10)))=10^x, st1 = floor(x*log2(10))
    fstp       st(1)                             ; pop st1
    fstp       TBYTE PTR[rcx]                    ; pop to x
    ret
ZeroExponent:                                    ; st0 = x
    fstp       st(0)                             ; pop st0
    fld1                                         ; st0 = 1
    fstp       TBYTE PTR[rcx]                    ; pop to x
    ret
 _D80exp10 ENDP
;void _D80exp2(Double80 &x);
 _D80exp2 PROC
    fld        TBYTE PTR[rcx]                    ; st0 = x
    fldz
    fcomip     st, st(1)
    je         ZeroExponent

    pushRoundMode ROUNDDOWN                      ; st0 = x (!= 0)
    fld        st(0)                             ; st0 = x                    , st1 = x
    frndint                                      ; st0 = floor(  x         )  , st1 = x
    fsub       st(1), st(0)                      ; st0 = floor(  x         )  , st1 = frac(x)
    fxch       st(1)                             ; st0 = frac(   x         )  , st1 = floor(x)
    f2xm1                                        ; st0 = 2^ frac(x         )-1, st1 = floor(x)
    fld1
    fadd                                         ; st0 = 2^ frac(x         )  , st1 = floor(x)
    fscale                                       ; st0 = 2^(frac(x         )+floor(x         ))=2^x, st1 = floor(x)
    fstp       st(1)                             ; pop st0
    fstp       TBYTE PTR[rcx]                    ; pop to x
    popRoundMode
    ret
ZeroExponent:                                    ; st0 = x
    fstp       st(0)                             ; pop st0
    fld1                                         ; st0 = 1
    fstp       TBYTE PTR[rcx]                    ; pop to x
    ret
 _D80exp2 ENDP

;void _D80log(Double80 &x);
 _D80log PROC
    fldln2                                       ; st0 = ln(2)
    fld        TBYTE PTR[rcx]                    ; st0 = x     , st1 = ln(2)
    fyl2x                                        ; st0 = st1*log2(st0) = ln(2)*ln(st0)/ln(2) = ln(x)
    fstp       TBYTE PTR[rcx]                    ; pop to x
    ret
 _D80log ENDP
;void _D80log10(Double80 &x);
 _D80log10 PROC
    fldlg2                                       ; st0 = log10(2)
    fld        TBYTE PTR[rcx]                    ; st0 = x     , st1 = log10(2) = ln(2)/ln(10)
    fyl2x                                        ; st0 = st1*log2(st0) = ln(2)/ln(10)*ln(st0)/ln(2) = log10(st0)
    fstp       TBYTE PTR[rcx]                    ; pop to x
    ret
 _D80log10 ENDP
;void _D80log2(Double80 &x);
 _D80log2 PROC
    fld1                                         ; st0 = 1
    fld        TBYTE PTR[rcx]                    ; st0 = x     , st1 = 1
    fyl2x                                        ; st0 = st1*log2(st0) = log2(x)
    fstp       TBYTE PTR[rcx]                    ; pop to x
    ret
 _D80log2 ENDP
; x = pow(x,y)
;void _D80pow(Double80 &x, const Double80 &y);
 _D80pow PROC
    fld        TBYTE PTR[rdx]                    ; st0 = y
    fldz
    fcomip     st, st(1)
    je         ZeroExponent                      ; if(y == 0) goto ZeroExponent;

    fld        TBYTE PTR[rcx]                    ; st0 = x                    , st1 = y
    fldz
    fcomip     st, st(1)
    je         ZeroBase                          ; if(x == 0) goto ZeroExponent;

                                                 ; st0 = x (!= 0)             , st1 = y (!= 0)
    fyl2x                                        ; st0 = st1*log2(st0) = y*log2(x)
    fld        st(0)                             ;
    frndint                                      ; st0 = floor(  y*log2(x ))  , st1 = y*log2(x )
    fsub       st(1), st(0)                      ; st0 = floor(  y*log2(x ))  , st1 = frac( y*log2(x ))
    fxch       st(1)                             ; st0 = frac(   y*log2(x ))  , st1 = floor(y*log2(x ))
    f2xm1                                        ; st0 = 2^ frac(y*log2(x ))-1, st1 = floor(y*log2(x ))
    fld1
    fadd                                         ; st0 = 2^ frac(y*log2(x ))  , st1 = floor(y*log2(x ))
    fscale                                       ; st0 = 2^(frac(y*log2(x ))+floor(y*log2(x )))=x ^y, st1 = floor(y*log2(x))
    fstp       st(1)                             ; pop st1
    fstp       TBYTE PTR[rcx]                    ; pop to x
    ret
ZeroExponent:                                    ; st0 = y
    fstp       st(0)                             ; pop st0
    fld1                                         ; st0 = 1
    fstp       TBYTE PTR[rcx]                    ; pop to x
    ret
ZeroBase:                                        ; st0 = x, st1 = y. x = 0. so st0 = 0
    fcomip     st, st(1)
    ja         ZeroBaseNegativeExponent    
    fstp       st(0)                             ; pop st0
    fldz                                         ; st0 = 0
    fstp       TBYTE PTR[rcx]                    ; pop to x
    ret
ZeroBaseNegativeExponent:                        ; st0 = y
    fstp       st(0)                             ; pop st0
    fld1                                         ; st0 = 1
    fldz                                         ; st0 = 0, st1 = 1
    fdiv                                         ; st0 = 1/0
    fstp       TBYTE PTR[rcx]                    ; pop to x - error
    ret
 _D80pow ENDP
; dst = 2^p
;void _D80pow2(Double80 &dst, const int &p);
 _D80pow2 PROC
    fild       DWORD PTR[rdx]
    fld1
    fscale
    fstp       TBYTE PTR[rcx]
    fstp       st(0)
    ret
 _D80pow2 ENDP
; ------------------------------------------------- Double80 floor,ceil --------------------------------

;void _D80floor(Double80 &x);
 _D80floor PROC
    fld        TBYTE PTR[rcx]
    pushRoundMode ROUNDDOWN
    frndint
    fstp       TBYTE PTR[rcx]
    popRoundMode
    ret
 _D80floor ENDP
;void _D80ceil(Double80 &x);
 _D80ceil PROC
    fld        TBYTE PTR[rcx]
    pushRoundMode ROUNDUP
    frndint
    fstp       TBYTE PTR[rcx]
    popRoundMode
    ret
 _D80ceil ENDP

END
