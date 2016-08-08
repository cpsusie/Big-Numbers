.CODE

;void callDoubleResultExpression(ExpressionEntryPoint ep, Real *valueTable, double &result);
callDoubleResultExpression PROC
    push        rsi
    push        rdi
    sub         rsp, 8
    mov         rsi, rdx
    mov         rdi, r8
    call        rcx
    add         rsp, 8
    pop         rdi
    pop         rsi
    ret
callDoubleResultExpression ENDP

;int callIntResultExpression(ExpressionEntryPoint ep, Real *valueTable);
callIntResultExpression PROC
    push        rsi
    mov         rsi, rdx
    call        rcx
    pop         rsi
    ret
callIntResultExpression ENDP

END

