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
    mov         eax, dword ptr[esi]
    add         dword ptr[edi], eax
    mov         eax, dword ptr[esi+4]
    adc         dword ptr[edi+4], eax
    mov         eax, dword ptr[esi+8]
    adc         dword ptr[edi+8], eax
    mov         eax, dword ptr[esi+12]
    adc         dword ptr[edi+12], eax
  }
}

void int128sub(void *dst, const void *x) {
  __asm {
    mov         esi, x
    mov         edi, dst
    mov         eax, dword ptr[esi]
    sub         dword ptr[edi], eax
    mov         eax, dword ptr[esi+4]
    sbb         dword ptr[edi+4], eax
    mov         eax, dword ptr[esi+8]
    sbb         dword ptr[edi+8], eax
    mov         eax, dword ptr[esi+12]
    sbb         dword ptr[edi+12], eax
  }
}

void int128inc(void *x) {
  __asm {
    mov         edi, x
    add         dword ptr[edi], 1
    jnc         Done
    adc         dword ptr[edi+4], 0
    adc         dword ptr[edi+8], 0
    adc         dword ptr[edi+12], 0
Done:
  }
}

void int128dec(void *x) {
  __asm {
    mov         edi, x
    sub         dword ptr[edi], 1
    jnc         Done
    sbb         dword ptr[edi+4], 0
    sbb         dword ptr[edi+8], 0
    sbb         dword ptr[edi+12], 0
Done:
  }
}

void int128shl(int shft, void *x) {
  __asm {
    mov         ecx, shft                      ; ecx = shift count
    mov         esi, x                         ; esi = x
    cmp         cl, 64
    jae         GE64
    cmp         cl, 32                         ; cl < 64
    jae         _32_63
    mov         eax, dword ptr[esi+8]          ; cl < 32
    shld        dword ptr[esi+12], eax, cl     ; shift x.dword[3] adding bits from x.dword[2]
    mov         eax, dword ptr[esi+4]
    shld        dword ptr[esi+8], eax, cl      ; shift x.dword[2] adding bits from x.dword[1]
    mov         eax, dword ptr[esi]
    shld        dword ptr[esi+4], eax, cl      ; shift x.dword[1] adding bits from x.dword[0]
    shl         dword ptr[esi], cl             ; shift x.dword[0]
    jmp         End

_32_63 :                                       ; 32 <= cl < 64
    and         cl, 1Fh                        ; cl %= 32
    mov         eax, dword ptr[esi+8]
    mov         dword ptr[esi+12], eax         ; x.dword[3] = x.dword[1]
    mov         eax, dword ptr[esi+4]
    shld        dword ptr[esi+12], eax, cl     ; shift x.dword[3] adding bits eax
    mov         dword ptr[esi+8], eax          ; x.dword[2] = x.dword[1]
    mov         eax, dword ptr[esi]            ; eax = x.dword[0]
    shld        dword ptr[esi+8], eax, cl      ; shift x.dword[2] adding bits eax
    shl         eax, cl
    mov         dword ptr[esi+4], eax          ; shift x.dword[1] eax << cl (old x.dword[0] << cl)
    xor         eax, eax
    mov         dword ptr[esi], eax            ; x.dword[0] = 0
    jmp         End

GE64:                                          ; cl >= 64
    cmp         cl, 96
    jae         GE96
    and         cl, 1Fh                        ; cl < 96; cl %= 32
    mov         eax, dword ptr[esi+4]
    mov         dword ptr[esi+12], eax         ; x.dword[3] = x.dword[1]
    mov         eax, dword ptr[esi]            ; eax = x.dword[0]
    shld        dword ptr[esi+12], eax, cl     ; shift x.dword[3] adding bits eax
    shl         eax, cl
    mov         dword ptr[esi+8], eax          ; shift x.dword[2] eax << cl
    xor         eax, eax
    mov         dword ptr[esi], eax            ; x.dword[0] = 0
    mov         dword ptr[esi+4], eax          ; x.dword[1] = 0
    jmp         End

GE96:                                          ; cl >= 96
    cmp         cl, 128
    jae         GE128
    and         cl, 1Fh                        ; cl < 128; cl %= 32
    mov         eax, dword ptr[esi]
    shl         eax, cl
    mov         dword ptr[esi+12], eax         ; x.dword[3] = x.dword[0] << cl
    xor         eax, eax
    mov         dword ptr[esi], eax            ; x.dword[0] = 0
    mov         dword ptr[esi+4], eax          ; x.dword[1] = 0
    mov         dword ptr[esi+8], eax          ; x.dword[2] = 0
    jmp         End

GE128:                                         ; cl >= 128
    xor         eax, eax                       ; return 0
    mov         dword ptr[esi   ], eax
    mov         dword ptr[esi+4 ], eax
    mov         dword ptr[esi+8 ], eax
    mov         dword ptr[esi+12], eax
  }
End:;
}

void int128shr(int shft, void *x) {            // signed shift right
  __asm {
    mov         ecx, shft                      ; ecx = shift count
    mov         esi, x                         ; esi = x
    cmp         cl, 64
    jae         GE64
    cmp         cl, 32                         ; cl < 64
    jae         _32_63
    mov         eax, dword ptr[esi+4]
    shrd        dword ptr[esi], eax, cl        ; shift x.dword[0] new bits from x.dword[1] (eax)
    mov         eax, dword ptr[esi+8]
    shrd        dword ptr[esi+4], eax, cl      ; shift x.dword[1] new bits from x.dword[2] (eax)
    mov         eax, dword ptr[esi+12]
    shrd        dword ptr[esi+8], eax, cl      ; shift x.dword[2] new bits from x.dword[3] (eax)
    sar         dword ptr[esi+12], cl          ; shift x.dword[3]
    jmp         End

_32_63 :                                       ; 32 <= cl < 64
    and         cl, 1Fh                        ; cl %= 32
    mov         eax, dword ptr[esi+4]
    mov         dword ptr[esi], eax            ; x.dword[0] = eax (old x.dword[1])
    mov         eax, dword ptr[esi+8]          ; eax = x.dword[2]
    shrd        dword ptr[esi], eax, cl        ; shift s[0] new bits from x.dword[2] (eax)
    mov         dword ptr[esi+4], eax          ; x.dword[1] = eax (old x.dword[2])
    mov         eax, dword ptr[esi+12]         ; eax = x.dword[3]
    shrd        dword ptr[esi+4], eax, cl      ; shift x.dword[1] new bits from eax (old x.dword[3])
    mov         dword ptr[esi+8], eax          ; x.dword[2] = eax (old x.dword[3])
    mov         eax, dword ptr[esi+12]         ; eax = x.dword[3]
    sar         eax, 1fh                       ; eax contain signbit in all bits
    shrd        dword ptr[esi+8], eax, cl      ; shift x.dword[1] new bits from eax (sign of x.dword[3])
    mov         dword ptr[esi+12], eax         ; x.dword[3] = eax
    jmp         End

GE64:                                          ; cl >= 64
    cmp         cl, 96
    jae         GE96
    and         cl, 1Fh                        ; cl < 96; cl %= 32
    mov         eax, dword ptr[esi+8]
    mov         dword ptr[esi], eax            ; x.dword[0] = x.dword[2]
    mov         eax, dword ptr[esi+12]         ; eax = x.dword[3]
    shrd        dword ptr[esi], eax, cl        ; shift x.dword[0] new bits from eax (old x.dword[3])
    sar         eax, cl                        ; eax contain sign bit in all bits
    mov         dword ptr[esi+4], eax          ; x.dword[1] = eax (old x.dword[3])
    sar         eax, 1fh
    mov         dword ptr[esi+8], eax          ; x.dword[2] = sign of x.dword[3]
    mov         dword ptr[esi+12], eax         ; x.dword[3] = sign of x.dword[3]
    jmp         End

GE96:                                          ; cl >= 96
    cmp         cl, 128
    jae         GE128
    and         cl, 1Fh                        ; cl < 128; cl %= 32
    mov         eax, dword ptr[esi+12]
    sar         eax, cl
    mov         dword ptr[esi], eax            ; x.dword[0] = x.dword[3] >> cl (shift in signbit of x.dword[3])
    sar         eax, 1fh
    mov         dword ptr[esi+4 ], eax         ; x.dword[1] = sign of x.dword[3]
    mov         dword ptr[esi+8 ], eax         ; x.dword[2] = sign of x.dword[3]
    mov         dword ptr[esi+12], eax         ; x.dword[3] = sign of x.dword[3]
    jmp         End

GE128:                                         ; cl >= 128
    mov         eax, dword ptr[esi+12]
    sar         eax,1Fh
    mov         dword ptr[esi   ], eax
    mov         dword ptr[esi+4 ], eax
    mov         dword ptr[esi+8 ], eax
    mov         dword ptr[esi+12], eax
  }
End:;
}

void uint128shr(int shft, void *x) { // unsigned shift right
  __asm {
    mov         ecx, shft                      ; ecx = shift count
    mov         esi, x
    cmp         cl, 64
    jae         GE64
    cmp         cl, 32                         ; cl < 64
    jae         _32_63
    mov         eax, dword ptr[esi+4]
    shrd        dword ptr[esi], eax, cl       ; shift x.dword[0] new bits from x.dword[1] (eax)
    mov         eax, dword ptr[esi+8]
    shrd        dword ptr[esi+4], eax, cl     ; shift x.dword[1] new bits from x.dword[2] (eax)
    mov         eax, dword ptr[esi+12]
    shrd        dword ptr[esi+8], eax, cl     ; shift x.dword[2] new bits from x.dword[3] (eax)
    shr         dword ptr[esi+12], cl         ; shift x.dword[3]
    jmp         End

_32_63 :                                      ; 32 <= cl < 64
    and         cl, 1Fh                       ; cl %= 32
    mov         eax, dword ptr[esi+4]
    mov         dword ptr[esi], eax           ; x.dword[0] = eax (old x.dword[1])
    mov         eax, dword ptr[esi+8]         ; eax = x.dword[2]
    shrd        dword ptr[esi], eax, cl       ; shift s[0] new bits from x.dword[2] (eax)
    mov         dword ptr[esi+4], eax         ; x.dword[1] = eax (old x.dword[2])
    mov         eax, dword ptr[esi+12]        ; eax = x.dword[3]
    shrd        dword ptr[esi+4], eax, cl     ; shift x.dword[1] new bits from eax (old x.dword[3])
    shr         eax, cl
    mov         dword ptr[esi+8], eax         ; x.dword[2] = eax >> cl
    xor         eax, eax
    mov         dword ptr[esi+12], eax        ; x.dword[3] = 0
    jmp         End

GE64:                                         ; cl >= 64
    cmp         cl, 96
    jae         GE96
    and         cl, 1Fh                       ; cl < 96; cl %= 32
    mov         eax, dword ptr[esi+8]
    mov         dword ptr[esi], eax           ; x.dword[0] = x.dword[2]
    mov         eax, dword ptr[esi+12]        ; eax = x.dword[3]
    shrd        dword ptr[esi], eax, cl       ; shift x.dword[0] new bits from eax (old x.dword[3])
    shr         eax, cl
    mov         dword ptr[esi+4], eax         ; x.dword[1] = eax >> cl (old x.dword[3] >> cl)
    xor         eax, eax
    mov         dword ptr[esi+8], eax         ; x.dword[2] = 0
    mov         dword ptr[esi+12], eax        ; x.dword[3] = 0
    jmp         End

GE96:                                         ; cl >= 96
    cmp         cl, 128
    jae         GE128
    and         cl, 1Fh                       ; cl < 128; cl %= 32
    mov         eax, dword ptr[esi+12]
    shr         eax, cl
    mov         dword ptr[esi], eax           ; x.dword[0] = x.dword[3] >> cl
    xor         eax, eax
    mov         dword ptr[esi+4 ], eax        ; x.dword[1] = 0
    mov         dword ptr[esi+8 ], eax        ; x.dword[2] = 0
    mov         dword ptr[esi+12], eax        ; x.dword[3] = 0
    jmp         End

GE128:                                        ; cl >= 128
    xor         eax,eax                       ; return 0
    mov         dword ptr[esi   ], eax
    mov         dword ptr[esi+4 ], eax
    mov         dword ptr[esi+8 ], eax
    mov         dword ptr[esi+12], eax
  }
End:;
}

static inline int getExpo2(UINT x) {
  UINT result;
  _asm {
    bsr         eax, x
    mov         result, eax
  }
  return result;
}

static int getExpo2(const _uint128 &n) {
  UINT result;
  __asm {
    pushf
    mov         ecx, 4
    mov         edi, n                         ; edi = &n
    add         edi, 12                        ; edi = &highend dword of n
    xor         eax, eax
    std
    repe        scasd
    jz          End
    bsr         eax, dword ptr[edi+4]          ; eax = index of highest 1 bit
    shl         ecx, 5                         ; ecx *= 32
    add         eax, ecx
End:
    mov         result, eax
    popf
  }
  return result;
}

// dst and x are both _uint128
// dwordIndex(_uint128 n) is the highest index,i = [0..3], so n.s4.i[i] != 0
// return (dwordIndex(dst)<<2) | dwordIndex(x), range:[0..15]
static inline BYTE getMultCode(const void *dst, const void *x) {
  BYTE result;
  __asm {
    pushf
    mov         ecx, 4
    mov         edi, x                         ; edi = &x
    add         edi, 12                        ; edi = &highend dword of x
    xor         eax, eax
    std
    repe        scasd
    mov         result, cl

    mov         cl, 4
    mov         edi, dst                       ; edi = &dst
    add         edi, 12                        ; edi = &highend dword of dst
    repe        scasd
    shl         cl, 2
    or          result, cl
    popf
  }
  return result;
}

// return scale
static int getFirst32(const _uint128 &n, UINT &n32) {
  const int expo2 = getExpo2(n);
  if(expo2 <= 31) {
    n32 = LO32(n);
    return 0;
  } else { // expo2 > 31
    const int scale = expo2 - 31;
    n32 = (UINT)(n >> scale);
    return scale;
  }
}

// return scale
static int getFirst63(const _uint128 &n, UINT64 &n63) {
  const int expo2 = getExpo2(n);
  if(expo2 <= 62) {
    n63 = LO64(n);
    return 0;
  } else {
    const int scale = expo2 - 62;
    n63 = (UINT64)(n >> scale);
    return scale;
  }
}

// dst and x are _uint128
static void mulU32(void *dst, UINT y, const void *x) {
  _asm {
    mov edi, dst               ; edi = &dst
    mov esi, x                 ; esi = &x
    mov eax, dword ptr[esi]
    mul y
    mov dword ptr[edi], eax
    mov dword ptr[edi+4], edx
    mov eax, dword ptr[esi+4]
    mul y
    add dword ptr[edi+4], eax
    mov dword ptr[edi+8], edx
    adc dword ptr[edi+8], 0
    mov eax, dword ptr[esi+8]
    mul y
    add dword ptr[edi+8], eax
    mov dword ptr[edi+12], edx
    adc dword ptr[edi+12], 0
    mov eax, dword ptr[esi+12]
    mul y
    add dword ptr[edi+12], eax
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
        push        ebp
        lea         ebx, a                       // ebx = &a
        mov         ecx, x                       // ecx = &x
        mov         ebp, dst                     // ebp = dst
        xor         eax, eax
        mov         dword ptr[ebp+8 ], eax       // dst[2..3] = 0
        mov         dword ptr[ebp+12], eax
        mov         eax, dword ptr[ebx]          // 1. round
        mul         dword ptr[ecx]               // [edx:eax] = x[0]*y[0]
        mov         dword ptr[ebp]  , eax
        mov         dword ptr[ebp+4], edx        // dst[0:1] = [edx:eax]

        mov         eax, dword ptr[ebx]          // 2. round
        mul         dword ptr[ecx+4]             // [edx:eax] = x[0]*y[1]
        mov         esi, eax                     //
        mov         edi, edx                     // [edi:esi] = [edx:eax]
        mov         eax, dword ptr[ebx+4]
        mul         dword ptr[ecx]               // [edx:eax] = x[1]*y[0]
        add         esi, eax                     //
        adc         edi, edx                     // [edi:esi] += [edx:eax]
        adc         dword ptr[ebp+12],0          // maybe extra carry for dst[3]
        add         dword ptr[ebp+4], esi
        adc         dword ptr[ebp+8], edi        // dst[1:2] += [edi:esi]

        mov         eax, dword ptr[ebx]          // 3. round. Extra carries are overflow
        mul         dword ptr[ecx+8]             // [edx:eax] = x[0]*y[2]
        mov         esi, eax                     //
        mov         edi, edx                     // [edi:esi] = [edx:eax]
        mov         eax, dword ptr[ebx+4]
        mul         dword ptr[ecx+4]             // [edx:eax] = x[1]*y[1]
        add         esi, eax                     //
        adc         edi, edx                     // [edi:esi] += [edx:eax]
        mov         eax, dword ptr[ebx+8]
        mul         dword ptr[ecx]               // [edx:eax] = x[2]*y[0]
        add         esi, eax                     //
        adc         edi, edx                     // [edi:esi] += [edx:eax]
        add         dword ptr[ebp+8 ], esi
        adc         dword ptr[ebp+12], edi       // dst[2:3] += [edi:esi]

        mov         eax, dword ptr[ebx]          // 4. round. Highend and carries are overflow
        mul         dword ptr[ecx+12]            // [edx:eax] = x[0]*y[3]
        mov         esi, eax                     // esi = eax
        mov         eax, dword ptr[ebx+4]        //
        mul         dword ptr[ecx+8]             // [edx:eax] = x[1]*y[2]
        add         esi, eax                     // esi += eax
        mov         eax, dword ptr[ebx+8]
        mul         dword ptr[ecx+4]             // [edx:eax] = x[2]*y[1]
        add         esi, eax                     // esi += eax
        mov         eax, dword ptr[ebx+12]
        mul         dword ptr[ecx]               // [edx:eax] = x[3]*y[0]
        add         esi, eax                     // esi += eax
        add         dword ptr[ebp+12], esi       // dst[3] += esi

        pop         ebp
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
      mov         ebx, x                         ; ebx     = &x
      mov         ecx, q                         ; ecx     = &q
      xor         edx, edx                       ;
      mov         eax, dword ptr[ebx+12]         ; edx:eax = 0:x.dword[3]
      div         y                              ; eax:edx = quotient:remainder
      mov         dword ptr[ecx+12], eax         ; save quot.dword[3]
      mov         eax, dword ptr[ebx+8]          ; edx:eax = last remainder:x.dword[2]
      div         y                              ; eax:edx = quotient:remainder
      mov         dword ptr[ecx+8], eax          ; save quot.dword[2]
      mov         eax, dword ptr[ebx+4]          ; edx:eax = last remainder:x.dword[1]
      div         y                              ; eax:edx = quotient:remainder
      mov         dword ptr[ecx+4], eax          ; save quot.dword[1]
      mov         eax, dword ptr[ebx]            ; edx:eax = last remainder:x.dword[0]
      div         y                              ; eax:edx = quotient:remainder
      mov         dword ptr[ecx], eax            ; save quot.dword[0]
      mov         r32, edx                       ; save remainder
      mul         y                              ; edx:eax = last quot * y
      cmp         eax, dword ptr[ebx]            ;
      jae         End                            ; if(eax < x.dword[0]) {
      sub         eax, dword ptr[ebx]            ;
      neg         eax                            ;   remainder = x.dword[0] - eax
      mov         r32, eax                       ; }
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
    for(int count = 0; rem >= y; count++) {
      UINT64         rem63;              // max 63 bits to prevent overflow in division
      const int      remScale = getFirst63(rem, rem63);
      int            shift    = remScale - yScale;
      UINT           q32;
      __asm {
        lea eax              , rem63
        mov edx              , dword ptr[eax+4]        ;
        mov eax              , dword ptr[eax]          ; edx:eax = rem63
        div y32                                        ; eax = rem63/y32, ignore remainder. See comment below
        mov ecx              , shift                   ;
        cmp ecx              , 0                       ;
        jge SaveQ32                                    ; if(shift < 0) {
        neg ecx                                        ;
        shr eax              , cl                      ;   eax = (rem63 / y32) >> -shift
        xor ecx              , ecx                     ;
        mov shift            , ecx                     ;   shift = 0
        cmp eax              , 1                       ;
        ja SaveQ32                                     ;   if(eax <= 1) {
        mov q32              , 1                       ;     q32 = 1;
        mov edx              , rem                     ;     edx = &rem
        mov ecx              , y                       ;     ecx = &y
        mov eax              , dword ptr[ecx   ]       ;     rem -= y
        sub dword ptr[edx   ], eax                     ;     we know y <= rem => rem-y >= 0
        mov eax              , dword ptr[ecx+ 4]       ;
        sbb dword ptr[edx+ 4], eax                     ;
        mov eax              , dword ptr[ecx+ 8]       ;
        sbb dword ptr[edx+ 8], eax                     ;
        mov eax              , dword ptr[ecx+12]       ;
        sbb dword ptr[edx+12], eax                     ;
        jmp UpdateQuot                                 ;     skip multiplication, subtraction already done, update quot and we're done
SaveQ32:                                               ;   }
        mov q32, eax                                   ; }
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

      _uint128 p128;                     // Assume: q32 > 1, shift >= 0
      mulU32(&p128, q32, &y);

      if(shift) {                        //   shift > 0;
        const _uint128 tmprem = rem >> shift;
        while(p128 > tmprem) {           // make sure p128 <= rem
          p128 -= y;                     // so we don't turn rem negative
          q32--;
        }
        p128 <<= shift;
      } else {                           //   shift == 0
        while(p128 > rem) {
          p128 -= y;
          q32--;
        }
      }
      rem -= p128;                         // Assume: p128 <= rem
UpdateQuot:
      if(quot) { // do we want the quot. If its NULL there's no need to do this
        if(count == 0) {
          *quot = q32;
        } else {
          const int ds = lastShift - shift;
          if(ds) {
            *quot <<= ds;
          }
          __asm {
            mov eax, q32               ; *quot += q32
            mov ecx, quot
            add dword ptr[ecx], eax
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

void int128neg(void *x) {
  __asm {
    mov esi, x
    not dword ptr[esi   ]
    not dword ptr[esi+ 4]
    not dword ptr[esi+ 8]
    not dword ptr[esi+12]
    add dword ptr[esi   ], 1
    adc dword ptr[esi+ 4], 0
    adc dword ptr[esi+ 8], 0
    adc dword ptr[esi+12], 0
  }
}

int int128cmp(const void *x1, const void *x2) {
  int result;
  __asm {
    mov         esi, x1
    mov         edi, x2
    mov         eax, dword ptr[esi+12]
    cmp         eax, dword ptr[edi+12]
    jl          lessthan                       ; signed compare of high int
    jg          greaterthan
    mov         eax, dword ptr[esi+8]
    cmp         eax, dword ptr[edi+8]
    jb          lessthan                       ; unsigned compare of the 3 others
    ja          greaterthan
    mov         eax, dword ptr[esi+4]
    cmp         eax, dword ptr[edi+4]
    jb          lessthan
    ja          greaterthan
    mov         eax, dword ptr[esi]            ;
    cmp         eax, dword ptr[edi]            ;
    jb          lessthan
    ja          greaterthan
    mov         result, 0                      ; they are equal
    jmp End
greaterthan:
    mov result, 1
    jmp End
lessThan:
    mov result, -1
  }
End:
  return result;
}

int uint128cmp(const void *x1, const void *x2) {
  int result;
  __asm {
    mov         esi, x1
    mov         edi, x2
    mov         eax, dword ptr[esi+12]
    cmp         eax, dword ptr[edi+12]
    jb          lessthan                       ; unsigned compare of all integers
    ja          greaterthan
    mov         eax, dword ptr[esi+8]
    cmp         eax, dword ptr[edi+8]
    jb          lessthan
    ja          greaterthan
    mov         eax, dword ptr[esi+4]
    cmp         eax, dword ptr[edi+4]
    jb          lessthan
    ja          greaterthan
    mov         eax, dword ptr[esi]            ;
    cmp         eax, dword ptr[edi]            ;
    jb          lessthan
    ja          greaterthan
    mov         result, 0                      ; they are equal
    jmp End
greaterthan:
    mov result, 1
    jmp End
lessThan:
    mov result, -1
  }
End:
  return result;
}

#endif // _M_X64
