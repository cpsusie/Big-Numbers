.CODE

;void callDoubleResultExpression(function f, double &result);
callDoubleResultExpression PROC
    push        rdx
    call        rcx
    pop         rdx
    fstp        qword ptr[rdx]
    ret
callDoubleResultExpression ENDP

END

