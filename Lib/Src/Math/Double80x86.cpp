#include "pch.h"

#ifdef IS32BIT
#include <Math/Double80.h>
#include <Math/FPU.h>

#pragma check_stack(off)
#pragma warning(disable : 4073)
#pragma init_seg(lib)

static const float    ten(10.0f);
const double          _Dmaxi16P1 =  ((UINT)_I16_MAX + 1);
const double          _Dmaxi32P1 =  ((UINT)_I32_MAX + 1);
const Double80        _D80maxi64(   (BYTE*)"\xfe\xff\xff\xff\xff\xff\xff\xff\x3d\x40"); // (Double80)_I64_MAX;
const Double80        _D80maxi64P1( (BYTE*)"\x00\x00\x00\x00\x00\x00\x00\x80\x3e\x40"); // (Double80)_I64_MAX + 1
static const Double80 _D802PiExp260((BYTE*)"\x35\xc2\x68\x21\xa2\xda\x0f\xc9\x3d\x40"); // (Double80)2pi*exp2(60) (=7.244019458077122e+018)

UINT D80ToUI32(const Double80 &x) {
  UINT result;
  if(x > _I32_MAX) {
    __asm {
      mov eax, x
      fld TBYTE PTR[eax]
      fsub _Dmaxi32P1
      fisttp result
    }
    result += (UINT)_I32_MAX + 1;
  } else if(x < -_I32_MAX) {
    __asm {
      mov eax, x
      fld TBYTE PTR[eax]
      fchs
      fsub _Dmaxi32P1
      fisttp result
    }
    result = -(int)result;
    result -= (UINT)_I32_MAX + 1;
  } else {
    __asm {
      mov eax, x
      fld TBYTE PTR[eax]
      fisttp result
    }
  }
  return result;
}

UINT64 D80ToUI64(const Double80 &x) {
  UINT64 result;
  if(x > _D80maxi64) {
    __asm {
      mov eax, x
      fld TBYTE PTR[eax]
      fld _D80maxi64P1
      fsub
      fisttp result;
    }
    if(result <= (UINT64)_I64_MAX) {
      result += (UINT64)_I64_MAX + 1;
    }
  } else {
    __asm {
      mov eax, x
      fld TBYTE PTR[eax]
      fisttp result;
    }
  }
  return result;
}

char D80cmpI16(const Double80 &x, short y) {
  char result;
  __asm {
    mov     eax, x
    fld     TBYTE PTR[eax]
    ficomp  y
    fnstsw  ax
    sahf
    jp      Undef
    jb      XBelowY
    setnz   result
    jmp     Done
Undef   :
    mov     result, 2
    jmp     Done
XBelowY :
    mov     result, -1
Done    :
  }
  return result;
}

char D80cmpUI16(const Double80 &x, USHORT y) {
  char result;
  FILDUINT16(y)
  __asm {
    mov     eax, x
    fld     TBYTE PTR[eax]
    fcomip  st, st(1)                          ; st(0)=x, st(1)=y
    jp      Undef
    jb      XBelowY
    setnz   result
    jmp     Done
Undef  :
    mov     result, 2
    jmp     Done
XBelowY:
    mov     result, -1
Done:
    fstp    st(0)                               ; pop y
  }
  return result;
}

char D80cmpI32(const Double80 &x, INT y) {
  char result;
  __asm {
    mov     eax, x
    fld     TBYTE PTR[eax]
    ficomp  y
    fnstsw  ax
    sahf
    jp      Undef
    jb      XBelowY
    setnz   result
    jmp     Done
Undef   :
    mov     result, 2
    jmp     Done
XBelowY :
    mov     result, -1
Done    :
  }
  return result;
}

char D80cmpUI32(const Double80 &x, UINT y) {
  char result;
  FILDUINT32(y)
  __asm {
    mov     eax, x
    fld     TBYTE PTR[eax]
    fcomip  st, st(1)                          ; st(0)=x, st(1)=y
    jp      Undef
    jb      XBelowY
    setnz   result
    jmp     Done
Undef:
    mov     result, 2
    jmp     Done
XBelowY:
    mov     result, -1
Done:
    fstp    st(0)                               ; pop y
  }
  return result;
}

char D80cmpI64(const Double80 &x, INT64 y) {
  char result;
  __asm {
    fild    y
    mov     eax, x
    fld     TBYTE PTR[eax]
    fcomip  st, st(1)                          ; st(0)=x, st(1)=y
    jp      Undef
    jb      XBelowY
    setnz   result
    jmp     Done
Undef:
    mov     result, 2
    jmp     Done
XBelowY:
    mov     result, -1
Done:
    fstp    st(0)                               ; pop y
  }
  return result;
}

char D80cmpUI64(const Double80 &x, UINT64 y) {
  char result;
  FILDUINT64(y)
  __asm {
    mov     eax, x
    fld     TBYTE PTR[eax]
    fcomip  st, st(1)                          ; st(0)=x, st(1)=y
    jp      Undef
    jb      XBelowY
    setnz   result
    jmp     Done
Undef:
    mov     result, 2
    jmp     Done
XBelowY:
    mov     result, -1
Done:
    fstp    st(0)                               ; pop y
  }
  return result;
}

char D80cmpFlt(const Double80 &x, float y) {
  char result;
  __asm {
    fld     y
    mov     eax, x
    fld     TBYTE PTR[eax]
    fcomip  st, st(1)                          ; st(0)=x, st(1)=y
    jp      Undef
    jb      XBelowY
    setnz   result
    jmp     Done
Undef:
    mov     result, 2
    jmp     Done
XBelowY:
    mov     result, -1
Done:
    fstp    st(0)                               ; pop y
  }
  return result;
}

char D80cmpDbl(const Double80 &x, double y) {
  char result;
  __asm {
    fld     y
    mov     eax, x
    fld     TBYTE PTR[eax]
    fcomip  st, st(1)                          ; st(0)=x, st(1)=y
    jp      Undef
    jb      XBelowY
    setnz   result
    jmp     Done
Undef:
    mov     result, 2
    jmp     Done
XBelowY:
    mov     result, -1
Done:
    fstp    st(0)                               ; pop y
  }
  return result;
}

char D80cmpD80(const Double80 &x, const Double80 &y) {
  char result;
  __asm {
    mov     eax, y
    fld     TBYTE PTR[eax]
    mov     eax, x
    fld     TBYTE PTR[eax]
    fcomip  st, st(1)                          ; st(0)=x, st(1)=y
    jp      Undef
    jb      XBelowY
    setnz   result
    jmp     Done
Undef:
    mov     result, 2
    jmp     Done
XBelowY:
    mov     result, -1
Done:
    fstp    st(0)                               ; pop y
  }
  return result;
}

void D80rem(Double80 &dst, const Double80 &x) {
  __asm {
    mov ecx, DWORD PTR x        // don't use eax as addr of dst,,,fstw ax will kill it!
    fld TBYTE PTR[ecx]          //                                                    st0=x
    fabs                        // x = abs(x)                                         st0=|x|
    mov ecx, DWORD PTR dst
    fld TBYTE PTR[ecx]          //                                                    st0=dst,st1=|x|
    fldz                        //                                                    st0=0,st1=dst,st2=|y|
    fcomip st, st(1)            // compare and pop zero                               st0=dst,st1=|x|
    ja RepeatNegative           // if st(0) > st(1) (0 > x) goto repeat_negative_x
RepeatPositive :                // do {                                               st0=dst,st1=|x|, dst > 0
    fprem                       //   st0 %= x
    fstsw ax
    sahf
    jpe RepeatPositive          // } while(statusword.c2 != 0);
    fldz                        //                                                    st0=0,st1=dst,st2=|x|
    fcomip st, st(1)            // compare and pop zero
    jbe pop2                    // if(st(0) <= st(1) (0 <= remainder) goto pop2
    fadd                        // remainder += x
    fstp TBYTE PTR[ecx]         // pop result
    jmp Exit                    // goto end
RepeatNegative  :               // do {                                               st0=dst,st=|x|, dst < 0
    fprem                       //   st0 %= x
    fstsw ax
    sahf
    jpe RepeatNegative          // } while(statusword.c2 != 0)
    fldz
    fcomip st, st(1)            // compare and pop zero
    jae pop2                    // if(st(0) >= st(1) (0 >= remainder) goto pop2
    fsubr                       // remainder -= x
    fstp TBYTE PTR[ecx]         // pop result
    jmp Exit                    // goto end

pop2 :                          //                                                    st0=dst%x,st1=x
    fstp TBYTE PTR[ecx]         // pop result
    fstp st(0)                  // pop x
Exit :
  }
}

int D80getExpo10(const Double80 &x) {
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
    or ax, 0x400                // set bit 10
    and ax, 0xf7ff              // clear bit 11
    mov ctrlFlags, ax           // FPU.ctrlWorld.bit[10;11] = 1,0 = ROUND DOWN
    fldcw ctrlFlags
    fistp result
    fldcw cwSave
Exit :
  }
  return result;
}

void D80sin(Double80 &x) {
  D80rem(x, _D802PiExp260);
  __asm {
    mov eax, x
    fld TBYTE PTR[eax]
    fsin
    fstp TBYTE PTR[eax]
  }
}

void D80cos(Double80 &x) {
  D80rem(x, _D802PiExp260);
  __asm {
    mov eax, x
    fld TBYTE PTR[eax]
    fcos
    fstp TBYTE PTR[eax]
  }
}
void D80tan(Double80 &x) {
  D80rem(x, _D802PiExp260);
  __asm {
    mov eax, x
    fld TBYTE PTR[eax]
    fptan
    fstp TBYTE PTR[eax]
    fstp TBYTE PTR[eax]
  }
}

// inout is c, out s
void D80sincos(Double80 &c, Double80 &s) {
  D80rem(c, _D802PiExp260);
  __asm {
    mov eax, c
    fld TBYTE PTR[eax]
    fsincos
    fstp TBYTE PTR[eax]
    mov eax, DWORD PTR s
    fstp TBYTE PTR[eax]
  }
}

void D80exp(Double80 &x) {
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
    fstp TBYTE PTR[eax]
  }
}

void D80exp10(Double80 &x) {
  if(x.isZero()) {
    x = Double80::_1;
    return;
  }

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
    fstp TBYTE PTR[eax]
  }
}

void D80exp2(Double80 &x) {
  if(x.isZero()) {
    x = Double80::_1;
    return;

  }
  const FPUControlWord cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUNDDOWN);
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
    fstp TBYTE PTR[eax]
  }
  FPU::restoreControlWord(cwSave);
}

// x = pow(x,y)
void D80pow(Double80 &x, const Double80 &y) {
  if(y.isZero()) {
    x = Double80::_1;
    return;
  }
  if(x.isZero()) {
    x = y.isNegative() ? (Double80::_1 / Double80::_0) : Double80::_0;
    return;
  }

  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR[eax]
    mov eax, DWORD PTR x      ;  eax = &x
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
    fstp TBYTE PTR[eax]
  }
}

void D80floor(Double80 &x) {
  const FPUControlWord cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUNDDOWN);
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]
    frndint
    fstp TBYTE PTR[eax]
  }
  FPU::restoreControlWord(cwSave);
}

void D80ceil(Double80 &x) {
  const FPUControlWord cwSave = FPU::setRoundMode(FPU_ROUNDCONTROL_ROUNDUP);
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]
    frndint
    fstp TBYTE PTR[eax]
  }
  FPU::restoreControlWord(cwSave);
}

#endif // IS32BIT
