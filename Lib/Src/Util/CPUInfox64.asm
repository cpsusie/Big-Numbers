; Assembler routines for getting CPU info
;
.CODE

;void getCPUBrandString(char *dst)
getCPUBrandString PROC
    mov r8, rcx
    mov BYTE PTR[r8], 0
    mov rax, 80000000H
    cpuid

    test eax, 80000000h
    jz Return

    cmp eax, 80000004h
    jb Return

    push rbx                      ; need rbx in the following

    mov rax, 80000002H
    cpuid
    mov DWORD PTR[r8]   , eax
    mov DWORD PTR[r8+4] , ebx
    mov DWORD PTR[r8+8] , ecx
    mov DWORD PTR[r8+12], edx

    mov rax,80000003H
    cpuid
    mov DWORD PTR[r8+16], eax
    mov DWORD PTR[r8+20], ebx
    mov DWORD PTR[r8+24], ecx
    mov DWORD PTR[r8+28], edx
  
    mov rax,80000004H
    cpuid
    mov DWORD PTR[r8+32], eax
    mov DWORD PTR[r8+36], ebx
    mov DWORD PTR[r8+40], ecx
    mov DWORD PTR[r8+44], edx
    xor al, al
    mov BYTE  PTR[r8+48], al      ; zero terminate brandstring

    pop rbx
Return:
    ret
getCPUBrandString ENDP


;int getCPUData(char *vendor, UINT &versionInfo, UINT &cpuBasics, UINT &featureInfo);
getCPUData PROC
    push rsi
    push rdi
    push rbx
    push r12

    mov  rsi, rcx                 ; rsi pointer to vendor 
    mov  rdi, rdx                 ; rdi pointer to versionInfo
                                  ; r8  pointer to cpuBasics
                                  ; r9  pointer to featureInfo
    xor  rax, rax
    cpuid                         ; call cpuid(eax=0)
    mov  r12, rax                 ; r12 = highestEAX
    mov  DWORD PTR[rsi  ], ebx    ; assign vendor
    mov  DWORD PTR[rsi+4], edx
    mov  DWORD PTR[rsi+8], ecx
    xor  rax, rax
    mov  BYTE  PTR[rsi+12],al     ; zero terminate vendor

    cmp  r12, 1                   ; if(highestEAX >= 1) {
    jb   Return

    mov  rax, 1
    cpuid                         ;   call cpuid(eax=1)
    mov  DWORD PTR[rdi], eax      ;   versionInfo = eax
    mov  DWORD PTR[r8] , ebx      ;   cpuBasics   = ebx
    mov  DWORD PTR[r9] , edx      ;   featureInfo = edx
                                  ; }

Return:
    mov rax, r12
    pop r12
    pop rbx
    pop rdi
    pop rsi
    ret
getCPUData ENDP

;UINT64 getCPUSerialNumber();
getCPUSerialNumber PROC
    mov rax, 3
    cpuid                         ;   call cpuid(eax=3)
    mov rax, rdx
    shl rax, 32
    or  eax, ecx
    ret
getCPUSerialNumber ENDP

END
