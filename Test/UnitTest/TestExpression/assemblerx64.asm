.CODE

;void assemblerCode()
assemblerCode PROC
    ret

  ADD AL  , 07fh
  ADD EAX , 07fh
  ADD EAX , 07fffffffh
  ADD AX  , 07fh
  ADD AX  , 07fffh
  ADD CL  , 07fh
  ADD ECX , 07fh
  ADD ECX , 07fffffffh
  ADD CX  , 07fh
  ADD CX  , 07fffh
  ADD RAX , 07fh
  ADD RAX , 07fffffffh
  ADD R8B , 07fh
  ADD R8D , 07fh
  ADD R8D , 07fffffffh
  ADD R8  , 07fh
  ADD R8  , 07fffffffh
  ADD R8W , 07fh
  ADD R8W , 07fffh

    movaps xmm0,xmm0
    movaps xmm0,xmm1
    movaps xmm0,xmm2
    movaps xmm0,xmm3
    movaps xmm1,xmm0
    movaps xmm1,xmm1
    movaps xmm1,xmm2
    movaps xmm1,xmm3
    movaps xmm0,xmm8
    movaps xmm0,xmm9
    movaps xmm0,xmm10
    movaps xmm0,xmm11
    movaps xmm8,xmm8
    movaps xmm8,xmm9
    movaps xmm8,xmm10
    movaps xmm8,xmm11

    movaps xmm0,xmm1
    movaps xmm0,xmm2
    movaps xmm0,xmm3
    movaps xmm1,xmm0
    movaps xmm1,xmm1
    movaps xmm1,xmm2
    movaps xmm1,xmm3
    movaps xmm0,xmm8
    movaps xmm0,xmm9
    movaps xmm0,xmm10
    movaps xmm0,xmm11
    movaps xmm8,xmm8
    movaps xmm8,xmm9
    movaps xmm8,xmm10
    movaps xmm8,xmm11

    mov al                   , byte  ptr [12345678h]
    mov eax                  , dword ptr [12345678h]
    mov ax                   , word  ptr [12345678h]
    mov rax                  , qword ptr [12345678h]
    mov byte  ptr [12345678h], al
    mov dword ptr [12345678h], eax
    mov word  ptr [12345678h], ax
    mov qword ptr [12345678h], rax

    mov al                           , byte  ptr [1234567812345678h]
    mov eax                          , dword ptr [1234567812345678h]
    mov ax                           , word  ptr [1234567812345678h]
    mov rax                          , qword ptr [1234567812345678h]
    mov byte  ptr [1234567812345678h], al
    mov dword ptr [1234567812345678h], eax
    mov word  ptr [1234567812345678h], ax
    mov qword ptr [1234567812345678h], rax

    mov byte  ptr [12345678h], cl
    mov byte  ptr [12345678h], dl
    mov byte  ptr [12345678h], bl
    mov byte  ptr [12345678h], ah
    mov byte  ptr [12345678h], ch
    mov byte  ptr [12345678h], r8b
    mov byte  ptr [12345678h], r9b
    mov byte  ptr [12345678h], r10b
    mov byte  ptr [12345678h], r11b
    mov byte  ptr [12345678h], r12b

    mov dword ptr [12345678h], ecx
    mov dword ptr [12345678h], edx
    mov dword ptr [12345678h], ebx
    mov dword ptr [12345678h], esp
    mov dword ptr [12345678h], ebp
    mov dword ptr [12345678h], r8d
    mov dword ptr [12345678h], r9d
    mov dword ptr [12345678h], r10d
    mov dword ptr [12345678h], r11d
    mov dword ptr [12345678h], r12d

    mov qword ptr [12345678h], rcx
    mov qword ptr [12345678h], rdx
    mov qword ptr [12345678h], rbx
    mov qword ptr [12345678h], rsp
    mov qword ptr [12345678h], rbp
    mov qword ptr [12345678h], r8 
    mov qword ptr [12345678h], r9 
    mov qword ptr [12345678h], r10
    mov qword ptr [12345678h], r11
    mov qword ptr [12345678h], r12

    mov cl                   , byte  ptr [12345678h]
    mov dl                   , byte  ptr [12345678h]
    mov bl                   , byte  ptr [12345678h]
    mov ah                   , byte  ptr [12345678h]
    mov ch                   , byte  ptr [12345678h]
    mov r8b                  , byte  ptr [12345678h]
    mov r9b                  , byte  ptr [12345678h]
    mov r10b                 , byte  ptr [12345678h]
    mov r11b                 , byte  ptr [12345678h]
    mov r12b                 , byte  ptr [12345678h]

    mov ecx                     , dword ptr [12345678h]
    mov edx                     , dword ptr [12345678h]
    mov ebx                     , dword ptr [12345678h]
    mov esp                     , dword ptr [12345678h]
    mov ebp                     , dword ptr [12345678h]
    mov r8d                     , dword ptr [12345678h]
    mov r9d                     , dword ptr [12345678h]
    mov r10d                    , dword ptr [12345678h]
    mov r11d                    , dword ptr [12345678h]
    mov r12d                    , dword ptr [12345678h]

    mov word  ptr [12345678h], cx
    mov word  ptr [12345678h], dx
    mov word  ptr [12345678h], bx
    mov word  ptr [12345678h], sp
    mov word  ptr [12345678h], bp
    mov word  ptr [12345678h], r8w
    mov word  ptr [12345678h], r9w
    mov word  ptr [12345678h], r10w
    mov word  ptr [12345678h], r11w
    mov word  ptr [12345678h], r12w

    mov cx                      , word  ptr [12345678h]
    mov dx                      , word  ptr [12345678h]
    mov bx                      , word  ptr [12345678h]
    mov sp                      , word  ptr [12345678h]
    mov bp                      , word  ptr [12345678h]
    mov r8w                     , word  ptr [12345678h]
    mov r9w                     , word  ptr [12345678h]
    mov r10w                    , word  ptr [12345678h]
    mov r11w                    , word  ptr [12345678h]
    mov r12w                    , word  ptr [12345678h]

    ret
assemblerCode ENDP

END
