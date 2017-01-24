; 
.DATA

four              tbyte 403dc90fdaa22168c235h

.CODE

; UINT64 mbloop(const Double80 &x, const Double80 &y, UINT64 maxIteration);
mbloop PROC                ;                                   st0        st1        st2        st3        st4        st5        st6        st7
    fld    four            ; Load 4                              4
    fld    tbyte ptr[rdx]  ; Load y                              y          4
    fld    tbyte ptr[rcx]  ; Load x                              x          y          4
    mov    rcx  , r8                                        
    fld    st(1)           ; Load y                              b=y        x          y          4
    fld    st(1)           ; Load x                              a=x        b          x          y          4

forloop:                   ; Stacksize = 5                       a          b          x          y          4
    fld	   st(0)           ; Load a.                             a          a          b          x          y           4   
    fmul   st(0), st(0)    ; st0*=st0                            a*a        a          b          x          y           4
    fld    st(2)           ; Load b                              b          a*a        a          b          x           y         4
    fmul   st(0), st(0)    ; st0*=st0                            b*b        a*a        a          b          x           y         4
    fld    st(1)           ; Load b^2.                           b*b        b*b        a*a        a          b           x         y          4
    fadd   st(0), st(1)    ; st(0) = a^2 + b^2.                  a*a+b*b    b*b        a*a        a          b           x         y          4
    fcomip st(0), st(7)    ; Compare st0 and st7, pop st0        b*b        a*a        a          b          x           y         4
    ja Epilog              ; if(a*a+b*b > 4) goto Epilog

    fsub                   ; st0 = a*a-b*b, pop st1              a*a-b*b    a          b          x          y           4
    fadd   st(0), st(3)    ; st0 += x                            a*a-b*b+x  a          b          x          y           4
    fld    st(1)           ; Load a                              a          a*a-b*b+x  a          b          x           y         4
    fmul   st(0), st(3)    ; st0 *= b                            a*b        a*a-b*b+x  a          b          x           y         4
    fadd   st(0), st(0)    ; st0 *= 2                            2*a*b      a*a-b*b+x  a          b          x           y         4
    fadd   st(0), st(5)    ; st0 += y                            2*a*b+y    a*a-b*b+x  a          b          x           y         4
    fstp   st(3)           ; b = 2*a*b+y, pop st0                a*a-b*b+x  a          new b      x          y           4
    fstp   st(1)           ; a = a*a-b*b+x, pop st0              new a      new b      x          y          4
    loop   forloop         ; Stacksize = 5. if(--rcx) goto forloop
    
Epilog:
    push	0
    fnstcw WORD PTR[rsp]   ; save FPU ctrlWord in cwSave (=*rsp)
    fninit
    fldcw	 WORD PTR[rsp]   ; restore FPU  ctrlWord (*rsp)
    add		 rsp, 8
    sub    rcx, r8
    neg    rcx
    mov    rax,rcx
    ret
mbloop ENDP

END