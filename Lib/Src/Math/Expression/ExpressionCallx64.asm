.CODE

;void callDoubleResultExpression(ExpressionEntryPoint e, double &result);
callDoubleResultExpression PROC
    push        rdi
    mov         rdi, rdx
    call        rcx
    pop         rdi
    ret
callDoubleResultExpression ENDP

;int callIntResultExpression(ExpressionEntryPoint e);
callIntResultExpression PROC
    jmp         rcx
callIntResultExpression ENDP

END

