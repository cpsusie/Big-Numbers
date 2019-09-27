#pragma once

#include <BasicIncludes.h>
#include <limits>
#include <ByteStream.h>
#include <Packer.h>
#include <Random.h>
#include "PragmaLib.h"

#pragma check_stack(off)

class Double80;

#ifdef IS64BIT
extern "C" {
void   D80FromI16(        Double80 &s  , const short    &x  );
void   D80FromUI16(       Double80 &s  , USHORT          x  );
void   D80FromI32(        Double80 &s  , const int      &x  );
void   D80FromUI32(       Double80 &s  , UINT            x  );
void   D80FromI64(        Double80 &s  , const INT64    &x  );
void   D80FromUI64(       Double80 &s  , UINT64          x  );
void   D80FromFlt(        Double80 &s  , const float    &x  );
void   D80FromDbl(        Double80 &s  , const double   &x  );
int    D80ToI32(          const Double80 &x);
UINT   D80ToUI32(         const Double80 &x);
INT64  D80ToI64(          const Double80 &x);
UINT64 D80ToUI64(         const Double80 &x);
float  D80ToFlt(          const Double80 &x);
double D80ToDbl(          const Double80 &x);
int    D80cmpI16(         const Double80 &x, const short    &y);
int    D80cmpUI16(        const Double80 &x, USHORT          y);
int    D80cmpI32(         const Double80 &x, const int      &y);
int    D80cmpUI32(        const Double80 &x, UINT            y);
int    D80cmpI64(         const Double80 &x, const INT64    &y);
int    D80cmpUI64(        const Double80 &x, UINT64          y);
int    D80cmpFlt(         const Double80 &x, const float    &y);
int    D80cmpDbl(         const Double80 &x, const double   &y);
int    D80cmpD80(         const Double80 &x, const Double80 &y);
int    D80isZero(         const Double80 &x);
void   D80addI16(         Double80 &dst, const short    &x);
void   D80subI16(         Double80 &dst, const short    &x);
void   D80subrI16(        Double80 &dst, const short    &x);
void   D80mulI16(         Double80 &dst, const short    &x);
void   D80divI16(         Double80 &dst, const short    &x);
void   D80divrI16(        Double80 &dst, const short    &x);
void   D80addUI16(        Double80 &dst, USHORT          x);
void   D80subUI16(        Double80 &dst, USHORT          x);
void   D80subrUI16(       Double80 &dst, USHORT          x);
void   D80mulUI16(        Double80 &dst, USHORT          x);
void   D80divUI16(        Double80 &dst, USHORT          x);
void   D80divrUI16(       Double80 &dst, USHORT          x);
void   D80addI32(         Double80 &dst, const int      &x);
void   D80subI32(         Double80 &dst, const int      &x);
void   D80subrI32(        Double80 &dst, const int      &x);
void   D80mulI32(         Double80 &dst, const int      &x);
void   D80divI32(         Double80 &dst, const int      &x);
void   D80divrI32(        Double80 &dst, const int      &x);
void   D80addUI32(        Double80 &dst, UINT            x);
void   D80subUI32(        Double80 &dst, UINT            x);
void   D80subrUI32(       Double80 &dst, UINT            x);
void   D80mulUI32(        Double80 &dst, UINT            x);
void   D80divUI32(        Double80 &dst, UINT            x);
void   D80divrUI32(       Double80 &dst, UINT            x);
void   D80addI64(         Double80 &dst, const INT64    &x);
void   D80subI64(         Double80 &dst, const INT64    &x);
void   D80subrI64(        Double80 &dst, const INT64    &x);
void   D80mulI64(         Double80 &dst, const INT64    &x);
void   D80divI64(         Double80 &dst, const INT64    &x);
void   D80divrI64(        Double80 &dst, const INT64    &x);
void   D80addUI64(        Double80 &dst, UINT64          x);
void   D80subUI64(        Double80 &dst, UINT64          x);
void   D80subrUI64(       Double80 &dst, UINT64          x);
void   D80mulUI64(        Double80 &dst, UINT64          x);
void   D80divUI64(        Double80 &dst, UINT64          x);
void   D80divrUI64(       Double80 &dst, UINT64          x);
void   D80addFlt(         Double80 &dst, const float    &x);
void   D80subFlt(         Double80 &dst, const float    &x);
void   D80subrFlt(        Double80 &dst, const float    &x);
void   D80mulFlt(         Double80 &dst, const float    &x);
void   D80divFlt(         Double80 &dst, const float    &x);
void   D80divrFlt(        Double80 &dst, const float    &x);
void   D80addDbl(         Double80 &dst, const double   &x);
void   D80subDbl(         Double80 &dst, const double   &x);
void   D80subrDbl(        Double80 &dst, const double   &x);
void   D80mulDbl(         Double80 &dst, const double   &x);
void   D80divDbl(         Double80 &dst, const double   &x);
void   D80divrDbl(        Double80 &dst, const double   &x);
void   D80addD80(         Double80 &dst, const Double80 &x);
void   D80subD80(         Double80 &dst, const Double80 &x);
void   D80mulD80(         Double80 &dst, const Double80 &x);
void   D80divD80(         Double80 &dst, const Double80 &x);
void   D80rem(            Double80 &dst, const Double80 &x);
void   D80neg(            Double80 &x);
void   D80inc(            Double80 &x);
void   D80dec(            Double80 &x);
int    D80getExpo10(      const Double80 &x);
void   D80fabs(           Double80 &x);
void   D80sqr(            Double80 &x);
void   D80sqrt(           Double80 &x);
void   D80sin(            Double80 &x);
void   D80cos(            Double80 &x);
void   D80tan(            Double80 &x);
void   D80atan(           Double80 &x);
void   D80atan2(          Double80 &y, const Double80 &x);
// inout is c, out s
void   D80sincos(         Double80 &c, Double80       &s);
void   D80exp(            Double80 &x);
void   D80exp10(          Double80 &x);
void   D80exp2(           Double80 &x);
void   D80log(            Double80 &x);
void   D80log10(          Double80 &x);
void   D80log2(           Double80 &x);
// x = pow(x,y)
void   D80pow(            Double80 &x, const Double80 &y);
// dst = 2^p
void   D80pow2(           Double80 &dst, const int &p);
void   D80floor(          Double80 &x);
void   D80ceil(           Double80 &x);
}

#else // IS32BIT

extern const double   _Dmaxi16P1;
extern const double   _Dmaxi32P1;
extern const Double80 _D80maxi64P1;

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
    __asm fld  _D80maxi64P1               \
    __asm fadd                            \
  }                                       \
}

#endif // IS32BIT

class Pow10Calculator {
public:
  virtual Double80 pow10(int p) const = 0;
#ifdef _DEBUG
  void dumpAll() const;
#endif // _DEBUG
};

class Double80 {
private:
  BYTE m_value[10]; // Must be the first field in the class

  static Pow10Calculator *s_p10Calculator;

#ifdef IS32BIT
  static inline void D80FromFlt(Double80 &s, float x) {
    __asm {
      fld x
      mov eax, s
      fstp TBYTE PTR[eax]
    }
  }

  static inline void D80FromDbl(Double80 &s, double x) {
    __asm {
      fld x
      mov eax, s
      fstp TBYTE PTR[eax]
    }
  }

#endif // // IS32BIT

public:
  inline Double80() {
  }

  inline Double80(long x) {
    *this = (Double80)(int)(x);
  }

  inline Double80(ULONG x) {
    *this = (Double80)(UINT)(x);
  }

  explicit inline Double80(const BYTE *bytes) {
    memcpy(&m_value, bytes, sizeof(m_value));
  }

  Double80(float  x);
  Double80(double x);

#ifdef IS32BIT
  inline Double80(short x) {
    __asm {
      fild x
      mov eax, this
      fstp TBYTE PTR [eax]
    }
  }
  inline Double80(USHORT x) {
    FILDUINT16(x)
    __asm {
      mov eax, this
      fstp TBYTE PTR [eax]
    }
  }
  inline Double80(int x) {
    __asm {
      fild x
      mov eax, this
      fstp TBYTE PTR [eax]
    }
  }
  inline Double80(UINT x) {
    FILDUINT32(x)
    __asm {
      mov eax, this
      fstp TBYTE PTR [eax]
    }
  }
  inline Double80(INT64 x) {
    __asm {
      fild x
      mov eax, this
      fstp TBYTE PTR [eax]
    }
  }
  Double80(UINT64 x) {
    FILDUINT64(x)
    __asm {
      mov eax, this
      fstp TBYTE PTR [eax]
    }
  }

  // x == 0 ? 0 : floor(log10(|x|))
  static int    getExpo10(const Double80 &x);

  // prefix-form
  inline Double80 &operator++() {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fld1
      fadd
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  // prefix-form
  inline Double80 &operator--() {
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fld1
      fsub
      fstp TBYTE PTR [eax]
    }
    return *this;
  }
  // postfix-form
  inline Double80 operator++(int) {
    Double80 result(*this);
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fld1
      fadd
      fstp TBYTE PTR [eax]
    }
    return result;
  }
  // postfix-form
  inline Double80 operator--(int) {
    Double80 result(*this);
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      fld1
      fsub
      fstp TBYTE PTR [eax]
    }
    return result;
  }

  inline bool isZero() const {
    bool result;
    __asm {
      mov eax, this
      fld TBYTE PTR [eax]
      ftst
      fstsw ax
      sahf
      fstp    st(0)
      sete result;                  // result = (*this == 0) ? 1 : 0
    }
    return result;
  }
  static inline Double80 pow2(int p) {
    Double80 result;
    __asm {
      fild p
      fld1
      fscale
      fstp result
      fstp st(0)
    }
    return result;
  }

#else // IS64BIT

  inline Double80(short  x) { D80FromI16( *this, x); }
  inline Double80(USHORT x) { D80FromUI16(*this, x); }
  inline Double80(int    x) { D80FromI32( *this, x); }
  inline Double80(UINT   x) { D80FromUI32(*this, x); }
  inline Double80(INT64  x) { D80FromI64( *this, x); }
  inline Double80(UINT64 x) { D80FromUI64(*this, x); }

// x == 0 ? 0 : floor(log10(|x|))
  static inline int getExpo10(const Double80 &x) { return D80getExpo10(x); }
  // prefix-form
  inline Double80 &operator++()   {  D80inc(*this);    return *this;  }
  inline Double80 &operator--()   {  D80dec(*this);    return *this;  }
  // postfix-form
  inline Double80 operator++(int) {  Double80 result(*this);  D80inc(*this);  return result;  }
  inline Double80 operator--(int) {  Double80 result(*this);  D80dec(*this);  return result;  }

  inline bool isZero() const      {  return D80isZero(*this) ? true : false;  }
  static inline Double80 pow2(int p) { Double80 result; D80pow2(result,p); return result; }

#endif // IS64BIT

  friend inline UINT64 getSignificand( const Double80 &x) {
    return *((UINT64*)x.m_value);
  }
  friend inline UINT   getExponent(    const Double80 &x) {
    return (*(UINT*)(x.m_value+8)) & 0x7fff;
  }
  friend inline BYTE   getSign(        const Double80 &x) {
    return x.m_value[9] & 0x80;
  }

  inline bool isPositive() const { return (m_value[9] & 0x80) == 0; }
  inline bool isNegative() const { return (m_value[9] & 0x80) != 0; }

  ULONG hashCode() const;

  // returns one of
  // _FPCLASS_SNAN  0x0001   signaling NaN
  // _FPCLASS_QNAN  0x0002   quiet NaN
  // _FPCLASS_NINF  0x0004   negative infinity
  // _FPCLASS_NN    0x0008   negative normal
  // _FPCLASS_ND    0x0010   negative denormal
  // _FPCLASS_NZ    0x0020   -0
  // _FPCLASS_PZ    0x0040   +0
  // _FPCLASS_PD    0x0080   positive denormal
  // _FPCLASS_PN    0x0100   positive normal
  // _FPCLASS_PINF  0x0200   positive infinity
  friend int _fpclass(const Double80 &x);

  inline void save(ByteOutputStream &s) const {
    s.putBytes((BYTE*)m_value, sizeof(m_value));
  }

  inline void load(ByteInputStream &s) {
    s.getBytesForced((BYTE*)m_value, sizeof(m_value));
  }

  static inline Double80       pow10(int p) {
    return s_p10Calculator->pow10(p);
  }
  static inline Pow10Calculator *getPow10Calculator() {
    return s_p10Calculator;
  }
  static inline Pow10Calculator *setPow10Calculator(Pow10Calculator *newValue) {
    Pow10Calculator *oldValue = s_p10Calculator;
    s_p10Calculator = newValue;
    return oldValue;
  }
  static const Double80 _0;                               // = 0
  static const Double80 _05;                              // 0.5
  static const Double80 _1;                               // = 1
  static const Double80 _DBL80_EPSILON;                   // Smallest such that 1.0+DBL80_EPSILON != 1.0 (=1.08420217248550443e-019)
  static const Double80 _DBL80_MIN;                       // Min positive value (=3.36210314311209209e-4932)
  static const Double80 _DBL80_MAX;                       // Max value          (=1.18973149535723227e+4932)
  static const Double80 _DBL80_QNAN;                      // non-signaling NaN (quiet NaN)
  static const Double80 _DBL80_SNAN;                      // signaling NaN
  static const Double80 _DBL80_PINF;                      // +infinity;
  static const Double80 _DBL80_NINF;                      // -infinity;
  static const Double80 _DBL80_TRUE_MIN;                  // min positive value (3.64519953188247460325e-4951)

  static void initClass();
};

extern const Double80 DBL80_PI;                        // = 3.1415926535897932384626433
extern const Double80 DBL80_PI_05;                     // pi/2






#ifdef IS32BIT
inline int getInt(const Double80 &x) {
  int result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fisttp result
  }
  return result;
}
UINT   getUint(   const Double80 &x);










inline INT64 getInt64(const Double80 &x) {
  INT64 result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fisttp result
  }
  return result;
}
UINT64 getUint64( const Double80 &x);










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











inline Double80 operator+(const Double80 &x, short         y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fiadd y
    fstp result
  }
  return result;
}










inline Double80 operator-(const Double80 &x, short         y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fisub y
    fstp result
  }
  return result;
}










inline Double80 operator*(const Double80 &x, short         y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fimul y
    fstp result
  }
  return result;
}










inline Double80 operator/(const Double80 &x, short         y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fidiv y
    fstp result
  }
  return result;
}










inline Double80 operator+(const Double80 &x, USHORT        y) {
  Double80 result;
  FILDUINT16(y)
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fadd
    fstp result
  }
  return result;
}









inline Double80 operator-(const Double80 &x, USHORT        y) {
  Double80 result;
  FILDUINT16(y)
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fsubrp st(1),st
    fstp result
  }
  return result;
}









inline Double80 operator*(const Double80 &x, USHORT        y) {
  Double80 result;
  FILDUINT16(y)
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fmul
    fstp result
  }
  return result;
}









inline Double80 operator/(const Double80 &x, USHORT        y) {
  Double80 result;
  FILDUINT16(y)
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fdivrp st(1),st
    fstp result
  }
  return result;
}









inline Double80 operator+(const Double80 &x, int           y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fiadd y
    fstp result
  }
  return result;
}










inline Double80 operator-(const Double80 &x, int           y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fisub y
    fstp result
  }
  return result;
}










inline Double80 operator*(const Double80 &x, int           y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fimul y
    fstp result
  }
  return result;
}










inline Double80 operator/(const Double80 &x, int           y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fidiv y
    fstp result
  }
  return result;
}










inline Double80 operator+(const Double80 &x, UINT          y) {
  Double80 result;
  FILDUINT32(y)
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fadd
    fstp result
  }
  return result;
}









inline Double80 operator-(const Double80 &x, UINT          y) {
  Double80 result;
  FILDUINT32(y)
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fsubrp st(1),st
    fstp result
  }
  return result;
}









inline Double80 operator*(const Double80 &x, UINT          y) {
  Double80 result;
  FILDUINT32(y)
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fmul
    fstp result
  }
  return result;
}









inline Double80 operator/(const Double80 &x, UINT          y) {
  Double80 result;
  FILDUINT32(y)
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fdivrp st(1),st
    fstp result
  }
  return result;
}









inline Double80 operator+(const Double80 &x, long          y) { return x + (int )y; }
inline Double80 operator-(const Double80 &x, long          y) { return x - (int )y; }
inline Double80 operator*(const Double80 &x, long          y) { return x * (int )y; }
inline Double80 operator/(const Double80 &x, long          y) { return x / (int )y; }
inline Double80 operator+(const Double80 &x, ULONG         y) { return x + (UINT)y; }
inline Double80 operator-(const Double80 &x, ULONG         y) { return x - (UINT)y; }
inline Double80 operator*(const Double80 &x, ULONG         y) { return x * (UINT)y; }
inline Double80 operator/(const Double80 &x, ULONG         y) { return x / (UINT)y; }












inline Double80 operator+(const Double80 &x, INT64         y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fild y
    fadd
    fstp result
  }
  return result;
}









inline Double80 operator-(const Double80 &x, INT64         y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fild y
    fsub
    fstp result
  }
  return result;
}









inline Double80 operator*(const Double80 &x, INT64         y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fild y
    fmul
    fstp result
  }
  return result;
}









inline Double80 operator/(const Double80 &x, INT64         y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fild y
    fdiv
    fstp result
  }
  return result;
}









inline Double80 operator+(const Double80 &x, UINT64        y) {
  Double80 result;
  FILDUINT64(y)
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fadd
    fstp result
  }
  return result;
}









inline Double80 operator-(const Double80 &x, UINT64        y) {
  Double80 result;
  FILDUINT64(y)
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fsubrp st(1),st
    fstp result
  }
  return result;
}









inline Double80 operator*(const Double80 &x, UINT64        y) {
  Double80 result;
  FILDUINT64(y)
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fmul
    fstp result
  }
  return result;
}









inline Double80 operator/(const Double80 &x, UINT64        y) {
  Double80 result;
  FILDUINT64(y)
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fdivrp st(1),st
    fstp result
  }
  return result;
}









inline Double80 operator+(const Double80 &x, float         y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fadd y
    fstp result
  }
  return result;
}










inline Double80 operator-(const Double80 &x, float         y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fsub y
    fstp result
  }
  return result;
}










inline Double80 operator*(const Double80 &x, float         y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fmul y
    fstp result
  }
  return result;
}










inline Double80 operator/(const Double80 &x, float         y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fdiv y
    fstp result
  }
  return result;
}










inline Double80 operator+(const Double80 &x, double        y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fadd y
    fstp result
  }
  return result;
}










inline Double80 operator-(const Double80 &x, double        y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fsub y
    fstp result
  }
  return result;
}










inline Double80 operator*(const Double80 &x, double        y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fmul y
    fstp result
  }
  return result;
}










inline Double80 operator/(const Double80 &x, double        y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fdiv y
    fstp result
  }
  return result;
}










inline Double80 operator+(const Double80 &x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fadd
    fstp result
  }
  return result;
}








inline Double80 operator-(const Double80 &x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fsub
    fstp result
  }
  return result;
}








inline Double80 operator-(const Double80 &x) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fchs
    fstp result
  }
  return result;
}










inline Double80 operator*(const Double80 &x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fmul
    fstp result
  }
  return result;
}








inline Double80 operator/(const Double80 &x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fdiv
    fstp result
  }
  return result;
}








inline Double80 operator+(short           x, const Double80 &y) { return y + x; }
inline Double80 operator-(short           x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fisubr x
    fstp result
  }
  return result;
}









inline Double80 operator*(short           x, const Double80 &y) { return y * x; }
inline Double80 operator/(short           x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fidivr x
    fstp result
  }
  return result;
}









inline Double80 operator+(USHORT          x, const Double80 &y) { return y + x; }
inline Double80 operator-(USHORT          x, const Double80 &y) {
  Double80 result;
  FILDUINT16(x)
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fsubp st(1), st
    fstp result
  }
  return result;
}








inline Double80 operator*(USHORT          x, const Double80 &y) { return y * x; }
inline Double80 operator/(USHORT          x, const Double80 &y) {
  Double80 result;
  FILDUINT16(x)
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fdivp st(1), st
    fstp result
  }
  return result;
}








inline Double80 operator+(int             x, const Double80 &y) { return y + x; }
inline Double80 operator-(int             x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fisubr x
    fstp result
  }
  return result;
}









inline Double80 operator*(int             x, const Double80 &y) { return y * x; }
inline Double80 operator/(int             x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fidivr x
    fstp result
  }
  return result;
}









inline Double80 operator+(UINT            x, const Double80 &y) { return y + x; }
inline Double80 operator-(UINT            x, const Double80 &y) {
  Double80 result;
  FILDUINT32(x)
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fsubp st(1), st
    fstp result
  }
  return result;
}








inline Double80 operator*(UINT            x, const Double80 &y) { return y * x; }
inline Double80 operator/(UINT            x, const Double80 &y) {
  Double80 result;
  FILDUINT32(x)
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fdivp st(1), st
    fstp result
  }
  return result;
}








inline Double80 operator+(long            x, const Double80 &y) { return y + x; }
inline Double80 operator-(long            x, const Double80 &y) { return (int)x - y; }
inline Double80 operator*(long            x, const Double80 &y) { return y * x; }
inline Double80 operator/(long            x, const Double80 &y) { return (int)x / y; }
inline Double80 operator+(ULONG           x, const Double80 &y) { return y + x; }
inline Double80 operator-(ULONG           x, const Double80 &y) { return (UINT)x - y; }
inline Double80 operator*(ULONG           x, const Double80 &y) { return y * x; }
inline Double80 operator/(ULONG           x, const Double80 &y) { return (UINT)x / y; }












inline Double80 operator+(INT64           x, const Double80 &y) { return y + x; }
inline Double80 operator-(INT64           x, const Double80 &y) {
  Double80 result;
  __asm {
    fild x
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fsub
    fstp result
  }
  return result;
}








inline Double80 operator*(INT64           x, const Double80 &y) { return y * x; }
inline Double80 operator/(INT64           x, const Double80 &y) {
  Double80 result;
  __asm {
    fild x
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fdiv
    fstp result
  }
  return result;
}








inline Double80 operator+(UINT64          x, const Double80 &y) { return y + x; }
inline Double80 operator-(UINT64          x, const Double80 &y) {
  Double80 result;
  FILDUINT64(x)
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fsubp st(1), st
    fstp result
  }
  return result;
}








inline Double80 operator*(UINT64          x, const Double80 &y) { return y * x; }
inline Double80 operator/(UINT64          x, const Double80 &y) {
  Double80 result;
  FILDUINT64(x)
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fdivp st(1), st
    fstp result
  }
  return result;
}








inline Double80 operator+(float           x, const Double80 &y) { return y + x; }
inline Double80 operator-(float           x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fsubr x
    fstp result
  }
  return result;
}









inline Double80 operator*(float           x, const Double80 &y) { return y * x; }
inline Double80 operator/(float           x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fdivr x
    fstp result
  }
  return result;
}









inline Double80 operator+(double          x, const Double80 &y) { return y + x; }
inline Double80 operator-(double          x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fsubr x
    fstp result
  }
  return result;
}









inline Double80 operator*(double          x, const Double80 &y) { return y * x; }
inline Double80 operator/(double          x, const Double80 &y) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    fdivr x
    fstp result
  }
  return result;
}









inline Double80 &operator+=(Double80 &x, short             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fiadd y
    fstp TBYTE PTR [eax]
  }
  return x;
}











inline Double80 &operator-=(Double80 &x, short             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fisub y
    fstp TBYTE PTR [eax]
  }
  return x;
}











inline Double80 &operator*=(Double80 &x, short             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fimul y
    fstp TBYTE PTR [eax]
  }
  return x;
}











inline Double80 &operator/=(Double80 &x, short             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fidiv y
    fstp TBYTE PTR [eax]
  }
  return x;
}











inline Double80 &operator+=(Double80 &x, USHORT            y) {
  FILDUINT16(y)
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fadd
    fstp TBYTE PTR [eax]
  }
  return x;
}










inline Double80 &operator-=(Double80 &x, USHORT            y) {
  FILDUINT16(y)
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fsubrp st(1), st
    fstp TBYTE PTR [eax]
  }
  return x;
}










inline Double80 &operator*=(Double80 &x, USHORT            y) {
  FILDUINT16(y)
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fmul
    fstp TBYTE PTR [eax]
  }
  return x;
}










inline Double80 &operator/=(Double80 &x, USHORT            y) {
  FILDUINT16(y)
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fdivrp st(1),st
    fstp TBYTE PTR [eax]
  }
  return x;
}










inline Double80 &operator+=(Double80 &x, int               y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fiadd y
    fstp TBYTE PTR [eax]
  }
  return x;
}











inline Double80 &operator-=(Double80 &x, int               y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fisub y
    fstp TBYTE PTR [eax]
  }
  return x;
}











inline Double80 &operator*=(Double80 &x, int               y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fimul y
    fstp TBYTE PTR [eax]
  }
  return x;
}











inline Double80 &operator/=(Double80 &x, int               y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fidiv y
    fstp TBYTE PTR [eax]
  }
  return x;
}











inline Double80 &operator+=(Double80 &x, UINT              y) {
  FILDUINT32(y)
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fadd
    fstp TBYTE PTR [eax]
  }
  return x;
}










inline Double80 &operator-=(Double80 &x, UINT              y) {
  FILDUINT32(y)
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fsubrp st(1), st
    fstp TBYTE PTR [eax]
  }
  return x;
}










inline Double80 &operator*=(Double80 &x, UINT              y) {
  FILDUINT32(y)
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fmul
    fstp TBYTE PTR [eax]
  }
  return x;
}










inline Double80 &operator/=(Double80 &x, UINT              y) {
  FILDUINT32(y)
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fdivrp st(1),st
    fstp TBYTE PTR [eax]
  }
  return x;
}










inline Double80 &operator+=(Double80 &x, long  y) { return x += (int )y; }
inline Double80 &operator-=(Double80 &x, long  y) { return x -= (int )y; }
inline Double80 &operator*=(Double80 &x, long  y) { return x *= (int )y; }
inline Double80 &operator/=(Double80 &x, long  y) { return x /= (int )y; }
inline Double80 &operator+=(Double80 &x, ULONG y) { return x += (UINT)y; }
inline Double80 &operator-=(Double80 &x, ULONG y) { return x -= (UINT)y; }
inline Double80 &operator*=(Double80 &x, ULONG y) { return x *= (UINT)y; }
inline Double80 &operator/=(Double80 &x, ULONG y) { return x /= (UINT)y; }












inline Double80 &operator+=(Double80 &x, INT64             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fild y
    fadd
    fstp TBYTE PTR [eax]
  }
  return x;
}










inline Double80 &operator-=(Double80 &x, INT64             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fild y
    fsub
    fstp TBYTE PTR [eax]
  }
  return x;
}










inline Double80 &operator*=(Double80 &x, INT64             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fild y
    fmul
    fstp TBYTE PTR [eax]
  }
  return x;
}










inline Double80 &operator/=(Double80 &x, INT64             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fild y
    fdiv
    fstp TBYTE PTR [eax]
  }
  return x;
}










inline Double80 &operator+=(Double80 &x, UINT64            y) {
  FILDUINT64(y)
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fadd
    fstp TBYTE PTR [eax]
  }
  return x;
}










inline Double80 &operator-=(Double80 &x, UINT64            y) {
  FILDUINT64(y)
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fsubrp st(1), st
    fstp TBYTE PTR [eax]
  }
  return x;
}










inline Double80 &operator*=(Double80 &x, UINT64            y) {
  FILDUINT64(y)
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fmul
    fstp TBYTE PTR [eax]
  }
  return x;
}










inline Double80 &operator/=(Double80 &x, UINT64            y) {
  FILDUINT64(y)
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fdivrp st(1), st
    fstp TBYTE PTR [eax]
  }
  return x;
}










inline Double80 &operator+=(Double80 &x, float             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fadd y
    fstp TBYTE PTR [eax]
  }
  return x;
}











inline Double80 &operator-=(Double80 &x, float             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fsub y
    fstp TBYTE PTR [eax]
  }
  return x;
}











inline Double80 &operator*=(Double80 &x, float             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fmul y
    fstp TBYTE PTR [eax]
  }
  return x;
}











inline Double80 &operator/=(Double80 &x, float             y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fdiv y
    fstp TBYTE PTR [eax]
  }
  return x;
}











inline Double80 &operator+=(Double80 &x, double            y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fadd y
    fstp TBYTE PTR [eax]
  }
  return x;
}











inline Double80 &operator-=(Double80 &x, double            y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fsub y
    fstp TBYTE PTR [eax]
  }
  return x;
}











inline Double80 &operator*=(Double80 &x, double            y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fmul y
    fstp TBYTE PTR [eax]
  }
  return x;
}











inline Double80 &operator/=(Double80 &x, double            y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    fdiv y
    fstp TBYTE PTR [eax]
  }
  return x;
}











inline Double80 &operator+=(Double80 &x, const Double80   &y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov ebx, DWORD PTR y
    fld TBYTE PTR [ebx]
    fadd
    fstp TBYTE PTR [eax]
  }
  return x;
}









inline Double80 &operator-=(Double80 &x, const Double80   &y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov ebx, DWORD PTR y
    fld TBYTE PTR [ebx]
    fsub
    fstp TBYTE PTR [eax]
  }
  return x;
}









inline Double80 &operator*=(Double80 &x, const Double80   &y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov ebx, DWORD PTR y
    fld TBYTE PTR [ebx]
    fmul
    fstp TBYTE PTR [eax]
  }
  return x;
}









inline Double80 &operator/=(Double80 &x, const Double80   &y) {
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov ebx, DWORD PTR y
    fld TBYTE PTR [ebx]
    fdiv
    fstp TBYTE PTR [eax]
  }
  return x;
}









inline bool operator==(const Double80 &x, short            y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    sete  result                // result = (st(0) == y) ? 1 : 0
  }
  return result;
}








inline bool operator!=(const Double80 &x, short            y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    setne result                // result = (st(0) != y) ? 1 : 0
  }
  return result;
}








inline bool operator<=(const Double80 &x, short            y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    setbe result                // result = (st(0) <= y) ? 1 : 0
  }
  return result;
}








inline bool operator>=(const Double80 &x, short            y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    setae result                // result = (st(0) >= y) ? 1 : 0
  }
  return result;
}








inline bool operator< (const Double80 &x, short            y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    setb  result                // result = (st(0) <  y) ? 1 : 0
  }
  return result;
}








inline bool operator> (const Double80 &x, short            y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    seta  result                // result = (st(0) >  y) ? 1 : 0
  }
  return result;
}








inline bool operator==(const Double80 &x, USHORT           y) {
  bool result;
  FILDUINT16(y)                 // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    sete  result                // result = (st(0) == st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}








inline bool operator!=(const Double80 &x, USHORT           y) {
  bool result;
  FILDUINT16(y)                 // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    setne result                // result = (st(0) != st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}








inline bool operator<=(const Double80 &x, USHORT           y) {
  bool result;
  FILDUINT16(y)                 // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    setbe result                // result = (st(0) <= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}








inline bool operator>=(const Double80 &x, USHORT           y) {
  bool result;
  FILDUINT16(y)                 // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    setae result                // result = (st(0) >= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}








inline bool operator< (const Double80 &x, USHORT           y) {
  bool result;
  FILDUINT16(y)                 // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    setb  result                // result = (st(0) <  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}








inline bool operator> (const Double80 &x, USHORT           y) {
  bool result;
  FILDUINT16(y)                 // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    seta  result                // result = (st(0) >  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}








inline bool operator==(const Double80 &x, int              y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    sete  result                // result = (st(0) == y) ? 1 : 0
  }
  return result;
}








inline bool operator!=(const Double80 &x, int              y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    setne result                // result = (st(0) != y) ? 1 : 0
  }
  return result;
}








inline bool operator<=(const Double80 &x, int              y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    setbe result                // result = (st(0) <= y) ? 1 : 0
  }
  return result;
}








inline bool operator>=(const Double80 &x, int              y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    setae result                // result = (st(0) >= y) ? 1 : 0
  }
  return result;
}








inline bool operator< (const Double80 &x, int              y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    setb  result                // result = (st(0) <  y) ? 1 : 0
  }
  return result;
}








inline bool operator> (const Double80 &x, int              y) {
  bool result;
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    ficomp y                    // compare and pop x
    fnstsw ax
    sahf
    seta  result                // result = (st(0) >  y) ? 1 : 0
  }
  return result;
}








inline bool operator==(const Double80 &x, UINT             y) {
  bool result;
  FILDUINT32(y)                 // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    sete  result                // result = (st(0) == st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}








inline bool operator!=(const Double80 &x, UINT             y) {
  bool result;
  FILDUINT32(y)                 // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    setne result                // result = (st(0) != st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}








inline bool operator<=(const Double80 &x, UINT             y) {
  bool result;
  FILDUINT32(y)                 // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    setbe result                // result = (st(0) <= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}








inline bool operator>=(const Double80 &x, UINT             y) {
  bool result;
  FILDUINT32(y)                 // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    setae result                // result = (st(0) >= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}








inline bool operator< (const Double80 &x, UINT             y) {
  bool result;
  FILDUINT32(y)                 // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    setb  result                // result = (st(0) <  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}








inline bool operator> (const Double80 &x, UINT             y) {
  bool result;
  FILDUINT32(y)                 // load y
  __asm {
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]
    fcomip st, st(1)            // compare and pop x
    seta  result                // result = (st(0) >  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}








inline bool operator==(const Double80 &x, long     y) { return x == (int )y; }
inline bool operator!=(const Double80 &x, long     y) { return x != (int )y; }
inline bool operator<=(const Double80 &x, long     y) { return x <= (int )y; }
inline bool operator>=(const Double80 &x, long     y) { return x >= (int )y; }
inline bool operator< (const Double80 &x, long     y) { return x <  (int )y; }
inline bool operator> (const Double80 &x, long     y) { return x >  (int )y; }
inline bool operator==(const Double80 &x, ULONG    y) { return x == (UINT)y; }
inline bool operator!=(const Double80 &x, ULONG    y) { return x != (UINT)y; }
inline bool operator<=(const Double80 &x, ULONG    y) { return x <= (UINT)y; }
inline bool operator>=(const Double80 &x, ULONG    y) { return x >= (UINT)y; }
inline bool operator< (const Double80 &x, ULONG    y) { return x <  (UINT)y; }
inline bool operator> (const Double80 &x, ULONG    y) { return x >  (UINT)y; }








inline bool operator==(const Double80 &x, const INT64     &y) {
  bool result;
  __asm {
    mov eax, y                  // load y
    fild QWORD PTR [eax]
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    sete  result                // result = (st(0) == st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}







inline bool operator!=(const Double80 &x, const INT64     &y) {
  bool result;
  __asm {
    mov eax, y                  // load y
    fild QWORD PTR [eax]
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setne result                // result = (st(0) != st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}







inline bool operator<=(const Double80 &x, const INT64     &y) {
  bool result;
  __asm {
    mov eax, y                  // load y
    fild QWORD PTR [eax]
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setbe result                // result = (st(0) <= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}







inline bool operator>=(const Double80 &x, const INT64     &y) {
  bool result;
  __asm {
    mov eax, y                  // load y
    fild QWORD PTR [eax]
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setae result                // result = (st(0) >= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}







inline bool operator< (const Double80 &x, const INT64     &y) {
  bool result;
  __asm {
    mov eax, y                  // load y
    fild QWORD PTR [eax]
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setb  result                // result = (st(0) <  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}







inline bool operator> (const Double80 &x, const INT64     &y) {
  bool result;
  __asm {
    mov eax, y                  // load y
    fild QWORD PTR [eax]
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    seta  result                // result = (st(0) >  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}







inline bool operator==(const Double80 &x, UINT64           y) {
  bool result;
  FILDUINT64(y)                 // load y
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    sete  result                // result = (st(0) == st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}








inline bool operator!=(const Double80 &x, UINT64           y) {
  bool result;
  FILDUINT64(y)                 // load y
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setne result                // result = (st(0) != st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}








inline bool operator<=(const Double80 &x, UINT64           y) {
  bool result;
  FILDUINT64(y)                 // load y
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setbe result                // result = (st(0) <= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}








inline bool operator>=(const Double80 &x, UINT64           y) {
  bool result;
  FILDUINT64(y)                 // load y
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setae result                // result = (st(0) >= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}








inline bool operator< (const Double80 &x, UINT64           y) {
  bool result;
  FILDUINT64(y)                 // load y
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setb  result                // result = (st(0) <  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}








inline bool operator> (const Double80 &x, UINT64           y) {
  bool result;
  FILDUINT64(y)                 // load y
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    seta  result                // result = (st(0) >  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}








inline bool operator==(const Double80 &x, float            y) {
  bool result;
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]
    fcomp y                     // compare and pop x
    fnstsw ax
    sahf
    sete  result                // result = (st(0) == y) ? 1 : 0
  }
  return result;
}








inline bool operator!=(const Double80 &x, float            y) {
  bool result;
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]
    fcomp y                     // compare and pop x
    fnstsw ax
    sahf
    setne result                // result = (st(0) != y) ? 1 : 0
  }
  return result;
}








inline bool operator<=(const Double80 &x, float            y) {
  bool result;
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]
    fcomp y                     // compare and pop x
    fnstsw ax
    sahf
    setbe result                // result = (st(0) <= y) ? 1 : 0
  }
  return result;
}








inline bool operator>=(const Double80 &x, float            y) {
  bool result;
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]
    fcomp y                     // compare and pop x
    fnstsw ax
    sahf
    setae result                // result = (st(0) >= y) ? 1 : 0
  }
  return result;
}








inline bool operator< (const Double80 &x, float            y) {
  bool result;
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]
    fcomp y                     // compare and pop x
    fnstsw ax
    sahf
    setb  result                // result = (st(0) <  y) ? 1 : 0
  }
  return result;
}








inline bool operator> (const Double80 &x, float            y) {
  bool result;
  __asm {
    mov eax, x                  // load x
    fld TBYTE PTR [eax]
    fcomp y                     // compare and pop x
    fnstsw ax
    sahf
    seta  result                // result = (st(0) >  y) ? 1 : 0
  }
  return result;
}








inline bool operator==(const Double80 &x, const double    &y) {
  bool result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov eax, y
    fcomp QWORD PTR[eax]        // compare and pop x
    fnstsw ax
    sahf
    sete  result                // result = (st(0) == y) ? 1 : 0
  }
  return result;
}







inline bool operator!=(const Double80 &x, const double    &y) {
  bool result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov eax, y
    fcomp QWORD PTR[eax]        // compare and pop x
    fnstsw ax
    sahf
    setne result                // result = (st(0) != y) ? 1 : 0
  }
  return result;
}







inline bool operator<=(const Double80 &x, const double    &y) {
  bool result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov eax, y
    fcomp QWORD PTR[eax]        // compare and pop x
    fnstsw ax
    sahf
    setbe result                // result = (st(0) <= y) ? 1 : 0
  }
  return result;
}







inline bool operator>=(const Double80 &x, const double    &y) {
  bool result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov eax, y
    fcomp QWORD PTR[eax]        // compare and pop x
    fnstsw ax
    sahf
    setae result                // result = (st(0) >= y) ? 1 : 0
  }
  return result;
}







inline bool operator< (const Double80 &x, const double    &y) {
  bool result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov eax, y
    fcomp QWORD PTR[eax]        // compare and pop x
    fnstsw ax
    sahf
    setb  result                // result = (st(0) <  y) ? 1 : 0
  }
  return result;
}







inline bool operator> (const Double80 &x, const double    &y) {
  bool result;
  __asm {
    mov eax, x
    fld TBYTE PTR [eax]
    mov eax, y
    fcomp QWORD PTR[eax]        // compare and pop x
    fnstsw ax
    sahf
    seta  result                // result = (st(0) >  y) ? 1 : 0
  }
  return result;
}







inline bool operator==(const Double80 &x, const Double80  &y) {
  bool result;
  __asm {
    mov eax, DWORD PTR y        // load y
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    sete  result                // result = (st(0) == st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}







inline bool operator!=(const Double80 &x, const Double80  &y) {
  bool result;
  __asm {
    mov eax, DWORD PTR y        // load y
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setne result                // result = (st(0) != st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}







inline bool operator>=(const Double80 &x, const Double80  &y) {
  bool result;
  __asm {
    mov eax, DWORD PTR y        // load y
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setae result                // result = (st(0) >= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}







inline bool operator<=(const Double80 &x, const Double80  &y) {
  bool result;
  __asm {
    mov eax, DWORD PTR y        // load y
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setbe result                // result = (st(0) <= st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}







inline bool operator> (const Double80 &x, const Double80  &y) {
  bool result;
  __asm {
    mov eax, DWORD PTR y        // load y
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    seta  result                // result = (st(0) >  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}







inline bool operator< (const Double80 &x, const Double80  &y) {
  bool result;
  __asm {
    mov eax, DWORD PTR y        // load y
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR x        // load x
    fld TBYTE PTR [eax]         // st(0)=x, st(1)=y
    fcomip st, st(1)            // compare and pop x
    setb  result                // result = (st(0) <  st(1)) ? 1 : 0
    fstp st(0)                  // pop y
  }
  return result;
}







inline Double80 fabs(const Double80 &x) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fabs
    fstp result
  }
  return result;
}

Double80 fmod(const Double80 &x, const Double80 &y);
inline Double80 sqr(const Double80 &x) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fmul st(0), st(0)
    fstp result
  }
  return result;
}
inline Double80 sqrt(const Double80 &x) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fsqrt
    fstp result
  }
  return result;
}
Double80 sin(   const Double80 &x);
Double80 cos(   const Double80 &x);
Double80 tan(   const Double80 &x);
inline Double80 atan(const Double80 &x) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fld1
    fpatan
    fstp result
  }
  return result;
}
inline Double80 atan2(const Double80 &y, const Double80 &x) {
  Double80 result;
  __asm {
    mov eax, DWORD PTR y
    fld TBYTE PTR [eax]
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fpatan
    fstp result
  }
  return result;
}

// calculate both cos and sin. c:inout c, s:out
void     sincos(Double80 &c, Double80 &s);
Double80 exp(  const Double80 &x);
Double80 exp10(const Double80 &x);
Double80 exp2( const Double80 &x);
inline Double80 log(const Double80 &x) {
  Double80 result;
  __asm {
    fldln2
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fyl2x
    fstp result
  }
  return result;
}
inline Double80 log10(const Double80 &x) {
  Double80 result;
  __asm {
    fldlg2
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fyl2x
    fstp result
  }
  return result;
}
inline Double80 log2(const Double80 &x) {
  Double80 result;
  __asm {
    fld1
    mov eax, DWORD PTR x
    fld TBYTE PTR [eax]
    fyl2x
    fstp result
  }
  return result;
}

Double80 pow(  const Double80 &x, const Double80 &y);
Double80 floor(const Double80 &x);
Double80 ceil( const Double80 &x);

#else // IS64BIT
inline int    getInt(    const Double80 &x) { return D80ToI32(  x); }
inline UINT   getUint(   const Double80 &x) { return D80ToUI32( x); }
inline INT64  getInt64(  const Double80 &x) { return D80ToI64(  x); }
inline UINT64 getUint64( const Double80 &x) { return D80ToUI64( x); }

inline Double80 operator+(const Double80 &x, const short    &y) { Double80 t(x); D80addI16(    t, y); return t;}
inline Double80 operator-(const Double80 &x, const short    &y) { Double80 t(x); D80subI16(    t, y); return t;}
inline Double80 operator*(const Double80 &x, const short    &y) { Double80 t(x); D80mulI16(    t, y); return t;}
inline Double80 operator/(const Double80 &x, const short    &y) { Double80 t(x); D80divI16(    t, y); return t;}
inline Double80 operator+(const Double80 &x, USHORT          y) { Double80 t(x); D80addUI16(   t, y); return t;}
inline Double80 operator-(const Double80 &x, USHORT          y) { Double80 t(x); D80subUI16(   t, y); return t;}
inline Double80 operator*(const Double80 &x, USHORT          y) { Double80 t(x); D80mulUI16(   t, y); return t;}
inline Double80 operator/(const Double80 &x, USHORT          y) { Double80 t(x); D80divUI16(   t, y); return t;}
inline Double80 operator+(const Double80 &x, const int      &y) { Double80 t(x); D80addI32(    t, y); return t;}
inline Double80 operator-(const Double80 &x, const int      &y) { Double80 t(x); D80subI32(    t, y); return t;}
inline Double80 operator*(const Double80 &x, const int      &y) { Double80 t(x); D80mulI32(    t, y); return t;}
inline Double80 operator/(const Double80 &x, const int      &y) { Double80 t(x); D80divI32(    t, y); return t;}
inline Double80 operator+(const Double80 &x, UINT            y) { Double80 t(x); D80addUI32(   t, y); return t;}
inline Double80 operator-(const Double80 &x, UINT            y) { Double80 t(x); D80subUI32(   t, y); return t;}
inline Double80 operator*(const Double80 &x, UINT            y) { Double80 t(x); D80mulUI32(   t, y); return t;}
inline Double80 operator/(const Double80 &x, UINT            y) { Double80 t(x); D80divUI32(   t, y); return t;}
inline Double80 operator+(const Double80 &x, const long     &y) { Double80 t(x); D80addI32(    t, (int&)y); return t;}
inline Double80 operator-(const Double80 &x, const long     &y) { Double80 t(x); D80subI32(    t, (int&)y); return t;}
inline Double80 operator*(const Double80 &x, const long     &y) { Double80 t(x); D80mulI32(    t, (int&)y); return t;}
inline Double80 operator/(const Double80 &x, const long     &y) { Double80 t(x); D80divI32(    t, (int&)y); return t;}
inline Double80 operator+(const Double80 &x, ULONG           y) { Double80 t(x); D80addUI32(   t, y); return t;}
inline Double80 operator-(const Double80 &x, ULONG           y) { Double80 t(x); D80subUI32(   t, y); return t;}
inline Double80 operator*(const Double80 &x, ULONG           y) { Double80 t(x); D80mulUI32(   t, y); return t;}
inline Double80 operator/(const Double80 &x, ULONG           y) { Double80 t(x); D80divUI32(   t, y); return t;}
inline Double80 operator+(const Double80 &x, const INT64    &y) { Double80 t(x); D80addI64(    t, y); return t;}
inline Double80 operator-(const Double80 &x, const INT64    &y) { Double80 t(x); D80subI64(    t, y); return t;}
inline Double80 operator*(const Double80 &x, const INT64    &y) { Double80 t(x); D80mulI64(    t, y); return t;}
inline Double80 operator/(const Double80 &x, const INT64    &y) { Double80 t(x); D80divI64(    t, y); return t;}
inline Double80 operator+(const Double80 &x, UINT64          y) { Double80 t(x); D80addUI64(   t, y); return t;}
inline Double80 operator-(const Double80 &x, UINT64          y) { Double80 t(x); D80subUI64(   t, y); return t;}
inline Double80 operator*(const Double80 &x, UINT64          y) { Double80 t(x); D80mulUI64(   t, y); return t;}
inline Double80 operator/(const Double80 &x, UINT64          y) { Double80 t(x); D80divUI64(   t, y); return t;}
inline Double80 operator+(const Double80 &x, const float    &y) { Double80 t(x); D80addFlt(    t, y); return t;}
inline Double80 operator-(const Double80 &x, const float    &y) { Double80 t(x); D80subFlt(    t, y); return t;}
inline Double80 operator*(const Double80 &x, const float    &y) { Double80 t(x); D80mulFlt(    t, y); return t;}
inline Double80 operator/(const Double80 &x, const float    &y) { Double80 t(x); D80divFlt(    t, y); return t;}
inline Double80 operator+(const Double80 &x, const double   &y) { Double80 t(x); D80addDbl(    t, y); return t;}
inline Double80 operator-(const Double80 &x, const double   &y) { Double80 t(x); D80subDbl(    t, y); return t;}
inline Double80 operator*(const Double80 &x, const double   &y) { Double80 t(x); D80mulDbl(    t, y); return t;}
inline Double80 operator/(const Double80 &x, const double   &y) { Double80 t(x); D80divDbl(    t, y); return t;}
inline Double80 operator+(const Double80 &x, const Double80 &y) { Double80 t(x); D80addD80(    t, y); return t;}
inline Double80 operator-(const Double80 &x, const Double80 &y) { Double80 t(x); D80subD80(    t, y); return t;}
inline Double80 operator-(const Double80 &x)                    { Double80 t(x); D80neg(       t   ); return t;}
inline Double80 operator*(const Double80 &x, const Double80 &y) { Double80 t(x); D80mulD80(    t, y); return t;}
inline Double80 operator/(const Double80 &x, const Double80 &y) { Double80 t(x); D80divD80(    t, y); return t;}
inline Double80 operator+(const short    &x, const Double80 &y) { Double80 t(y); D80addI16(    t, x); return t;}
inline Double80 operator-(const short    &x, const Double80 &y) { Double80 t(y); D80subrI16(   t, x); return t;}
inline Double80 operator*(const short    &x, const Double80 &y) { Double80 t(y); D80mulI16(    t, x); return t;}
inline Double80 operator/(const short    &x, const Double80 &y) { Double80 t(y); D80divrI16(   t, x); return t;}
inline Double80 operator+(USHORT          x, const Double80 &y) { Double80 t(y); D80addUI16(   t, x); return t;}
inline Double80 operator-(USHORT          x, const Double80 &y) { Double80 t(y); D80subrUI16(  t, x); return t;}
inline Double80 operator*(USHORT          x, const Double80 &y) { Double80 t(y); D80mulUI16(   t, x); return t;}
inline Double80 operator/(USHORT          x, const Double80 &y) { Double80 t(y); D80divrUI16(  t, x); return t;}
inline Double80 operator+(const int      &x, const Double80 &y) { Double80 t(y); D80addI32(    t, x); return t;}
inline Double80 operator-(const int      &x, const Double80 &y) { Double80 t(y); D80subrI32(   t, x); return t;}
inline Double80 operator*(const int      &x, const Double80 &y) { Double80 t(y); D80mulI32(    t, x); return t;}
inline Double80 operator/(const int      &x, const Double80 &y) { Double80 t(y); D80divrI32(   t, x); return t;}
inline Double80 operator+(UINT            x, const Double80 &y) { Double80 t(y); D80addUI32(   t, x); return t;}
inline Double80 operator-(UINT            x, const Double80 &y) { Double80 t(y); D80subrUI32(  t, x); return t;}
inline Double80 operator*(UINT            x, const Double80 &y) { Double80 t(y); D80mulUI32(   t, x); return t;}
inline Double80 operator/(UINT            x, const Double80 &y) { Double80 t(y); D80divrUI32(  t, x); return t;}
inline Double80 operator+(const long     &x, const Double80 &y) { Double80 t(y); D80addI32(    t, (int&)x); return t;}
inline Double80 operator-(const long     &x, const Double80 &y) { Double80 t(y); D80subrI32(   t, (int&)x); return t;}
inline Double80 operator*(const long     &x, const Double80 &y) { Double80 t(y); D80mulI32(    t, (int&)x); return t;}
inline Double80 operator/(const long     &x, const Double80 &y) { Double80 t(y); D80divrI32(   t, (int&)x); return t;}
inline Double80 operator+(ULONG           x, const Double80 &y) { Double80 t(y); D80addUI32(   t, x); return t;}
inline Double80 operator-(ULONG           x, const Double80 &y) { Double80 t(y); D80subrUI32(  t, x); return t;}
inline Double80 operator*(ULONG           x, const Double80 &y) { Double80 t(y); D80mulUI32(   t, x); return t;}
inline Double80 operator/(ULONG           x, const Double80 &y) { Double80 t(y); D80divrUI32(  t, x); return t;}
inline Double80 operator+(const INT64    &x, const Double80 &y) { Double80 t(y); D80addI64(    t, x); return t;}
inline Double80 operator-(const INT64    &x, const Double80 &y) { Double80 t(y); D80subrI64(   t, x); return t;}
inline Double80 operator*(const INT64    &x, const Double80 &y) { Double80 t(y); D80mulI64(    t, x); return t;}
inline Double80 operator/(const INT64    &x, const Double80 &y) { Double80 t(y); D80divrI64(   t, x); return t;}
inline Double80 operator+(UINT64          x, const Double80 &y) { Double80 t(y); D80addUI64(   t, x); return t;}
inline Double80 operator-(UINT64          x, const Double80 &y) { Double80 t(y); D80subrUI64(  t, x); return t;}
inline Double80 operator*(UINT64          x, const Double80 &y) { Double80 t(y); D80mulUI64(   t, x); return t;}
inline Double80 operator/(UINT64          x, const Double80 &y) { Double80 t(y); D80divrUI64(  t, x); return t;}
inline Double80 operator+(const float    &x, const Double80 &y) { Double80 t(y); D80addFlt(    t, x); return t;}
inline Double80 operator-(const float    &x, const Double80 &y) { Double80 t(y); D80subrFlt(   t, x); return t;}
inline Double80 operator*(const float    &x, const Double80 &y) { Double80 t(y); D80mulFlt(    t, x); return t;}
inline Double80 operator/(const float    &x, const Double80 &y) { Double80 t(y); D80divrFlt(   t, x); return t;}
inline Double80 operator+(const double   &x, const Double80 &y) { Double80 t(y); D80addDbl(    t, x); return t;}
inline Double80 operator-(const double   &x, const Double80 &y) { Double80 t(y); D80subrDbl(   t, x); return t;}
inline Double80 operator*(const double   &x, const Double80 &y) { Double80 t(y); D80mulDbl(    t, x); return t;}
inline Double80 operator/(const double   &x, const Double80 &y) { Double80 t(y); D80divrDbl(   t, x); return t;}

inline Double80 &operator+=( Double80 &x, const short    &y) { D80addI16(   x, y); return x; }
inline Double80 &operator-=( Double80 &x, const short    &y) { D80subI16(   x, y); return x; }
inline Double80 &operator*=( Double80 &x, const short    &y) { D80mulI16(   x, y); return x; }
inline Double80 &operator/=( Double80 &x, const short    &y) { D80divI16(   x, y); return x; }
inline Double80 &operator+=( Double80 &x, USHORT          y) { D80addUI16(  x, y); return x; }
inline Double80 &operator-=( Double80 &x, USHORT          y) { D80subUI16(  x, y); return x; }
inline Double80 &operator*=( Double80 &x, USHORT          y) { D80mulUI16(  x, y); return x; }
inline Double80 &operator/=( Double80 &x, USHORT          y) { D80divUI16(  x, y); return x; }
inline Double80 &operator+=( Double80 &x, const int      &y) { D80addI32(   x, y); return x; }
inline Double80 &operator-=( Double80 &x, const int      &y) { D80subI32(   x, y); return x; }
inline Double80 &operator*=( Double80 &x, const int      &y) { D80mulI32(   x, y); return x; }
inline Double80 &operator/=( Double80 &x, const int      &y) { D80divI32(   x, y); return x; }
inline Double80 &operator+=( Double80 &x, UINT            y) { D80addUI32(  x, y); return x; }
inline Double80 &operator-=( Double80 &x, UINT            y) { D80subUI32(  x, y); return x; }
inline Double80 &operator*=( Double80 &x, UINT            y) { D80mulUI32(  x, y); return x; }
inline Double80 &operator/=( Double80 &x, UINT            y) { D80divUI32(  x, y); return x; }
inline Double80 &operator+=( Double80 &x, const long     &y) { D80addI32(   x, (int&)y); return x; }
inline Double80 &operator-=( Double80 &x, const long     &y) { D80subI32(   x, (int&)y); return x; }
inline Double80 &operator*=( Double80 &x, const long     &y) { D80mulI32(   x, (int&)y); return x; }
inline Double80 &operator/=( Double80 &x, const long     &y) { D80divI32(   x, (int&)y); return x; }
inline Double80 &operator+=( Double80 &x, ULONG           y) { D80addUI32(  x, y); return x; }
inline Double80 &operator-=( Double80 &x, ULONG           y) { D80subUI32(  x, y); return x; }
inline Double80 &operator*=( Double80 &x, ULONG           y) { D80mulUI32(  x, y); return x; }
inline Double80 &operator/=( Double80 &x, ULONG           y) { D80divUI32(  x, y); return x; }
inline Double80 &operator+=( Double80 &x, const INT64    &y) { D80addI64(   x, y); return x; }
inline Double80 &operator-=( Double80 &x, const INT64    &y) { D80subI64(   x, y); return x; }
inline Double80 &operator*=( Double80 &x, const INT64    &y) { D80mulI64(   x, y); return x; }
inline Double80 &operator/=( Double80 &x, const INT64    &y) { D80divI64(   x, y); return x; }
inline Double80 &operator+=( Double80 &x, UINT64          y) { D80addUI64(  x, y); return x; }
inline Double80 &operator-=( Double80 &x, UINT64          y) { D80subUI64(  x, y); return x; }
inline Double80 &operator*=( Double80 &x, UINT64          y) { D80mulUI64(  x, y); return x; }
inline Double80 &operator/=( Double80 &x, UINT64          y) { D80divUI64(  x, y); return x; }
inline Double80 &operator+=( Double80 &x, const float    &y) { D80addFlt(   x, y); return x; }
inline Double80 &operator-=( Double80 &x, const float    &y) { D80subFlt(   x, y); return x; }
inline Double80 &operator*=( Double80 &x, const float    &y) { D80mulFlt(   x, y); return x; }
inline Double80 &operator/=( Double80 &x, const float    &y) { D80divFlt(   x, y); return x; }
inline Double80 &operator+=( Double80 &x, const double   &y) { D80addDbl(   x, y); return x; }
inline Double80 &operator-=( Double80 &x, const double   &y) { D80subDbl(   x, y); return x; }
inline Double80 &operator*=( Double80 &x, const double   &y) { D80mulDbl(   x, y); return x; }
inline Double80 &operator/=( Double80 &x, const double   &y) { D80divDbl(   x, y); return x; }
inline Double80 &operator+=( Double80 &x, const Double80 &y) { D80addD80(   x, y); return x; }
inline Double80 &operator-=( Double80 &x, const Double80 &y) { D80subD80(   x, y); return x; }
inline Double80 &operator*=( Double80 &x, const Double80 &y) { D80mulD80(   x, y); return x; }
inline Double80 &operator/=( Double80 &x, const Double80 &y) { D80divD80(   x, y); return x; }

inline bool operator==(const Double80 &x, const short    &y) { return D80cmpI16(   x, y) == 0; }
inline bool operator!=(const Double80 &x, const short    &y) { return D80cmpI16(   x, y) != 0; }
inline bool operator<=(const Double80 &x, const short    &y) { return D80cmpI16(   x, y) <= 0; }
inline bool operator>=(const Double80 &x, const short    &y) { return D80cmpI16(   x, y) >= 0; }
inline bool operator< (const Double80 &x, const short    &y) { return D80cmpI16(   x, y) <  0; }
inline bool operator> (const Double80 &x, const short    &y) { return D80cmpI16(   x, y) >  0; }
inline bool operator==(const Double80 &x, USHORT          y) { return D80cmpUI16(  x, y) == 0; }
inline bool operator!=(const Double80 &x, USHORT          y) { return D80cmpUI16(  x, y) != 0; }
inline bool operator<=(const Double80 &x, USHORT          y) { return D80cmpUI16(  x, y) <= 0; }
inline bool operator>=(const Double80 &x, USHORT          y) { return D80cmpUI16(  x, y) >= 0; }
inline bool operator< (const Double80 &x, USHORT          y) { return D80cmpUI16(  x, y) <  0; }
inline bool operator> (const Double80 &x, USHORT          y) { return D80cmpUI16(  x, y) >  0; }
inline bool operator==(const Double80 &x, const int      &y) { return D80cmpI32(   x, y) == 0; }
inline bool operator!=(const Double80 &x, const int      &y) { return D80cmpI32(   x, y) != 0; }
inline bool operator<=(const Double80 &x, const int      &y) { return D80cmpI32(   x, y) <= 0; }
inline bool operator>=(const Double80 &x, const int      &y) { return D80cmpI32(   x, y) >= 0; }
inline bool operator< (const Double80 &x, const int      &y) { return D80cmpI32(   x, y) <  0; }
inline bool operator> (const Double80 &x, const int      &y) { return D80cmpI32(   x, y) >  0; }
inline bool operator==(const Double80 &x, UINT            y) { return D80cmpUI32(  x, y) == 0; }
inline bool operator!=(const Double80 &x, UINT            y) { return D80cmpUI32(  x, y) != 0; }
inline bool operator<=(const Double80 &x, UINT            y) { return D80cmpUI32(  x, y) <= 0; }
inline bool operator>=(const Double80 &x, UINT            y) { return D80cmpUI32(  x, y) >= 0; }
inline bool operator< (const Double80 &x, UINT            y) { return D80cmpUI32(  x, y) <  0; }
inline bool operator> (const Double80 &x, UINT            y) { return D80cmpUI32(  x, y) >  0; }
inline bool operator==(const Double80 &x, const long     &y) { return D80cmpI32(   x, (int&)y) == 0; }
inline bool operator!=(const Double80 &x, const long     &y) { return D80cmpI32(   x, (int&)y) != 0; }
inline bool operator<=(const Double80 &x, const long     &y) { return D80cmpI32(   x, (int&)y) <= 0; }
inline bool operator>=(const Double80 &x, const long     &y) { return D80cmpI32(   x, (int&)y) >= 0; }
inline bool operator< (const Double80 &x, const long     &y) { return D80cmpI32(   x, (int&)y) <  0; }
inline bool operator> (const Double80 &x, const long     &y) { return D80cmpI32(   x, (int&)y) >  0; }
inline bool operator==(const Double80 &x, ULONG           y) { return D80cmpUI32(  x, y) == 0; }
inline bool operator!=(const Double80 &x, ULONG           y) { return D80cmpUI32(  x, y) != 0; }
inline bool operator<=(const Double80 &x, ULONG           y) { return D80cmpUI32(  x, y) <= 0; }
inline bool operator>=(const Double80 &x, ULONG           y) { return D80cmpUI32(  x, y) >= 0; }
inline bool operator< (const Double80 &x, ULONG           y) { return D80cmpUI32(  x, y) <  0; }
inline bool operator> (const Double80 &x, ULONG           y) { return D80cmpUI32(  x, y) >  0; }
inline bool operator==(const Double80 &x, const INT64    &y) { return D80cmpI64(   x, y) == 0; }
inline bool operator!=(const Double80 &x, const INT64    &y) { return D80cmpI64(   x, y) != 0; }
inline bool operator<=(const Double80 &x, const INT64    &y) { return D80cmpI64(   x, y) <= 0; }
inline bool operator>=(const Double80 &x, const INT64    &y) { return D80cmpI64(   x, y) >= 0; }
inline bool operator< (const Double80 &x, const INT64    &y) { return D80cmpI64(   x, y) <  0; }
inline bool operator> (const Double80 &x, const INT64    &y) { return D80cmpI64(   x, y) >  0; }
inline bool operator==(const Double80 &x, UINT64          y) { return D80cmpUI64(  x, y) == 0; }
inline bool operator!=(const Double80 &x, UINT64          y) { return D80cmpUI64(  x, y) != 0; }
inline bool operator<=(const Double80 &x, UINT64          y) { return D80cmpUI64(  x, y) <= 0; }
inline bool operator>=(const Double80 &x, UINT64          y) { return D80cmpUI64(  x, y) >= 0; }
inline bool operator< (const Double80 &x, UINT64          y) { return D80cmpUI64(  x, y) <  0; }
inline bool operator> (const Double80 &x, UINT64          y) { return D80cmpUI64(  x, y) >  0; }
inline bool operator==(const Double80 &x, const float    &y) { return D80cmpFlt(   x, y) == 0; }
inline bool operator!=(const Double80 &x, const float    &y) { return D80cmpFlt(   x, y) != 0; }
inline bool operator<=(const Double80 &x, const float    &y) { return D80cmpFlt(   x, y) <= 0; }
inline bool operator>=(const Double80 &x, const float    &y) { return D80cmpFlt(   x, y) >= 0; }
inline bool operator< (const Double80 &x, const float    &y) { return D80cmpFlt(   x, y) <  0; }
inline bool operator> (const Double80 &x, const float    &y) { return D80cmpFlt(   x, y) >  0; }
inline bool operator==(const Double80 &x, const double   &y) { return D80cmpDbl(   x, y) == 0; }
inline bool operator!=(const Double80 &x, const double   &y) { return D80cmpDbl(   x, y) != 0; }
inline bool operator<=(const Double80 &x, const double   &y) { return D80cmpDbl(   x, y) <= 0; }
inline bool operator>=(const Double80 &x, const double   &y) { return D80cmpDbl(   x, y) >= 0; }
inline bool operator< (const Double80 &x, const double   &y) { return D80cmpDbl(   x, y) <  0; }
inline bool operator> (const Double80 &x, const double   &y) { return D80cmpDbl(   x, y) >  0; }
inline bool operator==(const Double80 &x, const Double80 &y) { return D80cmpD80(   x, y) == 0; }
inline bool operator!=(const Double80 &x, const Double80 &y) { return D80cmpD80(   x, y) != 0; }
inline bool operator<=(const Double80 &x, const Double80 &y) { return D80cmpD80(   x, y) <= 0; }
inline bool operator>=(const Double80 &x, const Double80 &y) { return D80cmpD80(   x, y) >= 0; }
inline bool operator< (const Double80 &x, const Double80 &y) { return D80cmpD80(   x, y) <  0; }
inline bool operator> (const Double80 &x, const Double80 &y) { return D80cmpD80(   x, y) >  0; }

inline Double80 fabs(const Double80 &x) {
  Double80 tmp(x);
  D80fabs(tmp);
  return tmp;
}
inline Double80 fmod(const Double80 &x, const Double80 &y) {
  Double80 tmp(x);
  D80rem(tmp, y);
  return tmp;
}
inline Double80 sqr(const Double80 &x) {
  Double80 tmp(x);
  D80sqr(tmp);
  return tmp;
}
inline Double80 sqrt(const Double80 &x) {
  Double80 tmp(x);
  D80sqrt(tmp);
  return tmp;
}
inline Double80 sin(const Double80 &x) {
  Double80 tmp(x);
  D80sin(tmp);
  return tmp;
}
inline Double80 cos(const Double80 &x) {
  Double80 tmp(x);
  D80cos(tmp);
  return tmp;
}
inline Double80 tan(const Double80 &x) {
  Double80 tmp(x);
  D80tan(tmp);
  return tmp;
}
inline Double80 atan(const Double80 &x) {
  Double80 tmp(x);
  D80atan(tmp);
  return tmp;
}
inline Double80 atan2(const Double80 &y, const Double80 &x) {
  Double80 tmp(y);
  D80atan2(tmp, x);
  return tmp;
}
// calculate both cos and sin. c:inout c, s:out
inline void sincos(Double80 &c, Double80 &s) {
  D80sincos(c, s);
}
inline Double80 exp(const Double80 &x) {
  Double80 tmp(x);
  D80exp(tmp);
  return tmp;
}
inline Double80 exp10(const Double80 &x) {
  Double80 tmp(x);
  D80exp10(tmp);
  return tmp;
}
inline Double80 exp2(const Double80 &x) {
  Double80 tmp(x);
  D80exp2(tmp);
  return tmp;
}
inline Double80 log(const Double80 &x) {
  Double80 tmp(x);
  D80log(tmp);
  return tmp;
}
inline Double80 log10(const Double80 &x) {
  Double80 tmp(x);
  D80log10(tmp);
  return tmp;
}
inline Double80 log2(const Double80 &x) {
  Double80 tmp(x);
  D80log2(tmp);
  return tmp;
}
inline Double80 pow(const Double80 &x, const Double80 &y) {
  Double80 tmp(x);
  D80pow(tmp, y);
  return tmp;
}
inline Double80 floor(const Double80 &x) {
  Double80 tmp(x);
  D80floor(tmp);
  return tmp;
}
inline Double80 ceil(const Double80 &x) {
  Double80 tmp(x);
  D80ceil(tmp);
  return tmp;
}

#endif // IS64BIT

float  getFloat( const Double80 &x);
double getDouble(const Double80 &x);

inline bool operator==(short   x, const Double80 &y) {  return y == x; }
inline bool operator==(USHORT  x, const Double80 &y) {  return y == x; }
inline bool operator==(int     x, const Double80 &y) {  return y == x; }
inline bool operator==(UINT    x, const Double80 &y) {  return y == x; }
inline bool operator==(long    x, const Double80 &y) {  return y == x; }
inline bool operator==(ULONG   x, const Double80 &y) {  return y == x; }
inline bool operator==(INT64   x, const Double80 &y) {  return y == x; }
inline bool operator==(UINT64  x, const Double80 &y) {  return y == x; }
inline bool operator==(float   x, const Double80 &y) {  return y == x; }
inline bool operator==(double  x, const Double80 &y) {  return y == x; }
inline bool operator!=(short   x, const Double80 &y) {  return y != x; }
inline bool operator!=(USHORT  x, const Double80 &y) {  return y != x; }
inline bool operator!=(int     x, const Double80 &y) {  return y != x; }
inline bool operator!=(UINT    x, const Double80 &y) {  return y != x; }
inline bool operator!=(long    x, const Double80 &y) {  return y != x; }
inline bool operator!=(ULONG   x, const Double80 &y) {  return y != x; }
inline bool operator!=(INT64   x, const Double80 &y) {  return y != x; }
inline bool operator!=(UINT64  x, const Double80 &y) {  return y != x; }
inline bool operator!=(float   x, const Double80 &y) {  return y != x; }
inline bool operator!=(double  x, const Double80 &y) {  return y != x; }
inline bool operator<=(short   x, const Double80 &y) {  return y >= x; }
inline bool operator<=(USHORT  x, const Double80 &y) {  return y >= x; }
inline bool operator<=(int     x, const Double80 &y) {  return y >= x; }
inline bool operator<=(UINT    x, const Double80 &y) {  return y >= x; }
inline bool operator<=(long    x, const Double80 &y) {  return y >= x; }
inline bool operator<=(ULONG   x, const Double80 &y) {  return y >= x; }
inline bool operator<=(INT64   x, const Double80 &y) {  return y >= x; }
inline bool operator<=(UINT64  x, const Double80 &y) {  return y >= x; }
inline bool operator<=(float   x, const Double80 &y) {  return y >= x; }
inline bool operator<=(double  x, const Double80 &y) {  return y >= x; }
inline bool operator>=(short   x, const Double80 &y) {  return y <= x; }
inline bool operator>=(USHORT  x, const Double80 &y) {  return y <= x; }
inline bool operator>=(int     x, const Double80 &y) {  return y <= x; }
inline bool operator>=(UINT    x, const Double80 &y) {  return y <= x; }
inline bool operator>=(long    x, const Double80 &y) {  return y <= x; }
inline bool operator>=(ULONG   x, const Double80 &y) {  return y <= x; }
inline bool operator>=(INT64   x, const Double80 &y) {  return y <= x; }
inline bool operator>=(UINT64  x, const Double80 &y) {  return y <= x; }
inline bool operator>=(float   x, const Double80 &y) {  return y <= x; }
inline bool operator>=(double  x, const Double80 &y) {  return y <= x; }
inline bool operator< (short   x, const Double80 &y) {  return y >  x; }
inline bool operator< (USHORT  x, const Double80 &y) {  return y >  x; }
inline bool operator< (int     x, const Double80 &y) {  return y >  x; }
inline bool operator< (UINT    x, const Double80 &y) {  return y >  x; }
inline bool operator< (long    x, const Double80 &y) {  return y >  x; }
inline bool operator< (ULONG   x, const Double80 &y) {  return y >  x; }
inline bool operator< (INT64   x, const Double80 &y) {  return y >  x; }
inline bool operator< (UINT64  x, const Double80 &y) {  return y >  x; }
inline bool operator< (float   x, const Double80 &y) {  return y >  x; }
inline bool operator< (double  x, const Double80 &y) {  return y >  x; }
inline bool operator> (short   x, const Double80 &y) {  return y <  x; }
inline bool operator> (USHORT  x, const Double80 &y) {  return y <  x; }
inline bool operator> (int     x, const Double80 &y) {  return y <  x; }
inline bool operator> (UINT    x, const Double80 &y) {  return y <  x; }
inline bool operator> (long    x, const Double80 &y) {  return y <  x; }
inline bool operator> (ULONG   x, const Double80 &y) {  return y <  x; }
inline bool operator> (INT64   x, const Double80 &y) {  return y <  x; }
inline bool operator> (UINT64  x, const Double80 &y) {  return y <  x; }
inline bool operator> (float   x, const Double80 &y) {  return y <  x; }
inline bool operator> (double  x, const Double80 &y) {  return y <  x; }

inline char   getChar(  const Double80 &x) {  return (char  )getInt(x); }
inline UCHAR  getUchar( const Double80 &x) {  return (UCHAR )getInt(x); }
inline short  getShort( const Double80 &x) {  return (short )getInt(x); }
inline USHORT getUshort(const Double80 &x) {  return (USHORT)getInt(x); }
inline long   getLong(  const Double80 &x) {  return getInt(x);         }
inline ULONG  getUlong( const Double80 &x) {  return getUint(x);        }

inline Double80 cot(const Double80 &x) {
  return 1.0/tan(x);
}
Double80 asin( const Double80 &x);
inline Double80 acos(const Double80 &x) {
  return DBL80_PI_05 - asin(x);
}
inline Double80 acot(const Double80 &x) {
  return DBL80_PI_05 - atan(x);
}
Double80 mypow(const Double80 &x, const Double80 &y);
Double80 root( const Double80 &x, const Double80 &y);
inline Double80 cosh(const Double80 &x) {
  const Double80 e1 = exp(x);
  return (e1 + 1.0/e1)/2;
}
inline Double80 sinh(const Double80 &x) {
  const Double80 e1 = exp(x);
  return (e1 - 1.0/e1)/2;
}
inline Double80 tanh(const Double80 &x) {
  const Double80 e1 = exp(x), e2 = 1.0/e1;
  return (e1 - e2)/(e1+e2);
}
inline Double80 acosh(const Double80 &x) {
  return log(x + sqrt(x*x-1.0));
}
inline Double80 asinh(const Double80 &x) {
  return log(x + sqrt(x*x+1.0));
}
inline Double80 atanh(const Double80 &x) {
  return log(sqrt((1.0+x)/(1.0-x)));
}
inline Double80 hypot(const Double80 &x, const Double80 &y) {
  return sqrt(x*x+y*y);
}
Double80 fraction(   const Double80 &x);
Double80 round(      const Double80 &x, int prec = 0);

inline int sign(const Double80 &x) {
  return x.isZero() ? 0 : x.isNegative() ? -1 : 1;
}


inline int getExpo2(const Double80 &v) {
  return getExponent(v) - 0x3fff;
}

int fpclassify(const Double80 &v);

inline bool isPInfinity(const Double80 &v) {
  return isinf(v) && v.isPositive();
}
inline bool isNInfinity(const Double80 &v) {
  return isinf(v) && v.isNegative();
}

inline bool   isChar(   const Double80 &v) {  return isfinite( v) && (v == getChar(  v)); }
inline bool   isUchar(  const Double80 &v) {  return isfinite( v) && (v == getUchar( v)); }
inline bool   isShort(  const Double80 &v) {  return isfinite( v) && (v == getShort( v)); }
inline bool   isUshort( const Double80 &v) {  return isfinite( v) && (v == getUshort(v)); }
inline bool   isInt(    const Double80 &v) {  return isfinite( v) && (v == getInt(   v)); }
inline bool   isUint(   const Double80 &v) {  return isfinite( v) && (v == getUint(  v)); }
inline bool   isInt64(  const Double80 &v) {  return isfinite( v) && (v == getInt64( v)); }
inline bool   isUint64( const Double80 &v) {  return isfinite( v) && (v == getUint64(v)); }
inline bool   isFloat(  const Double80 &v) {  return !isnormal(v) || (v == getFloat( v)); }
inline bool   isDouble( const Double80 &v) {  return !isnormal(v) || (v == getDouble(v)); }

Double80 randDouble80(Random *rnd = NULL);
Double80 randDouble80(const Double80 &low, const Double80 &high, Random *rnd = NULL);

// dst must point to memory with at least 26 free char
char    *d80toa(char    *dst, const Double80 &x);
// dst must point to memory with at least 26 free wchar_t
wchar_t *d80tow(wchar_t *dst, const Double80 &x);

#ifdef _UNICODE
#define d80tot d80tow
#else
#define d80tot d80toa
#endif

String toString(const Double80 &x, StreamSize precision=6, StreamSize width=0, FormatFlags flags=0);

Double80 _strtod80_l(const char    *strSource, char    **endptr, _locale_t locale);
Double80 _wcstod80_l(const wchar_t *strSource, wchar_t **endptr, _locale_t locale);

inline Double80 strtod80(const char    *s, char    **end) {
  return _strtod80_l(s, end, _get_current_locale());
}
inline Double80 wcstod80(const wchar_t *s, wchar_t **end) {
  return _wcstod80_l(s, end, _get_current_locale());
}

#ifdef _UNICODE
#define _tcstod80_l _wcstod80_l
#define _tcstod80    wcstod80
#else
#define _tcstod80_l _strtood80_l
#define _tcstod80    strtod80
#endif

std::istream &operator>>(std::istream &s,       Double80 &x);
std::ostream &operator<<(std::ostream &s, const Double80 &x);

std::wistream &operator>>(std::wistream &s,       Double80 &x);
std::wostream &operator<<(std::wostream &s, const Double80 &x);

Packer &operator<<(Packer &p, const Double80 &x);
Packer &operator>>(Packer &p,       Double80 &x);

// CLASS numeric_limits<Double80>
template<> class std::numeric_limits<Double80>
    : public _Num_float_base
{	// limits for type Double80
public:
  _NODISCARD static Double80 (min)() noexcept
  {	// return minimum value
    return Double80::_DBL80_MIN;
  }

  _NODISCARD static Double80 (max)() noexcept
  {	// return maximum value
    return Double80::_DBL80_MAX;
  }

  _NODISCARD static Double80 lowest() noexcept
  {	// return most negative value
    return (-(max)());
  }

  _NODISCARD static Double80 epsilon() noexcept
  {	// return smallest effective increment from 1.0
    return Double80::_DBL80_EPSILON;
  }

  _NODISCARD static Double80 round_error() noexcept
  {	// return largest rounding error
    return Double80::_05;
  }

  _NODISCARD static Double80 denorm_min() noexcept
  {	// return minimum denormalized value
    return Double80::_DBL80_TRUE_MIN;
  }

  _NODISCARD static Double80 infinity() noexcept
  {	// return positive infinity
    return Double80::_DBL80_PINF;
  }

  _NODISCARD static Double80 quiet_NaN() noexcept
  {	// return non-signaling NaN
    return Double80::_DBL80_QNAN;
  }

  _NODISCARD static Double80 signaling_NaN() noexcept
  {	// return signaling NaN
    return Double80::_DBL80_SNAN;
  }

  static constexpr int digits         =     64;                             // # of bits in mantissa
  static constexpr int digits10       =     19;                             // # of decimal digits of precision
  static constexpr int max_digits10   =     20;                             // # of decimal digits of rounding precision
  static constexpr int max_exponent   =  16384;                             // max binary exponent
  static constexpr int max_exponent10 =   4932;                             // max decimal exponent
  static constexpr int min_exponent   = -16382;                             // min binary exponent
  static constexpr int min_exponent10 =  -4932;                             // min decimal exponent
};
#define DBL80_HAS_SUBNORM      1                                            // type does support subnormal numbers
#define _DBL80_RADIX           2                                            // exponent radix


#define DBL80_NAN          Double80::_DBL80_QNAN
#define DBL80_PINF         Double80::_DBL80_PINF
#define DBL80_NINF        (-DBL80_PINF)

#define DBL80_DECIMAL_DIG  std::numeric_limits<Double80>::max_digits10   // # of decimal digits of rounding precision
#define DBL80_DIG          std::numeric_limits<Double80>::digits10       // # of decimal digits of precision
#define DBL80_EPSILON      Double80::_DBL80_EPSILON                      // smallest such that 1.0+DBL_EPSILON != 1.0
#define DBL80_MANT_DIG     std::numeric_limits<Double80>::digits         // # of bits in mantissa
#define DBL80_MAX          Double80::_DBL80_MAX                          // max value
#define DBL80_MAX_10_EXP   std::numeric_limits<Double80>::max_exponent10 // max decimal exponent
#define DBL80_MAX_EXP      std::numeric_limits<Double80>::max_exponent   // max binary exponent
#define DBL80_MIN          Double80::_DBL80_MIN                          // min positive value
#define DBL80_MIN_10_EXP   std::numeric_limits<Double80>::min_exponent10 // min decimal exponent
#define DBL80_MIN_EXP      std::numeric_limits<Double80>::min_exponent   // min binary exponent
#define DBL80_TRUE_MIN     Double80::_DBL80_TRUE_MIN                     // min positive value
