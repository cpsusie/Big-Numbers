#pragma once

#ifdef IS64BIT

#pragma check_stack(off)

inline void memSwap(register char *p1, register char *p2, size_t w) {
#define _memSwapT(p1,p2,T) { const T tmp=*(T*)p1; *(T*)p1=*(T*)p2; *(T*)p2=tmp; }
#define _swapBasicType(if_or_while,type,w)  \
  if_or_while(w >= sizeof(type)) {          \
    _memSwapT(p1,p2,type)                   \
    w -= sizeof(type);                      \
    p1 += sizeof(type); p2 += sizeof(type); \
   }

  _swapBasicType(while,INT64,w)   /* take 8 bytes at a time */
  _swapBasicType(if   ,long ,w)   /* take 4 bytes at a time */
  _swapBasicType(if   ,short,w)   /* take 2 bytes at a time */
  _swapBasicType(if   ,char ,w)   /* take the last (if any) */
}

//extern "C" void memSwap(void *p1, void *p2, size_t w);

#else // IS64BIT

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
