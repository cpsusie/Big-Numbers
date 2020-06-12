#include "stdafx.h"

#if defined(IS64BIT)

extern "C" void _swap(void *p1, void *p2, size_t w);
#else
inline void _swap(void *p1, void *p2, size_t w) {
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
#endif

int main() {
  BYTE data1[256], data2[256];
#define BUFIZE sizeof(data1)
  for(int i = 0; i < BUFIZE; i++) {
    data1[i] = i;
    data2[i] = 255-i;
  }

  for (size_t i = 1; i <= BUFIZE; i++) {
    BYTE buf1[256], buf2[256];
    memcpy(buf1, data1, BUFIZE);
    memcpy(buf2, data2, BUFIZE);
    _swap(buf1, buf2, i);
    if(memcmp(buf1, data2, i)) {
      pause();
    }
    if(memcmp(buf2, data1, i)) {
      pause();
    }
    if(i < BUFIZE) {
      if(memcmp(buf1+i,data1+i,BUFIZE-i)) {
        pause();
      }
      if(memcmp(buf2+i,data2+i,BUFIZE-i)) {
        pause();
      }
    }
  }
  _tprintf(_T("all ok!\n"));
  return 0;
}

