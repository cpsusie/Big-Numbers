#include "pch.h"
#include <Limits.h>
#include <Float.h>

double cot(double x) {
  return 1.0 / tan(x);
}

double acot(double x) {
  return M_PI / 2 - atan(x);
}

int getExpo10(double x) {
  return (x == 0) ? 0 : (int)floor(log10(fabs(x)));
}

double mypow(double x, double y) {
  switch(sign(y)) {
  case 0: // y == 0
    return x == 0
         ? std::numeric_limits<double>::quiet_NaN()
         : 1;
  case -1: // y < 0
    if(x == 0) { // 0^negative = +inf
      return std::numeric_limits<double>::infinity();
    }
    // continue case
  default:
    if(x < 0) {
      if(y == floor(y)) {
        const INT64 d = (INT64)y;
        return isOdd(d) ? -pow(-x, y) : pow(-x, y);
      }
    }
    return pow(x, y);
  }
}

double root(double x, double y) {
  if(x < 0) {
    if(y == floor(y)) {
      const INT64 d = (INT64)y;
      if(isOdd(d)) {
        return -pow(-x, 1.0 / y);
      }
    }
  }
  return pow(x, 1.0/y);
}

#if defined(IS32BIT)
double exp10(double x) {
  if(x == 0) return 1;

  USHORT cwSave, ctrlFlags;
  double result;
  __asm {
    fld x
    fldl2t
    fmul
    fld st(0)
    fnstcw cwSave               // Save control word
    mov ax, cwSave
    or  ax, 0x400               // Set   bit 10
    and ax, 0xf7ff              // Clear bit 11
    mov ctrlFlags, ax
    fldcw ctrlFlags             // FPU.ctrlWorld.bit[10;11] = 1,0 = ROUND DOWN
    frndint
    fldcw cwSave                // Restore control word
    fsub st(1),st(0)
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
#define M_PI_2_60 7.244019458077122842384326056985109887461e+018 // pow(2,60) * 2*pi
void sincos(double &c, double &s) {
  s = sin(c);
  c = cos(c);
/* This code doesn't pass unittests with new SDK
  double r = fmod(c, M_PI_2_60);
  __asm {
    fld r
    fsincos
    mov eax, DWORD PTR c
    fstp QWORD PTR [eax]
    mov eax, DWORD PTR s
    fstp QWORD PTR [eax]
  }
*/
}
#endif
