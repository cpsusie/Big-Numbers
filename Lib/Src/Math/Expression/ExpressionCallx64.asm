.CODE

;void callRealResultExpression(ExpressionEntryPoint ep, const void *rsiValue, Real &result);
callRealResultExpression PROC
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
callRealResultExpression ENDP

;int callIntResultExpression(ExpressionEntryPoint ep, const void *rsiValue);
callIntResultExpression PROC
    push        rsi
    mov         rsi, rdx
    call        rcx
    pop         rsi
    ret
callIntResultExpression ENDP

END
