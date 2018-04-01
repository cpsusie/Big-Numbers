.CODE

;void assemblerCode()
assemblerCode PROC
    ret

    mov ax , es
    mov cx , es
    mov dx , es
    mov bx , es
    mov ax , cs
    mov cx , cs
    mov dx , cs
    mov bx , cs

    mov es, ax
    mov es, cx
    mov es, dx
    mov es, bx
    mov es, ax
    mov es, cx
    mov es, dx
    mov es, bx

    mov eax , es
    mov ecx , es
    mov edx , es
    mov ebx , es
    mov eax , cs
    mov ecx , cs
    mov edx , cs
    mov ebx , cs

    mov es, eax
    mov es, ecx
    mov es, edx
    mov es, ebx
    mov es, eax
    mov es, ecx
    mov es, edx
    mov es, ebx

  RET
  CMC
  CLC
  STC
  CLI
  STI
  CLD
  STD
  SAHF
  LAHF
  PUSHF
  POPF

  PUSHFQ
  POPFQ

  CBW
  CWDE
  CWD
  CDQ

  CDQE
  CQO
  CLGI
  STGI

  ret
assemblerCode ENDP

END
