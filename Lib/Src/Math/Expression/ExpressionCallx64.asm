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

;void fiskx64()
fisk PROC
    mov al ,1
    mov cl ,1
    mov r8b,1

    mov ax ,1
    mov cx ,1
    mov r8w,1
    mov ax ,1234h
    mov cx ,1234h
    mov r8w,1234h

    mov eax,1
    mov ecx,1
    mov r8d,1
    mov eax,12345678h
    mov ecx,12345678h
    mov r8d,12345678h

    mov rax,1
    mov rcx,1
    mov r8 ,1
    mov rax,1234567890123456h
    mov rcx,1234567890123456h
    mov r8 ,1234567890123456h
    ret
fisk ENDP
END

