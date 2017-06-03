#pragma once

#ifdef IS64BIT

extern "C" void memSwap(void *p1, void *p2, size_t w);

#else IS64BIT

inline void memSwap(void *p1, void *p2, size_t w) {
  __asm {
    mov ecx, w
    mov eax, p1
    mov edi, p2
    cmp ecx, 4
    jb SwapWord 
LoopDword:
    mov	ebx, DWORD PTR [eax]
    mov	edx, DWORD PTR [edi]
    mov	DWORD PTR [edi], ebx
    mov	DWORD PTR [eax], edx
    sub ecx, 4
    cmp ecx, 4
    jb EndLoopDword
    add	eax, 4
    add	edi, 4
    jmp LoopDword
EndLoopDword:
    jecxz End
    add	eax, 4
    add	edi, 4
SwapWord:
    mov	ebx, DWORD PTR [eax]
    mov	edx, DWORD PTR [edi]
    cmp ecx, 2
    jb SwapByte
    mov	WORD PTR [edi], bx
    mov	WORD PTR [eax], dx
    sub ecx, 2
    jz End
    add	eax, 2
    add	edi, 2
    shr ebx, 16
    shr edx, 16
SwapByte:
    mov	BYTE PTR [edi], bl
    mov	BYTE PTR [eax], dl
End:
  }
}

#endif // IS64BIT
