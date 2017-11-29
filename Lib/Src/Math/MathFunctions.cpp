#include "pch.h"
#include <Limits.h>
#include <Float.h>
#include <Random.h>

int getExpo10(double x) {
  return (x == 0) ? 0 : (int)floor(log10(fabs(x)));
}

bool isNan(double x) {
  return _isnan(x) ? true : false;
}

bool isPInfinity(double x) {
  return _fpclass(x) == _FPCLASS_PINF;
}

bool isNInfinity(double x) {
  return _fpclass(x) == _FPCLASS_NINF;
}

bool isInfinity(double x) {
  return !_finite(x) && !_isnan(x);
}

Real dsign(const Real &x) {
  return (x < 0) ? -1 : (x > 0) ? 1 : 0;
}

Real mypow(const Real &x, const Real &y) {
  if(x < 0) {
    if(y == floor(y)) {
      const int d = getInt(y);
      return (d & 1) ? -pow(-x,y) : pow(-x,y);
    }
  }
  return pow(x, y);
}

double root(double x, double y) { // must be double. not real
  if(x < 0) {
    if(y == floor(y)) {
      const int d = getInt(y);
      if((d & 1)) {
        return -pow(-x, 1.0/y);
      }
    }
  }
  return pow(x, 1.0/y);
}

#ifdef IS32BIT
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
  double r = fmod(c, M_PI_2_60);
  __asm {
    fld r
    fsincos
    mov eax, DWORD PTR c
    fstp QWORD PTR [eax]
    mov eax, DWORD PTR s
    fstp QWORD PTR [eax]
  }

}
#endif

Real binomial(const Real &n, const Real &k) {
  return fac(n)/(fac(k)*fac(n-k));
}

Real dmax(const Real &x1, const Real &x2) {
  return (x1 > x2) ? x1 : x2;
}

Real dmin(const Real &x1, const Real &x2) {
  return (x1 < x2) ? x1 : x2;
}

int dmax(int x1, int x2) {
  return (x1 > x2) ? x1 : x2;
}

int dmin(int x1, int x2) {
  return (x1 < x2) ? x1 : x2;
}

Real randReal() {
#ifndef LONGDOUBLE
  return randDouble();
#else
  return randDouble80();
#endif // LONGDOUBLE
}

Real randReal(const Real &lower, const Real &upper) {
  return randReal() * (upper-lower) + lower;
}

Real randomGaussian(const Real &mean, const Real &s) {
  return _standardRandomGenerator->nextGaussian(getDouble(mean),getDouble(s));
}

void setToRandom(Real &r) {
  r = randReal();
}

Real poly(const Real &x, int degree, const Real *coef) {
  const Real *cp = coef + degree;
  Real result = *(cp--);
  while(cp >= coef) {
    result = result * x + *(cp--);
  }
  return result;
}

Real poly1(const Real &x, int degree, const Real *coef) {
  Real result = *coef;
  for(const Real *last = coef + degree; coef++ < last;) {
    result = result * x + *coef;
  }
  return result;
}
