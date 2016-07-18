#include "pch.h"

#ifndef _M_X64


#include <Math/Int128.h>

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
    mov         eax, dword ptr[esi + 4]
    sbb         dword ptr[edi + 4], eax
    mov         eax, dword ptr[esi + 8]
    sbb         dword ptr[edi + 8], eax
    mov         eax, dword ptr[esi + 12]
    sbb         dword ptr[edi + 12], eax
  }
}

void int128inc(void *x) {
  __asm {
    mov         edi, x
    add         dword ptr[edi], 1
    adc         dword ptr[edi+4], 0
    adc         dword ptr[edi+8], 0
    adc         dword ptr[edi+12], 0
  }
}

void int128dec(void *x) {
  __asm {
    mov         edi, x
    sub         dword ptr[edi], 1
    sbb         dword ptr[edi+4], 0
    sbb         dword ptr[edi+8], 0
    sbb         dword ptr[edi+12], 0
  }
}

#pragma warning(disable:4731)

void int128mul(void *dst, const void *x) {
  _int128       *dp = (_int128*)dst;
  const _int128 *b = (const _int128*)x;

  if (!(dp->s4.i[1] || b->s4.i[1] || dp->s4.i[2] || b->s4.i[2] || dp->s4.i[3] || b->s4.i[3])) {
    HI(*dp) = 0;
    LO(*dp) = __int64(dp->s4.i[0]) * b->s4.i[0]; // simple _int64 multiplication. int32 * int32
  }
  else {
   _int128        a(*dp);
    __asm {
      push        ebp
      lea         ebx, a                   // ebx = a
      mov         ecx, b                   // ecx = b
      mov         ebp, dst                 // ebp = &prod
      xor         eax, eax
      mov         dword ptr[ebp+8 ], eax   // dst[2..3] = 0
      mov         dword ptr[ebp+12], eax
      mov         eax, dword ptr[ebx]      // 1. round
      mul         dword ptr[ecx]           // [edx:eax] = x[0]*y[0]
      mov         dword ptr[ebp], eax
      mov         dword ptr[ebp + 4], edx  // set prod{0:1]

      mov         eax, dword ptr[ebx]      // 2. round
      mul         dword ptr[ecx + 4]       // [edx:eax] = x[0]*y[1]
      mov         edi, eax                 //
      mov         esi, edx                 // [esi:esd] = [edx:eax]
      mov         eax, dword ptr[ebx + 4]
      mul         dword ptr[ecx]           // [edx:eax] = x[1]*y[0]
      add         edi, eax                 //
      adc         esi, edx                 // [esi:edi] += [edx:eax]
      pushfd                               // may be extra carry for prod[3]
      add         dword ptr[ebp + 4], edi
      adc         dword ptr[ebp + 8], esi  // prod[1:2] += [esi:edi]

      mov         eax, dword ptr[ebx]      // 3. round. extra carries are overflow
      mul         dword ptr[ecx + 8]       // [edx:eax] = x[0]*y[2]
      mov         edi, eax                 //
      mov         esi, edx                 // [esi:esd] = [edx:eax]
      mov         eax, dword ptr[ebx + 4]
      mul         dword ptr[ecx + 4]       // [edx:eax] = x[1]*y[1]
      add         edi, eax                 //
      adc         esi, edx                 // [esi:edi] += [edx:eax]
      mov         eax, dword ptr[ebx + 8]
      mul         dword ptr[ecx]           // [edx:eax] = x[2]*y[0]
      add         edi, eax                 //
      adc         esi, edx                 // [esi:edi] += [edx:eax]
      add         dword ptr[ebp+8], edi
      adc         dword ptr[ebp+12], esi   // prod[2:3] += [esi:edi]

      mov         eax, dword ptr[ebx]      // 4. round. dont care about high end or carris. its overflow
      mul         dword ptr[ecx+12]        // [edx:eax] = x[0]*y[3]
      mov         edi, eax                 // edi = eax
      mov         eax, dword ptr[ebx+4]    //
      mul         dword ptr[ecx+8]         // [edx:eax] = x[1]*y[2]
      add         edi, eax                 // edi += eax
      mov         eax, dword ptr[ebx+8]    
      mul         dword ptr[ecx+4]         // [edx:eax] = x[2]*y[1]
      add         edi, eax                 // edi += eax
      mov         eax, dword ptr[ebx+12]    
      mul         dword ptr[ecx]           // [edx:eax] = x[3]*y[0]
      add         edi, eax                 // edi += eax
      add         dword ptr[ebp+12], edi   // prod[3] += edi
      popfd
      adc         dword ptr[ebp + 12],0    // add the saved carry

      pop         ebp
    }
  }
}

void int128shl(void *x, int shft) {
  __asm {
    mov         ecx, shft                   ; ecx = shift count
    mov         esi, x
    cmp         cl, 128
    jae         RetZero
    cmp         cl, 96
    jae         More96
    cmp         cl, 64
    jae         More64
    cmp         cl, 32
    jae         More32
    mov         eax, dword ptr[esi + 8]
    shld        dword ptr[esi + 12], eax, cl  ; shift s4.i[3] adding bits from s4.i[2]
    mov         eax, dword ptr[esi + 4]
    shld        dword ptr[esi + 8], eax, cl   ; shift s4.i[2] adding bits from s4.i[1]
    mov         eax, dword ptr[esi]
    shld        dword ptr[esi + 4], eax, cl   ; shift s4.i[1] adding bits from s4.i[0]
    shl         dword ptr[esi], cl            ; shift s4.i[0]
    jmp         End
More32 :                                      ; 32 <= cl < 64
    and         cl, 1Fh                       ; cl %= 32
    mov         eax, dword ptr[esi + 8]
    mov         dword ptr[esi + 12], eax      ; s4.i[3] = s4.i[1]
    mov         eax, dword ptr[esi + 4]
    shld        dword ptr[esi + 12], eax, cl  ; shift s4.i[3] adding bits eax
    mov         dword ptr[esi + 8], eax       ; s4.i[2] = s4.i[1]
    mov         eax, dword ptr[esi]           ; eax = s4.i[0]
    shld        dword ptr[esi + 8], eax, cl   ; shift s4.i[2] adding bits eax
    shl         eax, cl
    mov         dword ptr[esi + 4], eax       ; shift s4.i[1] eax << cl (old i[0] << cl)
    xor         eax, eax
    mov         dword ptr[esi], eax           ; s4.i[0] = 0
    jmp         End
More64:                                       ; 64 <= cl < 96
    and         cl, 1Fh                       ; cl %= 32
    mov         eax, dword ptr[esi + 4]
    mov         dword ptr[esi + 12], eax      ; s4.i[3] = s4.i[1]
    mov         eax, dword ptr[esi]           ; eax = s4.i[0]
    shld        dword ptr[esi + 12], eax, cl  ; shift s4.i[3] adding bits eax
    shl         eax, cl
    mov         dword ptr[esi + 8], eax       ; shift s4.i[2] eax << cl
    xor         eax, eax
    mov         dword ptr[esi], eax           ; s4.i[0] = 0
    mov         dword ptr[esi + 4], eax       ; s4.i[1] = 0
    jmp         End
More96:                                       ; 96 <= cl < 128
    and         cl, 1Fh                       ; cl %= 32
    mov         eax, dword ptr[esi]
    shl         eax, cl
    mov         dword ptr[esi+12], eax        ; s4.i[3] = s4.i[0] << cl
    xor         eax, eax
    mov         dword ptr[esi], eax           ; s4.i[0] = 0
    mov         dword ptr[esi + 4], eax       ; s4.i[1] = 0
    mov         dword ptr[esi + 8], eax       ; s4.i[2] = 0
    jmp         End
RetZero:
    xor         eax, eax                      ; return 0
    mov         dword ptr[esi   ], eax
    mov         dword ptr[esi+4 ], eax
    mov         dword ptr[esi+8 ], eax
    mov         dword ptr[esi+12], eax
  }
End:;
}

void int128shr(void *x, int shft) {           // signed shift right
  __asm {
    mov         ecx, shft                     ; ecx = shift count
    mov         esi, x
    cmp         cl, 128
    jae         RetSign
    cmp         cl, 96
    jae         More96
    cmp         cl, 64
    jae         More64
    cmp         cl, 32
    jae         More32
    mov         eax, dword ptr[esi + 4]
    shrd        dword ptr[esi], eax, cl       ; shift s4[0] new bits from s4[1] (eax)
    mov         eax, dword ptr[esi + 8]
    shrd        dword ptr[esi + 4], eax, cl   ; shift s4[1] new bits from s4[2] (eax)
    mov         eax, dword ptr[esi + 12]
    shrd        dword ptr[esi + 8], eax, cl   ; shift s4[2] new bits from s4[3] (eax)
    sar         dword ptr[esi + 12], cl       ; shift s4[3]
    jmp         End
More32 :                                      ; 32 <= cl < 64
    and         cl, 1Fh                       ; cl %= 32
    mov         eax, dword ptr[esi + 4]
    mov         dword ptr[esi], eax           ; s4[0] = eax (old s4[1])
    mov         eax, dword ptr[esi + 8]       ; eax = s4[2]
    shrd        dword ptr[esi], eax, cl       ; shift s[0] new bits from s4[2] (eax)

    mov         dword ptr[esi + 4], eax       ; s4[1] = eax (old s4[2])
    mov         eax, dword ptr[esi + 12]      ; eax = s4[3]
    shrd        dword ptr[esi + 4], eax, cl   ; shift s4[1] new bits from eax (old s4[3])

    mov         dword ptr[esi + 8], eax       ; s4[2] = eax (old s4[3])
    mov         eax, dword ptr[esi + 12]      ; eax = s4[3]
    sar         eax, 1fh                      ; eax contain signbit in all bits
    shrd        dword ptr[esi + 8], eax, cl   ; shift s4[1] new bits from eax (sign of s4[3])
    mov         dword ptr[esi + 12], eax      ; s4[3] = eax
    jmp         End
More64:                                       ; 64 <= cl < 96
    and         cl, 1Fh                       ; cl %= 32

    mov         eax, dword ptr[esi + 8]
    mov         dword ptr[esi], eax           ; s4[0] = s4[2]
    mov         eax, dword ptr[esi + 12]      ; eax = s4[3]
    shrd        dword ptr[esi], eax, cl       ; shift s4[0] new bits from eax (old s4[3])

    sar         eax, cl                       ; eax contain sign bit in all bits
    mov         dword ptr[esi + 4], eax       ; s4[1] = eax (old s4[3])

    sar         eax, 1fh
    mov         dword ptr[esi + 8], eax       ; s4[2] = sign of s4[3]
    mov         dword ptr[esi + 12], eax      ; s4[3] = sign of s4[3]
    jmp         End

More96:                                       ; 96 <= cl < 128
    and         cl, 1Fh                       ; cl %= 32
    mov         eax, dword ptr[esi + 12]
    sar         eax, cl
    mov         dword ptr[esi], eax           ; s4[0] = s4[3] >> cl (shift in signbit of s4[3])
    sar         eax, 1fh      
    mov         dword ptr[esi + 4 ], eax      ; s4[1] = sign og s4[3]
    mov         dword ptr[esi + 8 ], eax      ; s4[2] = sign og s4[3]
    mov         dword ptr[esi + 12], eax      ; s4[3] = sign og s4[3]
    jmp         End
RetSign:
    mov         eax, dword ptr[esi+12]
    sar         eax,1Fh
    mov         dword ptr[esi   ], eax
    mov         dword ptr[esi+4 ], eax
    mov         dword ptr[esi+8 ], eax
    mov         dword ptr[esi+12], eax
  }
End:;
}

void uint128shr(void *x, int shft) { // usigned shift right
  __asm {
    mov         ecx, shft                     ; ecx = shift count
    mov         esi, x
    cmp         cl, 128
    jae         RetZero
    cmp         cl, 96
    jae         More96
    cmp         cl, 64
    jae         More64
    cmp         cl, 32
    jae         More32
    mov         eax, dword ptr[esi + 4]
    shrd        dword ptr[esi], eax, cl       ; shift s4[0] new bits from s4[1] (eax)
    mov         eax, dword ptr[esi + 8]
    shrd        dword ptr[esi + 4], eax, cl   ; shift s4[1] new bits from s4[2] (eax)
    mov         eax, dword ptr[esi + 12]
    shrd        dword ptr[esi + 8], eax, cl   ; shift s4[2] new bits from s4[3] (eax)
    shr         dword ptr[esi + 12], cl       ; shift s4[3]
    jmp         End

More32 :                                      ; 32 <= cl < 64
    and         cl, 1Fh                       ; cl %= 32
    mov         eax, dword ptr[esi + 4]
    mov         dword ptr[esi], eax           ; s4[0] = eax (old s4[1])
    mov         eax, dword ptr[esi + 8]       ; eax = s4[2]
    shrd        dword ptr[esi], eax, cl       ; shift s[0] new bits from s4[2] (eax)

    mov         dword ptr[esi + 4], eax       ; s4[1] = eax (old s4[2])
    mov         eax, dword ptr[esi + 12]      ; eax = s4[3]
    shrd        dword ptr[esi + 4], eax, cl   ; shift s4[1] new bits from eax (old s4[3])

    shr         eax, cl
    mov         dword ptr[esi + 8], eax       ; s4[2] = eax << cl
    xor         eax, eax
    mov         dword ptr[esi + 12], eax      ; s4[3] = 0
    jmp         End

More64:                                       ; 64 <= cl < 96
    and         cl, 1Fh                       ; cl %= 32

    mov         eax, dword ptr[esi + 8]
    mov         dword ptr[esi], eax           ; s4[0] = s4[2]
    mov         eax, dword ptr[esi + 12]      ; eax = s4[3]
    shrd        dword ptr[esi], eax, cl       ; shift s4[0] new bits from eax (old s4[3])

    shr         eax, cl
    mov         dword ptr[esi + 4], eax       ; s4[1] = eax >> cl (old s4[3] >> cl)

    xor         eax, eax
    mov         dword ptr[esi + 8], eax       ; s4[2] = 0
    mov         dword ptr[esi + 12], eax      ; s4[3] = 0
    jmp         End

More96:                                       ; 96 <= cl < 128
    and         cl, 1Fh                       ; cl %= 32
    mov         eax, dword ptr[esi + 12]
    shr         eax, cl
    mov         dword ptr[esi], eax           ; s4[0] = s4[3] >> cl
    xor         eax, eax      
    mov         dword ptr[esi + 4 ], eax      ; s4[1] = 0
    mov         dword ptr[esi + 8 ], eax      ; s4[2] = 0
    mov         dword ptr[esi + 12], eax      ; s4[3] = 0
    jmp         End
RetZero:
    xor         eax,eax
    mov         dword ptr[esi   ], eax
    mov         dword ptr[esi+4 ], eax
    mov         dword ptr[esi+8 ], eax
    mov         dword ptr[esi+12], eax
  }
End:;
}

void _uint128::approxQuot(unsigned long y, int yScale) {
  int xExpo;
  const unsigned int q = getFirstBitsNoScale(32, xExpo) / y;
  *this = q;

  if (q) {
    int shift = xExpo - yScale - 32;
    if (shift > 0) {
      *this <<= shift;
    } else if (shift < 0) {
      *this >>= -shift;
    }
  }
}

unsigned int _uint128::getFirstBitsAutoScale(int k, int &scale) const {
  unsigned int result;
  int tmpScale, expo;
  __asm {
    pushf
    mov         ecx, 4
    mov         edi, this
    add         edi, 12
    xor         eax, eax
    std
    repe        scasd
    jnz         SearchBit
    mov         result, 0
    jmp         End
SearchBit:                             ; assume ecx hold the index of integer with first 1-bit
    add         edi, 4
    mov         edx, dword ptr [edi]
    bsr         eax, edx               ; eax holds index of highest 1 bit
    inc         eax                    ; and now #bits in the int-element
    cmp         eax, k
    jge         GotEnoughDigits
    test        ecx, ecx               ; Get some bits from next, if we got one
    jne         AppendMoreBits         ; if(got more digits) goto AppendMoreBits
                                       ; else no more bits. take it as it is
    mov         edi, k
    sub         edi, eax               ; tmpScale (edi) = k - digits
    dec         eax
    mov         expo, eax              ; expo = index of higehst 1-bit (ecx == 0)
    jmp         RemoveTrailingZeros

AppendMoreBits:                        ; Assume edi points to highest int with 1-bits
                                       ; and this is not the lowest (ecx>0)
                                       ; and eax = index of higest 1-bit + 1
    shl         ecx, 5
    add         ecx, eax              
    dec         ecx
    mov         expo, ecx              ; expo = 32 * ecx + eax - 1
    mov         edi, dword ptr[edi-4]  ; edi = previous int
    mov         ecx, k
    sub         ecx, eax               ; ecx = k - bits already in edx
    shld        edx, edi, cl           ; Shift edx left adding new bits from previous digit (edi)
    xor         edi, edi               ; edi = 0
    jmp         RemoveTrailingZeros

GotEnoughDigits:                       ; assume ecx = largest index of nonzero int and eax = #bits in this
    shl         ecx, 5
    add         ecx, eax               
    dec         ecx
    mov         expo, ecx              ; expo = 32 * ecx + eax
    xor         edi, edi               ; tmpScale = 0
    sub         eax, k
    jle         RemoveTrailingZeros
    mov         ecx, eax
    shr         edx, cl

RemoveTrailingZeros:                   ; assume edx holds as many bits we can get, right shift.
                                       ; now downscale it and count number of shifts
    test        edx, 1
    jne         EndResult
StartLoop:
    shr         edx, 1
    inc         edi
    test        edx, 1
    je          startLoop
EndResult:
    add         edi, expo
    sub         edi, k
    mov         tmpScale, edi
    mov         result  , edx
End:
    popf
  }
  scale = tmpScale;
  return result;
}

unsigned int _uint128::getFirstBitsNoScale(int k, int &expo) const {
  unsigned int result;
  int tmpExpo;
  __asm {
    pushf
    mov         ecx, 4
    mov         edi, this
    add         edi, 12
    xor         eax, eax
    std
    repe        scasd
    jnz         SearchBit
    mov         result, 0
    jmp         End
SearchBit:                             ; assume ecx hold the index of integer with first 1-bit
    add         edi, 4
    mov         edx, dword ptr [edi]
    bsr         eax, edx               ; eax holds index of highest 1 bit
    inc         eax
    cmp         eax, k
    jge         GotEnoughDigits
    test        ecx, ecx               ; Get som more digit from next, if we got one
    jne         AppendMoreBits         ; if(got more digits) goto AppendMoreBits
                                       ; else not more bits. shift in 0-bits from right
    mov         ecx, k
    sub         ecx, eax
    shl         edx, cl
    dec         eax
    mov         tmpExpo, eax           ; expo = index of higehst 1-bit (ecx == 0)
    jmp         End

AppendMoreBits:                        ; Assume edi points to highest int with 1-bits
                                       ; and this is not the lowest (ecx>0)
                                       ; and eax = index of higest 1-bit + 1
    shl         ecx, 5
    add         ecx, eax              
    dec         ecx
    mov         tmpExpo, ecx           ; expo = 32 * ecx + eax - 1
    mov         edi, dword ptr[edi-4]  ; edi = previous int
    mov         ecx, k
    sub         ecx, eax               ; ecx = k - bits already in edx
    shld        edx, edi, cl           ; Shift edx left adding new bits from previous digit (edi)
    jmp         End

GotEnoughDigits:                       ; assume ecx = largest index of nonzero int and eax = #bits in this
    shl         ecx, 5
    add         ecx, eax               
    dec         ecx
    mov         tmpExpo, ecx
    sub         eax, k
    jle         End
    mov         ecx, eax
    shr         edx, cl

End:
    mov         result  , edx
    popf
  }
  expo = tmpExpo;
  return result;
}

void signedQuotRemainder(const _int128 &a, const _int128 &b, _int128 *quot, _int128 *rem) {
  const bool aNegative = a.isNegative();
  const bool bNegative = b.isNegative();
  _uint128 rest = aNegative ? -a : a;
  _uint128 y    = bNegative ? -b : b;

  int yScale;
  const unsigned int yFirst = y.getFirstBitsAutoScale(16, yScale);

  _uint128 result;
  for (bool loopDone = false;;loopDone = true) {
    if (loopDone) {
      _uint128 t = rest;
      t.approxQuot(yFirst, yScale);
      result += t;
      rest -= t * y;
    } else {
      result = rest;
      result.approxQuot(yFirst, yScale);
      rest -= result * y;
    }
    if (rest < y) break;
  }
  if (quot) {
    *quot = (aNegative == bNegative) ? result : -result;
  }
  if (rem) {
    *rem = aNegative ? rest : -rest;
  }
}

void unsignedQuotRemainder(const _uint128 &a, const _uint128 &y, _uint128 *quot, _uint128 *rem) {
  _uint128 rest = a;

  int yScale;
  const unsigned int yFirst = y.getFirstBitsAutoScale(16, yScale);

  _uint128 result;
  for (bool loopDone = false;;loopDone = true) {
    if (loopDone) {
      _uint128 t = rest;
      t.approxQuot(yFirst, yScale);
      result += t;
      rest -= t * y;
    } else {
      result = rest;
      result.approxQuot(yFirst, yScale);
      rest -= result * y;
    }
    if (rest < y) break;
  }
  if (quot) *quot = result;
  if (rem) *rem = rest;
}


void int128div(void *dst, const void *x) {
  _int128       *dp = (_int128*)dst;
  const _int128  a(*dp);
  const _int128 &b = *(const _int128*)x;
  signedQuotRemainder(a, b, dp, NULL);
}

void int128rem(void *dst, const void *x) {
  _int128       *dp = (_int128*)dst;
  const _int128  a(*dp);
  const _int128 &b = *(const _int128*)x;
  signedQuotRemainder(a, b, NULL, dp);
}

void uint128div(void *dst, const void *x) {
  _uint128      *dp = (_uint128*)dst;
  const _uint128  a(*dp);
  const _uint128 &b = *(const _uint128*)x;
  unsignedQuotRemainder(a, b, dp, NULL);
}

void uint128rem(void *dst, const void *x) {
  _uint128       *dp = (_uint128*)dst;
  const _uint128  a(*dp);
  const _uint128 &b = *(const _uint128*)x;
  unsignedQuotRemainder(a, b, NULL, dp);
}

void int128neg(void *x) {
  __asm {
    mov         esi, x
    mov         eax, dword ptr[esi]
    neg         eax
    mov         dword ptr[esi], eax

    mov         eax, dword ptr[esi + 4]
    adc         eax, 0
    neg         eax
    mov         dword ptr[esi + 4], eax

    mov         eax, dword ptr[esi + 8]
    adc         eax, 0
    neg         eax
    mov         dword ptr[esi + 8], eax

    mov         eax, dword ptr[esi + 12]
    adc         eax, 0
    neg         eax
    mov         dword ptr[esi + 12], eax
  }
}

int int128cmp(const void *n1, const void *n2) {
  int result;
  __asm {
    mov         esi, n1
    mov         edi, n2
    mov         eax, dword ptr[esi + 12]
    cmp         eax, dword ptr[edi + 12]
    jl          lessthan                    ; signed compare of high int
    jg          greaterthan
    mov         eax, dword ptr[esi + 8]
    cmp         eax, dword ptr[edi + 8]
    jb          lessthan                    ; unsigned compare of the 3 others
    ja          greaterthan
    mov         eax, dword ptr[esi + 4]
    cmp         eax, dword ptr[edi + 4]
    jb          lessthan
    ja          greaterthan
    mov         eax, dword ptr[esi]         ;
    cmp         eax, dword ptr[edi]         ;
    jb          lessthan           
    ja          greaterthan
    mov         result, 0                   ; they are equal
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

int uint128cmp(const void *n1, const void *n2) {
  int result;
  __asm {
    mov         esi, n1
    mov         edi, n2
    mov         eax, dword ptr[esi + 12]
    cmp         eax, dword ptr[edi + 12]
    jb          lessthan                    ; unsigned compare of all integers
    ja          greaterthan
    mov         eax, dword ptr[esi + 8]
    cmp         eax, dword ptr[edi + 8]
    jb          lessthan
    ja          greaterthan
    mov         eax, dword ptr[esi + 4]
    cmp         eax, dword ptr[edi + 4]
    jb          lessthan
    ja          greaterthan
    mov         eax, dword ptr[esi]         ;
    cmp         eax, dword ptr[edi]         ;
    jb          lessthan           
    ja          greaterthan
    mov         result, 0                   ; they are equal
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
