.CODE

;void assemblerCode()
assemblerCode PROC
    ret

    nop
    imul al
    imul cl
    imul dl
    imul bl
    imul r8b
    imul r15b
    imul byte ptr[rax]
    imul byte ptr[rcx]
    imul byte ptr[rdx]
    imul byte ptr[rbx]
    imul ax
    imul cx
    imul dx
    imul bx
    imul r8w
    imul r15w
    imul word ptr[rax]
    imul word ptr[rcx]
    imul word ptr[rdx]
    imul word ptr[rbx]
    imul eax
    imul ecx
    imul edx
    imul ebx
    imul rax
    imul r8d
    imul r15d
    imul dword ptr[rax]
    imul dword ptr[rcx]
    imul dword ptr[rdx]
    imul dword ptr[rbx]
    imul rcx
    imul rdx
    imul rbx
    imul r8
    imul r15
    imul qword ptr[rax]
    imul qword ptr[rcx]
    imul qword ptr[rdx]
    imul qword ptr[rbx]
    nop
    imul  ax ,  ax
    imul  ax ,  cx
    imul  ax ,  dx
    imul  ax ,  bx
    imul  cx ,  ax
    imul  cx ,  cx
    imul  cx ,  dx
    imul  cx ,  bx
    imul  dx ,  ax
    imul  dx ,  cx
    imul  dx ,  dx
    imul  dx ,  bx
    imul  bx ,  ax
    imul  bx ,  cx
    imul  bx ,  dx
    imul  bx ,  bx
    imul  ax , word ptr[rax]
    imul  ax , word ptr[rcx]
    imul  ax , word ptr[rdx]
    imul  ax , word ptr[rbx]
    imul  cx , word ptr[rax]
    imul  cx , word ptr[rcx]
    imul  cx , word ptr[rdx]
    imul  cx , word ptr[rbx]
    imul  dx , word ptr[rax]
    imul  dx , word ptr[rcx]
    imul  dx , word ptr[rdx]
    imul  dx , word ptr[rbx]
    imul  bx , word ptr[rax]
    imul  bx , word ptr[rcx]
    imul  bx , word ptr[rdx]
    imul  ax , word ptr[rbx]
    nop
    imul eax , eax
    imul eax , ecx
    imul eax , edx
    imul eax , ebx
    imul ecx , eax
    imul ecx , ecx
    imul ecx , edx
    imul ecx , ebx
    imul edx , eax
    imul edx , ecx
    imul edx , edx
    imul ebx , ebx
    imul ebx , eax
    imul ebx , ecx
    imul ebx , edx
    imul ebx , ebx
    imul eax ,dword ptr[rax]
    imul eax ,dword ptr[rcx]
    imul eax ,dword ptr[rdx]
    imul eax ,dword ptr[rbx]
    imul ecx ,dword ptr[rax]
    imul ecx ,dword ptr[rcx]
    imul ecx ,dword ptr[rdx]
    imul ecx ,dword ptr[rbx]
    imul edx ,dword ptr[rax]
    imul edx ,dword ptr[rcx]
    imul edx ,dword ptr[rdx]
    imul edx ,dword ptr[rbx]
    imul ebx ,dword ptr[rax]
    imul ebx ,dword ptr[rcx]
    imul ebx ,dword ptr[rdx]
    imul ebx ,dword ptr[rbx]
    nop
    imul rax , rax
    imul rax , rcx
    imul rax , rdx
    imul rax , rbx
    imul rcx , rax
    imul rcx , rcx
    imul rcx , rdx
    imul rcx , rbx
    imul rdx , rax
    imul rdx , rcx
    imul rdx , rdx
    imul rbx , rbx
    imul rbx , rax
    imul rbx , rcx
    imul rbx , rdx
    imul rbx , rbx
    imul rax ,qword ptr[rax]
    imul rax ,qword ptr[rcx]
    imul rax ,qword ptr[rdx]
    imul rax ,qword ptr[rbx]
    imul rcx ,qword ptr[rax]
    imul rcx ,qword ptr[rcx]
    imul rcx ,qword ptr[rdx]
    imul rcx ,qword ptr[rbx]
    imul rdx ,qword ptr[rax]
    imul rdx ,qword ptr[rcx]
    imul rdx ,qword ptr[rdx]
    imul rdx ,qword ptr[rbx]
    imul rbx ,qword ptr[rax]
    imul rbx ,qword ptr[rcx]
    imul rbx ,qword ptr[rdx]
    imul rbx ,qword ptr[rbx]
    nop
    imul  ax , 7fh
    imul  cx , 7fh
    imul  dx , 7fh
    imul  bx , 7fh
    imul eax , 7fh
    imul ecx , 7fh
    imul edx , 7fh
    imul ebx , 7fh
    imul eax , 7fffffffh
    imul ecx , 7fffffffh
    imul edx , 7fffffffh
    imul ebx , 7fffffffh
    imul rax , 7fh
    imul rcx , 7fh
    imul rdx , 7fh
    imul rbx , 7fh
    imul rax , 7fffffffh
    imul rcx , 7fffffffh
    imul rdx , 7fffffffh
    imul rbx , 7fffffffh
    nop
    imul  ax ,  ax          ,7fh
    imul  ax ,  cx          ,7fh
    imul  ax ,  dx          ,7fh
    imul  ax ,  bx          ,7fh
    imul  cx ,  ax          ,7fh
    imul  cx ,  cx          ,7fh
    imul  cx ,  dx          ,7fh
    imul  cx ,  bx          ,7fh
    imul  dx ,  ax          ,7fh
    imul  dx ,  cx          ,7fh
    imul  dx ,  dx          ,7fh
    imul  dx ,  bx          ,7fh
    imul  bx ,  ax          ,7fh
    imul  bx ,  cx          ,7fh
    imul  bx ,  dx          ,7fh
    imul  bx ,  bx          ,7fh
    imul  ax ,  ax          ,7fffh
    imul  ax ,  cx          ,7fffh
    imul  ax ,  dx          ,7fffh
    imul  ax ,  bx          ,7fffh
    imul  cx ,  ax          ,7fffh
    imul  cx ,  cx          ,7fffh
    imul  cx ,  dx          ,7fffh
    imul  cx ,  bx          ,7fffh
    imul  dx ,  ax          ,7fffh
    imul  dx ,  cx          ,7fffh
    imul  dx ,  dx          ,7fffh
    imul  dx ,  bx          ,7fffh
    imul  bx ,  ax          ,7fffh
    imul  bx ,  cx          ,7fffh
    imul  bx ,  dx          ,7fffh
    imul  bx ,  bx          ,7fffh
    imul  ax , word ptr[rax],7fh
    imul  ax , word ptr[rcx],7fh
    imul  ax , word ptr[rdx],7fh
    imul  ax , word ptr[rbx],7fh
    imul  cx , word ptr[rax],7fh
    imul  cx , word ptr[rcx],7fh
    imul  cx , word ptr[rdx],7fh
    imul  cx , word ptr[rbx],7fh
    imul  dx , word ptr[rax],7fh
    imul  dx , word ptr[rcx],7fh
    imul  dx , word ptr[rdx],7fh
    imul  dx , word ptr[rbx],7fh
    imul  bx , word ptr[rax],7fh
    imul  bx , word ptr[rcx],7fh
    imul  bx , word ptr[rdx],7fh
    imul  bx , word ptr[rbx],7fh
    imul  ax , word ptr[rax],7fffh
    imul  ax , word ptr[rcx],7fffh
    imul  ax , word ptr[rdx],7fffh
    imul  ax , word ptr[rbx],7fffh
    imul  cx , word ptr[rax],7fffh
    imul  cx , word ptr[rcx],7fffh
    imul  cx , word ptr[rdx],7fffh
    imul  cx , word ptr[rbx],7fffh
    imul  dx , word ptr[rax],7fffh
    imul  dx , word ptr[rcx],7fffh
    imul  dx , word ptr[rdx],7fffh
    imul  dx , word ptr[rbx],7fffh
    imul  bx , word ptr[rax],7fffh
    imul  bx , word ptr[rcx],7fffh
    imul  bx , word ptr[rdx],7fffh
    imul  bx , word ptr[rbx],7fffh
    nop
    imul eax , eax          ,7fh
    imul eax , ecx          ,7fh
    imul eax , edx          ,7fh
    imul eax , ebx          ,7fh
    imul ecx , eax          ,7fh
    imul ecx , ecx          ,7fh
    imul ecx , edx          ,7fh
    imul ecx , ebx          ,7fh
    imul edx , eax          ,7fh
    imul edx , ecx          ,7fh
    imul edx , edx          ,7fh
    imul edx , ebx          ,7fh
    imul ebx , eax          ,7fh
    imul ebx , ecx          ,7fh
    imul ebx , edx          ,7fh
    imul ebx , ebx          ,7fh
    imul eax ,dword ptr[rax],7fh
    imul eax ,dword ptr[rcx],7fh
    imul eax ,dword ptr[rdx],7fh
    imul eax ,dword ptr[rbx],7fh
    imul ecx ,dword ptr[rax],7fh
    imul ecx ,dword ptr[rcx],7fh
    imul ecx ,dword ptr[rdx],7fh
    imul ecx ,dword ptr[rbx],7fh
    imul edx ,dword ptr[rax],7fh
    imul edx ,dword ptr[rcx],7fh
    imul edx ,dword ptr[rdx],7fh
    imul edx ,dword ptr[rbx],7fh
    imul ebx ,dword ptr[rax],7fh
    imul ebx ,dword ptr[rcx],7fh
    imul ebx ,dword ptr[rdx],7fh
    imul ebx ,dword ptr[rbx],7fh
    nop
    imul eax , eax          ,7fffffffh
    imul eax , ecx          ,7fffffffh
    imul eax , edx          ,7fffffffh
    imul eax , ebx          ,7fffffffh
    imul ecx , eax          ,7fffffffh
    imul ecx , ecx          ,7fffffffh
    imul ecx , edx          ,7fffffffh
    imul ecx , ebx          ,7fffffffh
    imul edx , eax          ,7fffffffh
    imul edx , ecx          ,7fffffffh
    imul edx , edx          ,7fffffffh
    imul edx , ebx          ,7fffffffh
    imul ebx , eax          ,7fffffffh
    imul ebx , ecx          ,7fffffffh
    imul ebx , edx          ,7fffffffh
    imul ebx , ebx          ,7fffffffh
    imul eax ,dword ptr[rax],7fffffffh
    imul eax ,dword ptr[rcx],7fffffffh
    imul eax ,dword ptr[rdx],7fffffffh
    imul eax ,dword ptr[rbx],7fffffffh
    imul ecx ,dword ptr[rax],7fffffffh
    imul ecx ,dword ptr[rcx],7fffffffh
    imul ecx ,dword ptr[rdx],7fffffffh
    imul ecx ,dword ptr[rbx],7fffffffh
    imul edx ,dword ptr[rax],7fffffffh
    imul edx ,dword ptr[rcx],7fffffffh
    imul edx ,dword ptr[rdx],7fffffffh
    imul edx ,dword ptr[rbx],7fffffffh
    imul ebx ,dword ptr[rax],7fffffffh
    imul ebx ,dword ptr[rcx],7fffffffh
    imul ebx ,dword ptr[rdx],7fffffffh
    imul ebx ,dword ptr[rbx],7fffffffh
    nop
    imul rax , rax          ,7fh
    imul rax , rcx          ,7fh
    imul rax , rdx          ,7fh
    imul rax , rbx          ,7fh
    imul rcx , rax          ,7fh
    imul rcx , rcx          ,7fh
    imul rcx , rdx          ,7fh
    imul rcx , rbx          ,7fh
    imul rdx , rax          ,7fh
    imul rdx , rcx          ,7fh
    imul rdx , rdx          ,7fh
    imul rdx , rbx          ,7fh
    imul rbx , rax          ,7fh
    imul rbx , rcx          ,7fh
    imul rbx , rdx          ,7fh
    imul rbx , rbx          ,7fh
    imul rax ,qword ptr[rax],7fh
    imul rax ,qword ptr[rcx],7fh
    imul rax ,qword ptr[rdx],7fh
    imul rax ,qword ptr[rbx],7fh
    imul rcx ,qword ptr[rax],7fh
    imul rcx ,qword ptr[rcx],7fh
    imul rcx ,qword ptr[rdx],7fh
    imul rcx ,qword ptr[rbx],7fh
    imul rdx ,qword ptr[rax],7fh
    imul rdx ,qword ptr[rcx],7fh
    imul rdx ,qword ptr[rdx],7fh
    imul rdx ,qword ptr[rbx],7fh
    imul rbx ,qword ptr[rax],7fh
    imul rbx ,qword ptr[rcx],7fh
    imul rbx ,qword ptr[rdx],7fh
    imul rbx ,qword ptr[rbx],7fh
    nop
    imul rax , rax          ,7fffffffh
    imul rax , rcx          ,7fffffffh
    imul rax , rdx          ,7fffffffh
    imul rax , rbx          ,7fffffffh
    imul rcx , rax          ,7fffffffh
    imul rcx , rcx          ,7fffffffh
    imul rcx , rdx          ,7fffffffh
    imul rcx , rbx          ,7fffffffh
    imul rdx , rax          ,7fffffffh
    imul rdx , rcx          ,7fffffffh
    imul rdx , rdx          ,7fffffffh
    imul rdx , rbx          ,7fffffffh
    imul rbx , rax          ,7fffffffh
    imul rbx , rcx          ,7fffffffh
    imul rbx , rdx          ,7fffffffh
    imul rbx , rbx          ,7fffffffh
    imul rax ,qword ptr[rax],7fffffffh
    imul rax ,qword ptr[rcx],7fffffffh
    imul rax ,qword ptr[rdx],7fffffffh
    imul rax ,qword ptr[rbx],7fffffffh
    imul rcx ,qword ptr[rax],7fffffffh
    imul rcx ,qword ptr[rcx],7fffffffh
    imul rcx ,qword ptr[rdx],7fffffffh
    imul rcx ,qword ptr[rbx],7fffffffh
    imul rdx ,qword ptr[rax],7fffffffh
    imul rdx ,qword ptr[rcx],7fffffffh
    imul rdx ,qword ptr[rdx],7fffffffh
    imul rdx ,qword ptr[rbx],7fffffffh
    imul rbx ,qword ptr[rax],7fffffffh
    imul rbx ,qword ptr[rcx],7fffffffh
    imul rbx ,qword ptr[rdx],7fffffffh
    imul rbx ,qword ptr[rbx],7fffffffh
    nop



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
