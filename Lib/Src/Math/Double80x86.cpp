#include "pch.h"
#include <Math/Double80.h>
#include <Math/FPU.h>

#ifdef IS32BIT

#pragma check_stack(off)
#pragma warning(disable : 4073)
#pragma init_seg(lib)

static const float   ten(10.0f);
const double         _Dmaxi16P1 = ((UINT)_I16_MAX + 1);
const double         _Dmaxi32P1 = ((UINT)_I32_MAX + 1);
const Double80       _D80maxi64(  (BYTE*)"\xfe\xff\xff\xff\xff\xff\xff\xff\x3d\x40");           // _I64_MAX;
const Double80       _D80maxi64P1((BYTE*)"\x00\x00\x00\x00\x00\x00\x00\x80\x3e\x40");           // (UINT64)_I64_MAX + 1
static const Double80 M_2PiExp260((BYTE*)"\x35\xc2\x68\x21\xa2\xda\x0f\xc9\x3d\x40"); // 2pi*exp2(60) (=7.244019458077122e+018)

int Double80::getExpo10(const Double80 &x) { // static
  USHORT cwSave, ctrlFlags;
  int result;
  __asm {
    mov eax, x
    fld TBYTE PTR[eax]
    fldz
    fcomip st, st(1)            // compare x and pop 0
    jne x_not_zero              // if(x != 0) goto x_not_zero
    fstp st(0)                  // pop x
    mov result, 0               // x == 0 => result = 0
    jmp Exit
    x_not_zero :
    fabs
      fldlg2
      fxch st(1)
      fyl2x
      fnstcw cwSave
      mov ax, cwSave
      or ax, 0x400               // set bit 10
      and ax, 0xf7ff              // clear bit 11
      mov ctrlFlags, ax           // FPU.ctrlWorld.bit[10;11] = 1,0 = ROUND DOWN
      fldcw ctrlFlags
      fistp result
      fldcw cwSave
      Exit :
  }
  return result;
}

UINT getUint(const Double80 &x) {
  UINT result;
  if (x > _I32_MAX) {
    __asm {
      mov eax, x
      fld TBYTE PTR[eax]
      fsub _Dmaxi32P1
      fisttp result
    }
    result += (UINT)_I32_MAX + 1;
  }
  else if (x < -_I32_MAX) {
    __asm {
      mov eax, x
      fld TBYTE PTR[eax]
      fchs
      fsub _Dmaxi32P1
      fisttp result
    }
    result = -(int)result;
    result -= (UINT)_I32_MAX + 1;
  }
  else {
    __asm {
      mov eax, x
      fld TBYTE PTR[eax]
      fisttp result
    }
  }
  return result;
}

UINT64 getUint64(const Double80 &x) {
  UINT64 result;
  if (x > _D80maxi64) {
    __asm {
      mov eax, x
      fld TBYTE PTR[eax]
      fld _D80maxi64P1
      fsub
      fisttp result;
    }
    if (result <= (UINT64)_I64_MAX) {
      result += (UINT64)_I64_MAX + 1;
    }
  }
  else {
    __asm {
      mov eax, x
      fld TBYTE PTR[eax]
      fisttp result;
    }
  }
  return result;
}

Double80 fmod(const Double80 &x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR[eax]         //                                                    st0=y
    fabs                        // y = abs(y)                                         st0=|y|
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]         //                                                    st0=x,st1=|y|
    fldz                        //                                                    st0=0,st1=x,st2=|y|
    fcomip st, st(1)            // compare and pop zero                               st0=x,st1=|y|
    ja RepeatNegativeX          // if st(0) > st(1) (0 > x) goto repeat_negative_x
    RepeatPositiveX :                // do {                                               st0=x,st1=|y|, x > 0
    fprem                       //   st0 %= y
      fstsw ax
      sahf
      jpe RepeatPositiveX         // } while(statusword.c2 != 0);
      fldz                        //                                                    st0=0,st1=x,st2=|y|
      fcomip st, st(1)            // compare and pop zero
      jbe pop2                    // if(st(0) <= st(1) (0 <= remainder) goto pop2
      fadd                        // remainder += y
      fstp result                 // pop result
      jmp Exit                    // goto end
      RepeatNegativeX :                // do {                                               st0=x,st=|y|, x < 0
    fprem                       //   st0 %= y
      fstsw ax
      sahf
      jpe RepeatNegativeX         // } while(statusword.c2 != 0)
      fldz
      fcomip st, st(1)            // compare and pop zero
      jae pop2                    // if(st(0) >= st(1) (0 >= remainder) goto pop2
      fsubr                       // remainder -= y
      fstp result                 // pop result
      jmp Exit                    // goto end

      pop2 :                           //                                                    st0=x%y,st1=y
    fstp result                 // pop result
      fstp st(0)                  // pop y
      Exit :
  }
  return result;
}

Double80 sin(const Double80 &x) {
  Double80 result = fmod(x, M_2PiExp260);
  __asm {
    fld result
    fsin
    fstp result
  }
  return result;
}

Double80 cos(const Double80 &x) {
  Double80 result = fmod(x, M_2PiExp260);
  __asm {
    fld result
    fcos
    fstp result
  }
  return result;
}

Double80 tan(const Double80 &x) {
  Double80 result = fmod(x, M_2PiExp260);
  __asm {
    fld result
    fptan
    fstp result
    fstp result
  }
  return result;
}

void sincos(Double80 &c, Double80 &s) { // calculate both cos and sin. c:inout c, s:out
  Double80 r = fmod(c, M_2PiExp260);
  __asm {
    fld r
    fsincos
    mov eax, DWORD PTR c
    fstp TBYTE PTR[eax]
    mov eax, DWORD PTR s
    fstp TBYTE PTR[eax]
  }
}

Double80 exp(const Double80 &x) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]
    fldl2e
    fmul
    fld st(0)
    frndint
    fsub st(1), st(0)
    fxch st(1)
    f2xm1
    fld1
    fadd
    fscale
    fstp st(1)
    fstp result
  }
  return result;
}

Double80 exp10(const Double80 &x) {
  if (x.isZero()) {
    return Double80::_1;
  }

  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]
    fldl2t
    fmul
    fld st(0)
    frndint
    fsub st(1), st(0)
    fxch st(1)
    f2xm1
    fld1
    fadd
    fscale
    fstp st(1)
    fstp result
  }
  return result;
}

Double80 exp2(const Double80 &x) {
  if (x.isZero()) {
    return Double80::_1;
  }
  const FPUControlWord cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUNDDOWN);
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]
    fld st(0)
    frndint
    fsub st(1), st(0)
    fxch st(1)
    f2xm1
    fld1
    fadd
    fscale
    fstp st(1)
    fstp result
  }
  FPU::restoreControlWord(cwSave);
  return result;
}

Double80 pow(const Double80 &x, const Double80 &y) {
  if (y.isZero()) {
    return Double80::_1;
  }
  if (x.isZero()) {
    return y.isNegative() ? (Double80::_1 / Double80::_0) : Double80::_0;
  }

  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR[eax]
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]
    fyl2x
    fld st(0)
    frndint
    fsub st(1), st(0)
    fxch st(1)
    f2xm1
    fld1
    fadd
    fscale
    fstp st(1)
    fstp result
  }
  return result;
}

Double80 floor(const Double80 &x) {
  const FPUControlWord cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUNDDOWN);
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]
    frndint
    fstp result
  }
  FPU::restoreControlWord(cwSave);
  return result;
}

Double80 ceil(const Double80 &x) {
  const FPUControlWord cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUNDUP);
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]
    frndint
    fstp result
  }
  FPU::restoreControlWord(cwSave);
  return result;
}

#endif // IS32BIT
