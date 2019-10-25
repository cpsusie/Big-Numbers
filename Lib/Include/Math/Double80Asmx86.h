#pragma once

#ifdef IS32BIT

class Double80;

extern const double   _Dmaxi16P1;   // (double)_I16_MAX + 1)
extern const double   _Dmaxi32P1;   // (double)_I32_MAX + 1);
extern const Double80 _D80maxi64P1; // (Double80)_I64_MAX + 1

// x is USHORT
#define FILDUINT16(x) {                  \
if(x <= _I16_MAX)                        \
  __asm {                                \
    __asm fild x                         \
  }                                      \
else                                     \
  __asm {                                \
    __asm and  x, _I16_MAX               \
    __asm fild x                         \
    __asm fadd _Dmaxi16P1                \
  }                                      \
}

// x is UINT
#define FILDUINT32(x) {                  \
if(x <= _I32_MAX)                        \
  __asm {                                \
    __asm fild x                         \
  }                                      \
else                                     \
  __asm {                                \
    __asm and  x, _I32_MAX               \
    __asm fild x                         \
    __asm fadd _Dmaxi32P1                \
  }                                      \
}

// x is UINT64
#define FILDUINT64(x) {                   \
if(x <= _I64_MAX)                         \
  __asm {                                 \
    __asm fild x                          \
  }                                       \
else                                      \
  __asm {                                 \
    __asm lea  eax, x                     \
    __asm and  DWORD PTR[eax+4], _I32_MAX \
    __asm fild QWORD PTR[eax]             \
    __asm fld  TBYTE PTR _D80maxi64P1     \
    __asm fadd                            \
  }                                       \
}




inline void D80FromI16(Double80 &dst, short    x) {
  __asm {
    fild x
    mov eax, dst
    fstp TBYTE PTR[eax]
  }
}

inline void D80FromUI16(Double80 &dst, USHORT x) {
  FILDUINT16(x)
  __asm {
    mov eax, dst
    fstp TBYTE PTR[eax]
  }
}

inline void D80FromI32(Double80 &dst, int x) {
  __asm {
    fild x
    mov eax, dst
    fstp TBYTE PTR[eax]
  }
}

inline void D80FromUI32(Double80 &dst, UINT x) {
  FILDUINT32(x)
  __asm {
    mov eax, dst
    fstp TBYTE PTR[eax]
  }
}

inline void D80FromI64(Double80 &dst, INT64 x) {
  __asm {
    fild x
    mov eax, dst
    fstp TBYTE PTR[eax]
  }
}

inline void D80FromUI64(Double80 &dst, UINT64 x) {
  FILDUINT64(x)
  __asm {
    mov eax, dst
    fstp TBYTE PTR[eax]
  }
}

inline void D80FromFlt(Double80 &dst, float x) {
  __asm {
    fld x
    mov eax, dst
    fstp TBYTE PTR[eax]
  }
}

inline void D80FromDbl(Double80 &dst, double x) {
  __asm {
    fld x
    mov eax, dst
    fstp TBYTE PTR[eax]
  }
}

inline int D80ToI32(const Double80 &x) {
  int result;
  __asm {
    mov eax, x
    fld TBYTE PTR[eax]
    fisttp result
  }
  return result;
}

UINT D80ToUI32(const Double80 &x);

inline INT64 D80ToI64(const Double80 &x) {
  INT64 result;
  __asm {
    mov eax, x
    fld TBYTE PTR[eax]
    fisttp result
  }
  return result;
}

UINT64 D80ToUI64(const Double80 &x);

inline float D80ToFlt(const Double80 &x) { // assume !isnan(x)
  float result;
  __asm {
    mov eax, x
    fld TBYTE PTR[eax]
    fstp result
  }
  return result;
}

inline double D80ToDbl(const Double80 &x) { // assume !isnan(x)
  double result;
  __asm {
    mov eax, x
    fld TBYTE PTR[eax]
    fstp result
  }
  return result;
}

char D80cmpI16(  const Double80 &x, short           y); // return sign(x-y) or 2 if x is nan
char D80cmpUI16( const Double80 &x, USHORT          y); // return sign(x-y) or 2 if x is nan
char D80cmpI32(  const Double80 &x, int             y); // return sign(x-y) or 2 if x is nan
char D80cmpUI32( const Double80 &x, UINT            y); // return sign(x-y) or 2 if x is nan
char D80cmpI64(  const Double80 &x, INT64           y); // return sign(x-y) or 2 if x is nan
char D80cmpUI64( const Double80 &x, UINT64          y); // return sign(x-y) or 2 if x is nan
char D80cmpFlt(  const Double80 &x, float           y); // return sign(x-y) or 2 if x is nan
char D80cmpDbl(  const Double80 &x, double          y); // return sign(x-y) or 2 if x is nan
char D80cmpD80(  const Double80 &x, const Double80 &y); // return sign(x-y) or 2 if x is nan

inline BYTE D80isZero(const Double80 &x) {              // return 1 if x == 0, 0 if x != 0
  BYTE result;
  __asm {
    mov eax, x
    fld TBYTE PTR[eax]
    ftst
    fstsw ax
    sahf
    fstp    st(0)
    sete result;                  // result = (x == 0) ? 1 : 0
  }
  return result;
}


inline void D80addI16(Double80 &dst, short x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fiadd x
    fstp TBYTE PTR[eax]
  }
}

inline void D80subI16(Double80 &dst, short x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fisub x
    fstp TBYTE PTR[eax]
  }
}

inline void D80subrI16(Double80 &dst, short x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fisubr x
    fstp TBYTE PTR[eax]
  }
}

inline void D80mulI16(Double80 &dst, short x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fimul x
    fstp TBYTE PTR[eax]
  }
}

inline void D80divI16(Double80 &dst, short x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fidiv x
    fstp TBYTE PTR[eax]
  }
}

inline void D80divrI16(Double80 &dst, short x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fidivr x
    fstp TBYTE PTR[eax]
  }
}

inline void D80addUI16(Double80 &dst, USHORT x) {
  FILDUINT16(x)
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fadd
    fstp TBYTE PTR[eax]
  }
}
inline void D80subUI16(Double80 &dst, USHORT x) {
  FILDUINT16(x)
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fsubrp st(1), st
    fstp TBYTE PTR[eax]
  }
}
inline void D80subrUI16(Double80 &dst, USHORT x) {
  FILDUINT16(x)
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fsub
    fstp TBYTE PTR[eax]
  }
}
inline void D80mulUI16(Double80 &dst, USHORT x) {
  FILDUINT16(x)
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fmul
    fstp TBYTE PTR[eax]
  }
}
inline void D80divUI16(Double80 &dst, USHORT x) {
  FILDUINT16(x)
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fdivrp st(1), st
    fstp TBYTE PTR[eax]
  }
}
inline void D80divrUI16(Double80 &dst, USHORT x) {
  FILDUINT16(x)
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fdiv
    fstp TBYTE PTR[eax]
  }
}

inline void D80addI32(Double80 &dst, int x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fiadd x
    fstp TBYTE PTR[eax]
  }
}
inline void D80subI32(  Double80 &dst, int x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fisub x
    fstp TBYTE PTR[eax]
  }
}
inline void D80subrI32( Double80 &dst, int x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fisubr x
    fstp TBYTE PTR[eax]
  }
}
inline void D80mulI32(  Double80 &dst, int x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fimul x
    fstp TBYTE PTR[eax]
  }
}
inline void D80divI32(  Double80 &dst, int x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fidiv x
    fstp TBYTE PTR[eax]
  }
}
inline void D80divrI32(Double80 &dst, int x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fidivr x
    fstp TBYTE PTR[eax]
  }
}

inline void D80addUI32( Double80 &dst, UINT   x) {
  FILDUINT32(x)
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fadd
    fstp TBYTE PTR[eax]
  }
}

inline void D80subUI32( Double80 &dst, UINT   x) {
  FILDUINT32(x)
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fsubrp st(1), st
    fstp TBYTE PTR[eax]
  }
}
inline void D80subrUI32(Double80 &dst, UINT   x) {
  FILDUINT32(x)
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fsub
    fstp TBYTE PTR[eax]
  }
}
inline void D80mulUI32( Double80 &dst, UINT   x) {
  FILDUINT32(x)
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fmul
    fstp TBYTE PTR[eax]
  }
}
inline void D80divUI32( Double80 &dst, UINT   x) {
  FILDUINT32(x)
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fdivrp st(1), st
    fstp TBYTE PTR[eax]
  }
}
inline void D80divrUI32(Double80 &dst, UINT   x) {
  FILDUINT32(x)
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fdiv
    fstp TBYTE PTR[eax]
  }
}


inline void D80addI64(Double80 &dst, INT64  x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fild x
    fadd
    fstp TBYTE PTR[eax]
  }
}
inline void D80subI64(Double80 &dst, INT64  x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fild x
    fsub
    fstp TBYTE PTR[eax]
  }
}
inline void D80subrI64(Double80 &dst, INT64  x) {
  __asm {
    fild x
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fsub
    fstp TBYTE PTR[eax]
  }
}
inline void D80mulI64(  Double80 &dst, INT64  x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fild x
    fmul
    fstp TBYTE PTR[eax]
  }
}
inline void D80divI64(Double80 &dst, INT64  x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fild x
    fdiv
    fstp TBYTE PTR[eax]
  }
}
inline void D80divrI64(Double80 &dst, INT64  x) {
  __asm {
    fild x
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fdiv
    fstp TBYTE PTR[eax]
  }
}
inline void D80addUI64(Double80 &dst, UINT64 x) {
  FILDUINT64(x)
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fadd
    fstp TBYTE PTR[eax]
  }
}
inline void D80subUI64(Double80 &dst, UINT64 x) {
  FILDUINT64(x)
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fsubrp st(1), st
    fstp TBYTE PTR[eax]
  }
}
inline void D80subrUI64(Double80 &dst, UINT64 x) {
  FILDUINT64(x)
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fsub
    fstp TBYTE PTR[eax]
  }
}
inline void D80mulUI64(Double80 &dst, UINT64 x) {
  FILDUINT64(x)
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fmul
    fstp TBYTE PTR[eax]
  }
}
inline void D80divUI64(Double80 &dst, UINT64 x) {
  FILDUINT64(x)
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fdivrp st(1), st
    fstp TBYTE PTR[eax]
  }
}
inline void D80divrUI64(Double80 &dst, UINT64 x) {
  FILDUINT64(x)
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fdiv
    fstp TBYTE PTR[eax]
  }
}

inline void D80addFlt(Double80 &dst, float  x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fadd x
    fstp TBYTE PTR[eax]
  }
}
inline void D80subFlt(Double80 &dst, float  x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fsub x
    fstp TBYTE PTR[eax]
  }
}
inline void D80subrFlt(Double80 &dst, float  x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fsubr x
    fstp TBYTE PTR[eax]
  }
}
inline void D80mulFlt(Double80 &dst, float  x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fmul x
    fstp TBYTE PTR[eax]
  }
}
inline void D80divFlt(Double80 &dst, float  x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fdiv x
    fstp TBYTE PTR[eax]
  }
}
inline void D80divrFlt(Double80 &dst, float  x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fdivr x
    fstp TBYTE PTR[eax]
  }
}

inline void D80addDbl(Double80 &dst, double x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fadd x
    fstp TBYTE PTR[eax]
  }
}
inline void D80subDbl(Double80 &dst, double x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fsub x
    fstp TBYTE PTR[eax]
  }
}
inline void D80subrDbl(Double80 &dst, double x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fsubr x
    fstp TBYTE PTR[eax]
  }
}
inline void D80mulDbl(  Double80 &dst, double x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fmul x
    fstp TBYTE PTR[eax]
  }
}
inline void D80divDbl(Double80 &dst, double x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fdiv x
    fstp TBYTE PTR[eax]
  }
}
inline void D80divrDbl(Double80 &dst, double x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fdivr x
    fstp TBYTE PTR[eax]
  }
}

inline void D80addD80(Double80 &dst, const Double80 &x) {
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fadd
    fstp TBYTE PTR[eax]
  }
}
inline void D80subD80(Double80 &dst, const Double80 &x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    mov ecx, DWORD PTR x
    fld TBYTE PTR[ecx]
    fsub
    fstp TBYTE PTR[eax]
  }
}
inline void D80mulD80(Double80 &dst, const Double80 &x) {
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    fmul
    fstp TBYTE PTR[eax]
  }
}
inline void D80divD80(Double80 &dst, const Double80 &x) {
  __asm {
    mov eax, DWORD PTR dst
    fld TBYTE PTR[eax]
    mov ecx, DWORD PTR x
    fld TBYTE PTR[ecx]
    fdiv
    fstp TBYTE PTR[eax]
  }
}

void D80rem(Double80 &dst, const Double80 &x);

inline void D80neg(Double80 &x) {
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]
    fchs
    fstp TBYTE PTR[eax]
  }
}

inline void D80inc(Double80 &x) {
  __asm {
    mov eax, x
    fld TBYTE PTR[eax]
    fld1
    fadd
    fstp TBYTE PTR[eax]
  }
}

inline void D80dec(Double80 &x) {
  __asm {
    mov eax, x
    fld TBYTE PTR[eax]
    fld1
    fsub
    fstp TBYTE PTR[eax]
  }
}

int    D80getExpo10(      const Double80 &x);

inline void D80fabs(Double80 &x) {
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]
    fabs
    fstp TBYTE PTR[eax]
  }
}

inline void D80sqr(Double80 &x) {
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]
    fmul st(0), st(0)
    fstp TBYTE PTR[eax]
  }
}

inline void D80sqrt(Double80 &x) {
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]
    fsqrt
    fstp TBYTE PTR[eax]
  }
}

void   D80sin(            Double80 &x);
void   D80cos(            Double80 &x);
void   D80tan(            Double80 &x);

inline void D80atan(Double80 &x) {
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]
    fld1
    fpatan
    fstp TBYTE PTR[eax]
  }
}

inline void D80atan2(Double80 &y, const Double80 &x) {
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR[eax]
    mov ecx, DWORD PTR x
    fld TBYTE PTR[ecx]
    fpatan
    fstp TBYTE PTR[eax]
  }
}

// inout is c, out s
void   D80sincos(         Double80 &c, Double80       &s);
void   D80exp(            Double80 &x);
void   D80exp10(          Double80 &x);
void   D80exp2(           Double80 &x);

inline void D80log(Double80 &x) {
  __asm {
    fldln2
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]
    fyl2x
    fstp TBYTE PTR[eax]
  }
}

inline void D80log10(Double80 &x) {
  __asm {
    fldlg2
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]
    fyl2x
    fstp TBYTE PTR[eax]
  }
}

inline void D80log2(Double80 &x) {
  __asm {
    fld1
    mov eax, DWORD PTR x
    fld TBYTE PTR[eax]
    fyl2x
    fstp TBYTE PTR[eax]
  }
}

// x = pow(x,y)
void D80pow(Double80 &x, const Double80 &y);

// dst = 2^p
inline void   D80pow2(Double80 &dst, int p) {
  __asm {
    fild p
    fld1
    fscale
    mov eax, dst;
    fstp TBYTE PTR[eax]
    fstp st(0)
  }
}

void   D80floor(          Double80 &x);
void   D80ceil(           Double80 &x);

#endif // IS32BIT
