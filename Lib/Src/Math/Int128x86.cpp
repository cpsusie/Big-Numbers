#include "pch.h"

// The code in this file is only used for x86 compilation.
// For x64 mode the file Int128x64.asm should be used instead.
// They execute much faster, because they use 64-bit registers,
// and division functions are coded entirely in assembler.

#if !defined(_M_X64)

#include <Math/Int128.h>

#define LO32(n) (n).s4.i[0]

void int128add( void *dst, const void *x) {
  __asm {
    mov         esi, x
    mov         edi, dst
    mov         eax, dword ptr[esi   ]
    add         dword ptr[edi   ], eax
    mov         eax, dword ptr[esi+ 4]
    adc         dword ptr[edi+ 4], eax
    mov         eax, dword ptr[esi+ 8]
    adc         dword ptr[edi+ 8], eax
    mov         eax, dword ptr[esi+12]
    adc         dword ptr[edi+12], eax
  }
}

void int128sub(void *dst, const void *x) {
  __asm {
    mov         esi, x
    mov         edi, dst
    mov         eax, dword ptr[esi   ]
    sub         dword ptr[edi   ], eax
    mov         eax, dword ptr[esi+ 4]
    sbb         dword ptr[edi+ 4], eax
    mov         eax, dword ptr[esi+ 8]
    sbb         dword ptr[edi+ 8], eax
    mov         eax, dword ptr[esi+12]
    sbb         dword ptr[edi+12], eax
  }
}

void int128inc(void *x) {
  __asm {
    mov         edi, x
    add         dword ptr[edi   ], 1
    jnc         Done
    adc         dword ptr[edi+ 4], 0
    adc         dword ptr[edi+ 8], 0
    adc         dword ptr[edi+12], 0
Done:
  }
}

void int128dec(void *x) {
  __asm {
    mov         edi, x
    sub         dword ptr[edi   ], 1
    jnc         Done
    sbb         dword ptr[edi+ 4], 0
    sbb         dword ptr[edi+ 8], 0
    sbb         dword ptr[edi+12], 0
Done:
  }
}

void int128neg(void *x) {
  __asm {
    mov         esi, x
    not         dword ptr[esi   ]
    not         dword ptr[esi+ 4]
    not         dword ptr[esi+ 8]
    not         dword ptr[esi+12]
    add         dword ptr[esi   ], 1
    jnc         Done
    adc         dword ptr[esi+ 4], 0
    adc         dword ptr[esi+ 8], 0
    adc         dword ptr[esi+12], 0
Done:
  }
}

// signed/unsigned shift left
void int128shl(int shft, void *x) {
  __asm {
    mov         ecx, shft                              ; ecx  = shift count
    mov         esi, x                                 ; esi  = x
    cmp         cl, 64                                 ;
    jae         GE64                                   ;
    cmp         cl, 32                                 ; Assume cl < 64
    jae         _32_63                                 ;
                                                       ; Assume cl < 32
    mov         eax, dword ptr[esi+ 8]                 ; eax  = x[2]
    shld        dword ptr[esi+12], eax, cl             ; shift x[3] adding bits from x[2]
    mov         eax, dword ptr[esi+ 4]                 ; eax  = x[1]
    shld        dword ptr[esi+ 8], eax, cl             ; shift x[2] adding bits from x[1]
    mov         eax, dword ptr[esi   ]                 ; eax  = x[0]
    shld        dword ptr[esi+ 4], eax, cl             ; shift x[1] adding bits from x[0]
    shl         dword ptr[esi   ]     , cl             ; x[0] <<= cl
    jmp         End                                    ;
_32_63 :                                               ; Assume 32 <= cl < 64
    and         cl, 1Fh                                ; cl %= 32
    mov         eax, dword ptr[esi+ 8]                 ; eax  = x[2]
    mov         dword ptr[esi+12], eax                 ; x[3] = x[2]
    mov         eax, dword ptr[esi+ 4]                 ; eax  = x[1]
    shld        dword ptr[esi+12], eax, cl             ; shift x[3] adding bits from x[1]
    mov         dword ptr[esi+ 8], eax                 ; x[2] = x[1]
    mov         eax, dword ptr[esi   ]                 ; eax  = x[0]
    shld        dword ptr[esi+ 8], eax, cl             ; shift x[2] adding bits from x[0]
    shl         eax                   , cl             ; eax  <<= cl
    mov         dword ptr[esi+ 4], eax                 ; x[1] = eax
    xor         eax, eax                               ; eax  = 0
    mov         dword ptr[esi   ], eax                 ; x[0] = eax
    jmp         End                                    ;
GE64:                                                  ; Assume cl >= 64
    cmp         cl, 96                                 ;
    jae         GE96                                   ;
    and         cl, 1Fh                                ; Assume 64 <= cl < 96; cl %= 32
    mov         eax, dword ptr[esi+ 4]                 ;
    mov         dword ptr[esi+12], eax                 ; x[3] = x[1]
    mov         eax, dword ptr[esi   ]                 ; eax  = x[0]
    shld        dword ptr[esi+12], eax, cl             ; shift x[3] adding bits from x[0]
    shl         eax                   , cl             ; eax  <<= cl
    mov         dword ptr[esi+ 8], eax                 ; x[2] = eax (=x[0] << cl)
    xor         eax, eax                               ; eax  = 0
    mov         dword ptr[esi   ], eax                 ; x[0..1] = eax
    mov         dword ptr[esi+ 4], eax                 ;
    jmp         End                                    ;
GE96:                                                  ; Assume cl >= 96
    cmp         cl, 128                                ;
    jae         GE128                                  ;
    and         cl, 1Fh                                ; Assume 96 <= cl < 128; cl %= 32
    mov         eax, dword ptr[esi   ]                 ; eax  = x[0]
    shl         eax                   , cl             ; eax  <<== cl
    mov         dword ptr[esi+12], eax                 ; x[3] = eax
    xor         eax, eax                               ; eax  = 0
    mov         dword ptr[esi   ], eax                 ; x[0..2] = eax
    mov         dword ptr[esi+ 4], eax                 ;
    mov         dword ptr[esi+ 8], eax                 ;
    jmp         End                                    ;
GE128:                                                 ; Assume cl >= 128
    xor         eax, eax                               ; eax  = 0
    mov         dword ptr[esi   ], eax                 ; x[0..3] = eax
    mov         dword ptr[esi+ 4], eax                 ;
    mov         dword ptr[esi+ 8], eax                 ;
    mov         dword ptr[esi+12], eax                 ;
  }
End:;
}

// signed shift right
void int128shr(int shft, void *x) {
  __asm {
    mov         ecx, shft                              ; ecx  = shift count
    mov         esi, x                                 ; esi  = x
    cmp         cl, 64                                 ;
    jae         GE64                                   ;
    cmp         cl, 32                                 ; Assume cl < 64
    jae         _32_63                                 ;
                                                       ; Assume cl < 32
    mov         eax, dword ptr[esi+ 4]                 ; eax  = x[1]
    shrd        dword ptr[esi   ], eax, cl             ; shift x[0] adding bits from x[1]
    mov         eax, dword ptr[esi+ 8]                 ; eax  = x[2]
    shrd        dword ptr[esi+ 4], eax, cl             ; shift x[1] adding bits from x[2]
    mov         eax, dword ptr[esi+12]                 ; eax  = x[3]
    shrd        dword ptr[esi+ 8], eax, cl             ; shift x[2] adding bits from x[3]
    sar         dword ptr[esi+12]     , cl             ; shift x[3] adding sign bit
    jmp         End                                    ;
_32_63 :                                               ; Assume 32 <= cl < 64
    and         cl, 1Fh                                ; cl %= 32
    mov         eax, dword ptr[esi+ 4]                 ;
    mov         dword ptr[esi   ], eax                 ; x[0] = x[1]
    mov         eax, dword ptr[esi+ 8]                 ; eax  = x[2]
    shrd        dword ptr[esi   ], eax, cl             ; shift x[0] adding bits from x[2]
    mov         dword ptr[esi+ 4], eax                 ; x[1] = x[2]
    mov         eax, dword ptr[esi+12]                 ; eax  = x[3]
    shrd        dword ptr[esi+ 4], eax, cl             ; shift x[1] adding bits from x[3]
    sar         eax                   , cl             ; eax  >>= cl adding sign bits
    mov         dword ptr[esi+ 8], eax                 ; x[2] = eax
    sar         eax                   , 1fh            ; all bits of eax = sign bit
    mov         dword ptr[esi+12]     , eax            ; x[3] = eax
    jmp         End                                    ;
GE64:                                                  ; Assume cl >= 64
    cmp         cl, 96                                 ;
    jae         GE96                                   ;
    and         cl, 1Fh                                ; Assume 64 <= cl < 96; cl %= 32
    mov         eax, dword ptr[esi+ 8]                 ;
    mov         dword ptr[esi   ], eax                 ; x[0] = x[2]
    mov         eax, dword ptr[esi+12]                 ; eax  = x[3]
    shrd        dword ptr[esi   ], eax, cl             ; shift x[0] adding bits from x[3]
    sar         eax                   , cl             ; eax  >>= cl adding sign bits
    mov         dword ptr[esi+ 4], eax                 ; x[1] = eax
    sar         eax                   , 1fh            ; all bits of eax = sign bit
    mov         dword ptr[esi+ 8], eax                 ; x[2..3] = eax
    mov         dword ptr[esi+12], eax                 ;
    jmp         End                                    ;
GE96:                                                  ; Assume cl >= 96
    cmp         cl, 128                                ;
    jae         GE128                                  ;
    and         cl, 1Fh                                ; Assume 96 <= cl < 128; cl %= 32
    mov         eax, dword ptr[esi+12]                 ; eax  = x[3]
    sar         eax                   , cl             ; eax  >>= cl adding sign bits
    mov         dword ptr[esi   ], eax                 ; x[0] = eax
    sar         eax                   , 1fh            ; all bits of eax = sign bit
    mov         dword ptr[esi+ 4], eax                 ; x[1..3] = eax
    mov         dword ptr[esi+ 8], eax                 ;
    mov         dword ptr[esi+12], eax                 ;
    jmp         End                                    ;
GE128:                                                 ; Assume cl >= 128
    mov         eax, dword ptr[esi+12]                 ;
    sar         eax                   ,1Fh             ; all bits of eax = sign bit
    mov         dword ptr[esi   ], eax                 ; x[0..3] = eax
    mov         dword ptr[esi+ 4], eax                 ;
    mov         dword ptr[esi+ 8], eax                 ;
    mov         dword ptr[esi+12], eax                 ;
  }
End:;
}

 // unsigned shift right
void uint128shr(int shft, void *x) {
  __asm {
    mov         ecx, shft                              ; ecx = shift count
    mov         esi, x                                 ; esi = x
    cmp         cl, 64                                 ;
    jae         GE64                                   ;
    cmp         cl, 32                                 ; Assume cl < 64
    jae         _32_63                                 ;
                                                       ; Assume cl < 32
    mov         eax, dword ptr[esi+ 4]                 ; eax  = x[1]
    shrd        dword ptr[esi   ], eax, cl             ; shift x[0] adding bits from x[1]
    mov         eax, dword ptr[esi+ 8]                 ; eax  = x[2]
    shrd        dword ptr[esi+ 4], eax, cl             ; shift x[1] adding bits from x[2]
    mov         eax, dword ptr[esi+12]                 ; eax  = x[3]
    shrd        dword ptr[esi+ 8], eax, cl             ; shift x[2] adding bits from x[3]
    shr         dword ptr[esi+12]     , cl             ; x[3] >>= cl adding 0-bits
    jmp         End                                    ;
_32_63 :                                               ; Assume 32 <= cl < 64
    and         cl, 1Fh                                ; cl %= 32
    mov         eax, dword ptr[esi+ 4]                 ;
    mov         dword ptr[esi   ], eax                 ; x[0] = x[1]
    mov         eax, dword ptr[esi+ 8]                 ; eax  = x[2]
    shrd        dword ptr[esi   ], eax, cl             ; shift x[0] adding bits from x[2]
    mov         dword ptr[esi+ 4], eax                 ; x[1] = x[2]
    mov         eax, dword ptr[esi+12]                 ; eax  = x[3]
    shrd        dword ptr[esi+ 4], eax, cl             ; shift x[1] adding bits from x[3]
    shr         eax                   , cl             ; eax  >>= cl adding 0-bits
    mov         dword ptr[esi+ 8], eax                 ; x[2] = eax
    xor         eax, eax                               ; eax  = 0
    mov         dword ptr[esi+12], eax                 ; x[3] = eax
    jmp         End                                    ;
GE64:                                                  ; Assume cl >= 64
    cmp         cl, 96                                 ;
    jae         GE96                                   ;
    and         cl, 1Fh                                ; Assume 64 <= cl < 96; cl %= 32
    mov         eax, dword ptr[esi+ 8]                 ;
    mov         dword ptr[esi   ], eax                 ; x[0] = x[2]
    mov         eax, dword ptr[esi+12]                 ; eax  = x[3]
    shrd        dword ptr[esi   ], eax, cl             ; shift x[0] adding bits from x[3]
    shr         eax                   , cl             ; eax  >>= cl adding 0-bits
    mov         dword ptr[esi+ 4], eax                 ; x[1] = eax (=x[3] >> cl)
    xor         eax, eax                               ; eax  = 0
    mov         dword ptr[esi+ 8], eax                 ; x[2..3] = eax
    mov         dword ptr[esi+12], eax                 ;
    jmp         End                                    ;
GE96:                                                  ; Assume cl >= 96
    cmp         cl, 128                                ;
    jae         GE128                                  ;
    and         cl, 1Fh                                ; Assume 96 <= cl < 128; cl %= 32
    mov         eax, dword ptr[esi+12]                 ; eax  = x[3]
    shr         eax                   , cl             ; eax  >>= cl adding 0-bits
    mov         dword ptr[esi   ], eax                 ; x[0] = eax
    xor         eax, eax                               ; eax  = 0
    mov         dword ptr[esi+ 4], eax                 ; x[1..3] = eax
    mov         dword ptr[esi+ 8], eax                 ;
    mov         dword ptr[esi+12], eax                 ;
    jmp         End                                    ;
GE128:                                                 ; Assume cl >= 128
    xor         eax, eax                               ; eax  = 0
    mov         dword ptr[esi   ], eax                 ; x[0..3] = eax
    mov         dword ptr[esi+ 4], eax                 ;
    mov         dword ptr[esi+ 8], eax                 ;
    mov         dword ptr[esi+12], eax                 ;
  }
End:;
}

// Return e = (n==0)?0:floor(log2(n)), e=[0..31]
static inline int getExpo2(UINT x) {
  UINT result;
  _asm {
    bsr         eax, x
    mov         result, eax
  }
  return result;
}

// Return e = (n==0)?0:floor(log2(n)), e=[0..127]
static int getExpo2(const _uint128 &n) {
  UINT result;
  __asm {
    pushf                                              ;
    mov         ecx, 4                                 ;
    mov         edi, n                                 ; edi = &n
    add         edi, 12                                ; edi = &n[3]
    xor         eax, eax                               ;
    std                                                ; make repe scasd search backwards
    repe        scasd                                  ;
    jz          End                                    ;
    bsr         eax, dword ptr[edi+4]                  ; eax = index of leftmost 1 bit
    shl         ecx, 5                                 ; ecx *= 32
    add         eax, ecx                               ;
End:                                                   ;
    mov         result, eax                            ;
    popf                                               ;
  }
  return result;
}

// dst and x are both _uint128
// dwordIndex(_uint128 n) is the highest index,i = [0..3], so n[i] != 0
// return (dwordIndex(dst)<<2) | dwordIndex(x), range:[0..15]
static inline BYTE getMultCode(const void *dst, const void *x) {
  BYTE result;
  __asm {
    pushf                                              ;
    mov         ecx, 4                                 ;
    mov         edi, x                                 ; edi = &x
    add         edi, 12                                ; edi = &x[3]
    xor         eax, eax                               ;
    std                                                ; make repe scasd search backwards
    repe        scasd                                  ;
    mov         result, cl                             ;
                                                       ;
    mov         cl, 4                                  ;
    mov         edi, dst                               ; edi = &dst
    add         edi, 12                                ; edi = &dst[3]
    repe        scasd                                  ;
    shl         cl, 2                                  ;
    or          result, cl                             ;
    popf                                               ;
  }
  return result;
}

// Set n32 = (scale==0)?LO32(n) : (n >> scale); where scale = (max(0,expo2(n) - 31) (=[0..127-31]=[0..96])
// Return scale
static int getFirst32(const _uint128 &n, UINT &n32) {
  const int scale = getExpo2(n) - 31;
  if(scale <= 0) {
    n32 = LO32(n);
    return 0;
  } else {
    n32 = (UINT)(n >> scale);
    return scale;
  }
}

// Set n63 = (scale==0)?LO64(n) : (n >> scale); where scale = (max(0,expo2(n) - 62) (=[0..127-62]=[0..65])
// Return scale
static int getFirst63(const _uint128 &n, UINT64 &n63) {
  const int scale = getExpo2(n) - 62;
  if(scale <= 0) {
    n63 = LO64(n);
    return 0;
  } else {
    n63 = (UINT64)(n >> scale);
    return scale;
  }
}

// dst and x are _uint128
static void mulU32(void *dst, UINT y, const void *x) {
  _asm {
    mov         edi, dst                               ; edi = &dst
    mov         esi, x                                 ; esi = &x
    mov         eax, dword ptr[esi  ]                  ;
    mul         y                                      ;
    mov         dword ptr[edi   ], eax                 ;
    mov         dword ptr[edi+ 4], edx                 ;
    mov         eax, dword ptr[esi+4]                  ;
    mul         y                                      ;
    add         dword ptr[edi+ 4], eax                 ;
    mov         dword ptr[edi+ 8], edx                 ;
    adc         dword ptr[edi+ 8], 0                   ;
    mov         eax, dword ptr[esi+ 8]                 ;
    mul         y                                      ;
    add         dword ptr[edi+ 8], eax                 ;
    mov         dword ptr[edi+12], edx                 ;
    adc         dword ptr[edi+12], 0                   ;
    mov         eax, dword ptr[esi+12]                 ;
    mul         y                                      ;
    add         dword ptr[edi+12], eax                 ;
  }
}

#pragma warning(disable:4731) // warning C4731: 'int128mul': frame pointer register 'ebp' modified by inline assembly code

void int128mul(void *dst, const void *x) {
  switch(getMultCode(dst,x)) {
  case 0 : // both max 32 bit int
    HI64(*(_uint128*)dst) = 0;
    LO64(*(_uint128*)dst) = __int64(*(UINT*)dst) * (*(UINT*)x); // simple _int64 multiplication. int32 * int32
    break;
  case 1 : // 0,1 dst max 32 bit
  case 2 : // 0,2
  case 3 : // 0,3
    mulU32(dst, *(UINT*)dst, x);
    break;
  case 4 : // 1,0 x max 32 bit
  case 8 : // 2,0
  case 12: // 3,0
    { const _uint128 copy(*(_uint128*)dst);
      mulU32(dst, *(UINT*)x, &copy);
    }
    break;
  default:
    { const _uint128 a(*(_uint128*)dst);
      __asm {
        push    ebp                                    ;
        lea     ebx, a                                 ; ebx = &a
        mov     ecx, x                                 ; ecx = &x
        mov     ebp, dst                               ; ebp = dst
        xor     eax, eax                               ;
        mov     dword ptr[ebp+ 8], eax                 ; dst[2..3] = 0
        mov     dword ptr[ebp+12], eax                 ;
        mov     eax, dword ptr[ebx   ]                 ; 1. round
        mul     dword ptr[ecx   ]                      ; edx:eax = x[0]*y[0]
        mov     dword ptr[ebp   ], eax                 ;
        mov     dword ptr[ebp+ 4], edx                 ; dst[0:1] = edx:eax
                                                       ;
        mov     eax, dword ptr[ebx   ]                 ; 2. round
        mul     dword ptr[ecx+ 4]                      ; edx:eax = x[0]*y[1]
        mov     esi, eax                               ;
        mov     edi, edx                               ; edi:esi = edx:eax
        mov     eax, dword ptr[ebx+ 4]                 ;
        mul     dword ptr[ecx   ]                      ; edx:eax = x[1]*y[0]
        add     esi, eax                               ;
        adc     edi, edx                               ; edi:esi += edx:eax
        adc     dword ptr[ebp+12], 0                   ; maybe extra carry for dst[3]
        add     dword ptr[ebp+ 4], esi                 ;
        adc     dword ptr[ebp+ 8], edi                 ; dst[1:2] += edi:esi
                                                       ;
        mov     eax, dword ptr[ebx  ]                  ; 3. round. Extra carries are overflow
        mul     dword ptr[ecx+ 8]                      ; edx:eax = x[0]*y[2]
        mov     esi, eax                               ;
        mov     edi, edx                               ; edi:esi = edx:eax
        mov     eax, dword ptr[ebx+ 4]                 ;
        mul     dword ptr[ecx+ 4]                      ; edx:eax = x[1]*y[1]
        add     esi, eax                               ;
        adc     edi, edx                               ; edi:esi += edx:eax
        mov     eax, dword ptr[ebx+ 8]                 ;
        mul     dword ptr[ecx   ]                      ; edx:eax = x[2]*y[0]
        add     esi, eax                               ;
        adc     edi, edx                               ; edi:esi += edx:eax
        add     dword ptr[ebp+ 8], esi                 ;
        adc     dword ptr[ebp+12], edi                 ; dst[2:3] += edi:esi
                                                       ;
        mov     eax, dword ptr[ebx   ]                 ; 4. round. Highend and carries are overflow
        mul     dword ptr[ecx+12]                      ; edx:eax] = x[0]*y[3]
        mov     esi, eax                               ; esi = eax
        mov     eax, dword ptr[ebx+ 4]                 ;
        mul     dword ptr[ecx+ 8]                      ; edx:eax = x[1]*y[2]
        add     esi, eax                               ; esi += eax
        mov     eax, dword ptr[ebx+ 8]                 ;
        mul     dword ptr[ecx+ 4]                      ; edx:eax = x[2]*y[1]
        add     esi, eax                               ; esi += eax
        mov     eax, dword ptr[ebx+12]                 ;
        mul     dword ptr[ecx   ]                      ; edx:eax = x[3]*y[0]
        add     esi, eax                               ; esi += eax
        add     dword ptr[ebp+12], esi                 ; dst[3] += esi
                                                       ;
        pop     ebp                                    ;
      }
    }
  }
}

static void unsignedQuotRemainderU32(const _uint128 &x, UINT y, _uint128 *quot, _uint128 &rem) {
  const int xExpo2 = getExpo2(x);
  if(xExpo2 <= 31) {
    rem = LO32(x) % y;
    if(quot) {
      *quot = LO32(x) / y;
    }
  } else if(xExpo2 <= 63) {
    rem = LO64(x) % y;
    if(quot) {
      *quot = LO64(x) / y;
    }
  } else {
    _uint128 tmp, &q = quot ? *quot : tmp;
    UINT     r32;
    __asm {
      mov       ebx, x                                 ; ebx     = &x
      mov       ecx, q                                 ; ecx     = &q
      xor       edx, edx                               ;
      mov       eax, dword ptr[ebx+12]                 ; edx:eax = 0:x[3]
      div       y                                      ; eax:edx = quotient:remainder
      mov       dword ptr[ecx+12], eax                 ; save quot[3]
      mov       eax, dword ptr[ebx+ 8]                 ; edx:eax = last remainder:x[2]
      div       y                                      ; eax:edx = quotient:remainder
      mov       dword ptr[ecx+ 8], eax                 ; save quot[2]
      mov       eax, dword ptr[ebx+ 4]                 ; edx:eax = last remainder:x[1]
      div       y                                      ; eax:edx = quotient:remainder
      mov       dword ptr[ecx+ 4], eax                 ; save quot[1]
      mov       eax, dword ptr[ebx   ]                 ; edx:eax = last remainder:x[0]
      div       y                                      ; eax:edx = quotient:remainder
      mov       dword ptr[ecx   ], eax                 ; save quot[0]
      mov       r32, edx                               ; save remainder
      mul       y                                      ; edx:eax = last quot * y
      cmp       eax, dword ptr[ebx]                    ;
      jae       End                                    ; if(eax < x[0]) {
      sub       eax, dword ptr[ebx]                    ;
      neg       eax                                    ;   remainder = x[0] - eax
      mov       r32, eax                               ; }
    }
End:
    rem = r32;
  }
}

static void unsignedQuotRemainder(const _uint128 &x, const _uint128 &y, _uint128 *quot, _uint128 &rem) {
  UINT      y32;
  const int yScale = getFirst32(y, y32);
  if(yScale == 0) {
    unsignedQuotRemainderU32(x,y32,quot,rem);
  } else {
    rem = x;
    if(quot) *quot = 0;

    int lastShift = 0;
    for(int count = 0; rem >= y;) {
      UINT64         rem63;                            // max 63 bits to prevent overflow in division
      int            shift = getFirst63(rem, rem63) - yScale;
      UINT           q32;
      __asm {
        lea     eax              , rem63               ;
        mov     edx              , dword ptr[eax+4]    ;
        mov     eax              , dword ptr[eax]      ; edx:eax = rem63
        div     y32                                    ; eax = rem63/y32, ignore remainder. See comment below
        mov     ecx              , shift               ;
        cmp     ecx              , 0                   ;
        jge     SaveQ32                                ; if(shift < 0) {
        neg     ecx                                    ;
        shr     eax              , cl                  ;   eax = (rem63 / y32) >> -shift
        xor     ecx              , ecx                 ;
        mov     shift            , ecx                 ;   shift = 0
        cmp     eax              , 1                   ;
        ja      SaveQ32                                ;   if(eax <= 1) {
        mov     q32              , 1                   ;     q32 = 1;
        mov     edx              , rem                 ;     edx = &rem
        mov     ecx              , y                   ;     ecx = &y
        mov     eax              , dword ptr[ecx   ]   ;     rem -= y
        sub     dword ptr[edx   ], eax                 ;     we know y <= rem => rem-y >= 0
        mov     eax              , dword ptr[ecx+ 4]   ;
        sbb     dword ptr[edx+ 4], eax                 ;
        mov     eax              , dword ptr[ecx+ 8]   ;
        sbb     dword ptr[edx+ 8], eax                 ;
        mov     eax              , dword ptr[ecx+12]   ;
        sbb     dword ptr[edx+12], eax                 ;
        jmp     UpdateQuot                             ;     skip multiplication, subtraction already done, update quot and we're done
SaveQ32:                                               ;   }
        mov     q32, eax                               ; }
      }
                                                       // Division never overflow.
                                                       // rem63<2^63 and y32>=2^31 =>
                                                       // rem63/y32 < 2^(63-31)=2^32, which is max 32 bits.
                                                       // if shift >= 0 we have:
                                                       //   y32<2^32,
                                                       //   shift(=remScale-yScale)>=0, yScale>0 =>
                                                       //   remScale(=expo2(rem)-62)>0 => expo2(rem)>=63
                                                       //   =>rem63>=2^62 => q32=rem63/y32 >= 2^(62-32)=2^30
                                                       //   which is at least 31 bits
                                                       // if shift < 0 and division followed by right shift <= 1, then
                                                       //   set q32=1 and skip multiplication, just subtract y
                                                       //   we know, that y <= rem (loop condition)
                                                       //   so q32 will be at least 1, and rem >= 0
                                                       //   but because we make div by rem63,y32, this may result in 0

      _uint128 p128;                                   // Assume: q32 > 1, shift >= 0
      mulU32(&p128, q32, &y);

      if(shift) {                                      //   shift > 0;
        const _uint128 tmprem = rem >> shift;
        while(p128 > tmprem) {                         // make sure p128 <= rem
          p128 -= y;                                   // so we don't turn rem negative
          q32--;
        }
        p128 <<= shift;
      } else {                                         //   shift == 0
        while(p128 > rem) {
          p128 -= y;
          q32--;
        }
      }
      rem -= p128;                                     // Assume: p128 <= rem
UpdateQuot:
      if(quot) {                                       // Do we want the quot. If its NULL there's no need to do this
        if(count++ == 0) {
          *quot = q32;
        } else {
          const int ds = lastShift - shift;
          if(ds) {
            *quot <<= ds;
          }
          __asm {
            mov eax, q32                               ; *quot += q32
            mov ecx, quot                              ;
            add dword ptr[ecx], eax                    ;
          }
        }
      }
      lastShift = shift;
    }
    if(lastShift && quot) {
      *quot <<= lastShift;
    }
  }
}

static void signedQuotRemainder(const _int128 &a, const _int128 &b, _int128 *quot, _int128 &rem) {
  const bool aNegative = a.isNegative();
  const bool bNegative = b.isNegative();
  if(!aNegative && !bNegative) {
    unsignedQuotRemainder(*(const _uint128*)&a, *(const _uint128*)&b, (_uint128*)quot, (_uint128&)rem);
  } else {
    unsignedQuotRemainder((_uint128)(aNegative?-a:a), (_uint128)(bNegative?-b:b), (_uint128*)quot, (_uint128&)rem);
    if(quot && (aNegative != bNegative)) int128neg(quot);
    if(aNegative) int128neg(&rem);
  }
}

// -----------------------------------------------------------------


void int128div(void *dst, void *x) {
  const _int128  a(*(_int128*)dst);
  _int128 rem;
  signedQuotRemainder(a, *(const _int128*)x, (_int128*)dst, rem);
}

void int128rem(void *dst, void *x) {
  const _int128  a(*(_int128*)dst);
  signedQuotRemainder(a, *(const _int128*)x, NULL, *(_int128*)dst);
}

void uint128div(void *dst, const void *x) {
  const _uint128  a(*(_uint128*)dst);
  _uint128 rem;
  unsignedQuotRemainder(a, *(const _uint128*)x, (_uint128*)dst, rem);
}

void uint128rem(void *dst, const void *x) {
  const _uint128  a(*(_uint128*)dst);
  unsignedQuotRemainder(a, *(const _uint128*)x, NULL, *(_uint128*)dst);
}

// qr type _ui128div_t. See below
void uint128quotrem(void *numer, void *denom, void *qr) {
  _ui128div_t *div_t = (_ui128div_t*)qr;
  unsignedQuotRemainder(*(_uint128*)numer, *(_uint128*)denom, &div_t->quot, div_t->rem);
}

int int128cmp(const void *x1, const void *x2) {
  int result;
  __asm {
    mov         esi, x1                                ;
    mov         edi, x2                                ;
    mov         eax, dword ptr[esi+12]                 ;
    cmp         eax, dword ptr[edi+12]                 ;
    jl          Lessthan                               ; signed compare of high int
    jg          Greaterthan                            ;
    mov         eax, dword ptr[esi+ 8]                 ;
    cmp         eax, dword ptr[edi+ 8]                 ;
    jb          Lessthan                               ; unsigned compare of the 3 others
    ja          Greaterthan                            ;
    mov         eax, dword ptr[esi+ 4]                 ;
    cmp         eax, dword ptr[edi+ 4]                 ;
    jb          Lessthan                               ;
    ja          Greaterthan                            ;
    mov         eax, dword ptr[esi   ]                 ;
    cmp         eax, dword ptr[edi   ]                 ;
    jb          Lessthan                               ;
    ja          Greaterthan                            ;
    mov         result, 0                              ; they are equal
    jmp         End                                    ;
Greaterthan:                                           ;
    mov         result, 1                              ;
    jmp         End                                    ;
LessThan:                                              ;
    mov         result, -1                             ;
  }
End:
  return result;
}

int uint128cmp(const void *x1, const void *x2) {
  int result;
  __asm {
    mov         esi, x1                                ;
    mov         edi, x2                                ;
    mov         eax, dword ptr[esi+12]                 ;
    cmp         eax, dword ptr[edi+12]                 ;
    jb          Lessthan                               ; unsigned compare of all integers
    ja          Greaterthan                            ;
    mov         eax, dword ptr[esi+ 8]                 ;
    cmp         eax, dword ptr[edi+ 8]                 ;
    jb          Lessthan                               ;
    ja          Greaterthan                            ;
    mov         eax, dword ptr[esi+ 4]                 ;
    cmp         eax, dword ptr[edi+ 4]                 ;
    jb          Lessthan                               ;
    ja          Greaterthan                            ;
    mov         eax, dword ptr[esi   ]                 ;
    cmp         eax, dword ptr[edi   ]                 ;
    jb          Lessthan                               ;
    ja          Greaterthan                            ;
    mov         result, 0                              ; they are equal
    jmp         End                                    ;
Greaterthan:                                           ;
    mov         result, 1                              ;
    jmp         End                                    ;
LessThan:                                              ;
    mov         result, -1                             ;
  }
End:
  return result;
}

#endif // _M_X64
