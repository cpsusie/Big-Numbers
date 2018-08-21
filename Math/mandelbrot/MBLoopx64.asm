; 
.DATA

_4  dword 40800000h

.CODE

;void prepareFPU();
prepareFPU PROC          ;                         st0       st1       st2       st3       st4       st5       st6       st7
    fld    _4            ; Load 4                    4
    ret
prepareFPU ENDP

;void cleanupFPU();
cleanupFPU PROC          ;
    push	0
    fnstcw WORD PTR[rsp] ; save FPU ctrlWord in cwSave (=*rsp)
    fninit
    fldcw	 WORD PTR[rsp] ; restore FPU ctrlWord (*rsp)
    add		 rsp, 8
    ret
cleanupFPU ENDP

;UINT64 mbloop(const Double80 &x, const Double80 &y, UINT64 maxCount);
mbloop PROC              ;                         st0       st1       st2       st3       st4       st5       st6       st7
                         ;                           4 assumed to be loaded with prepareFPU
    fld    tbyte ptr[rdx]; Load y                    y         4
    fld    tbyte ptr[rcx]; Load x                    x         y         4
    mov    rcx  , r8
    fld    st(1)         ; Load y                    b=y       x         y         4
    fld    st(1)         ; Load x                    a=x       b         x         y         4

forloop:                 ; Stacksize = 5             a         b         x         y         4
    fld    st(0)         ; Load a.                   a         a         b         x         y         4
    fmul   st(0), st(0)  ; st0*=st0                  a^2       a         b         x         y         4
    fld    st(2)         ; Load b                    b         a^2       a         b         x         y         4
    fmul   st(0), st(0)  ; st0*=st0                  b^2       a^2       a         b         x         y         4
    fld    st(1)         ; Load a^2.                 a^2       b^2       a^2       a         b         x         y         4
    fadd   st(0), st(1)  ; st0 += st1                a^2+b^2   b^2       a^2       a         b         x         y         4
    fcomip st(0), st(7)  ; Compare st0,st7, pop st0  b^2       a^2       a         b         x         y         4
    ja EpilogPop6        ; Stacksize = 7. if(a^2+b^2 > 4) goto EpilogPop6

    fsub                 ; st1 -= st0, pop st0       a^2-b^2   a         b         x         y         4
    fadd   st(0), st(3)  ; st0 += x                  a^2-b^2+x a         b         x         y         4
    fld    st(1)         ; Load a                    a         a^2-b^2+x a         b         x         y         4
    fmul   st(0), st(3)  ; st0 *= b                  ab        a^2-b^2+x a         b         x         y         4
    fadd   st(0), st(0)  ; st0 *= 2                  2ab       a^2-b^2+x a         b         x         y         4
    fadd   st(0), st(5)  ; st0 += st(5)              2ab+y     a^2-b^2+x a         b         x         y         4
    fstp   st(3)         ; b = 2ab+y                 a^2-b^2+x a         new b     x         y         4
    fstp   st(1)         ; a = a^2-b^2+x, pop st0    new a     new b     x         y         4
    loop   forloop       ; Stacksize = 5. if(--rcx) goto forloop
    
EpilogPop4:              ; we have reached maxCount => rcx == 0
    fstp   st(0)
    fstp   st(0)
    fstp   st(0)
    fstp   st(0)
    mov    rax, r8
    ret
EpilogPop6:
    fstp   st(0)
    fstp   st(0)
    fstp   st(0)
    fstp   st(0)
    fstp   st(0)
    fstp   st(0)
    sub    rcx, r8
    neg    rcx
    mov    rax, rcx
    ret
mbloop ENDP

END
