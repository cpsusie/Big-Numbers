.CODE

;void callDoubleResultExpression(ExpressionEntryPoint e, double &result);
callDoubleResultExpression PROC
    push        rdx
    call        rcx
    pop         rdx
    fstp        qword ptr[rdx]
    ret
callDoubleResultExpression ENDP

;int callIntResultExpression(ExpressionEntryPoint e);
callIntResultExpression PROC
    jmp         rcx
callIntResultExpression ENDP

END

