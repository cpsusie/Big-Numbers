; Fast routines for FindRange2Equal x64
;

include \mytools2015\lib\src\math\fpu.inc

.DATA

_8   tbyte 8.0

.CODE

;int findRange2Equal(EndGamePosIndex f, EndGamePosIndex index)
;return n = (sqrt(1 + (double)index*8/f) - 1) / 2; invers of index = f*n*(n+1)/2 = sum(i=1..n) { f * i }

findRange2Equal PROC
    pushRoundMode TRUNCATE
    push rdx
    push rcx
    fild qword ptr[rsp+8]              ; load index (rdx)
    fld  _8
    fmul
    fild qword ptr[rsp]                ; fild f     (rcx)
    fdiv
    fld1
    fadd
    fsqrt
    fld1
    fsub
    fistp qword ptr[rsp]               ; *rsp = result
    pop rax
    pop rdx
    popRoundMode
    sar rax, 1
    ret

findRange2Equal ENDP

END
