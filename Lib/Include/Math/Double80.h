#pragma once

#include <BasicIncludes.h>
#include <limits>
#include <Random.h>
#include "FPU.h"
#include "PragmaLib.h"

#pragma check_stack(off)

class Double80;

#if defined(IS32BIT)
#include "Double80Asmx86.h"
#else // IS32BIT
#include "Double80Asmx64.h"
#endif // IS32BIT

class Pow10Calculator {
public:
  virtual Double80 pow10(int p) const = 0;
#if defined(_DEBUG)
  void dumpAll() const;
#endif // _DEBUG
};

class Double80 {
private:
  BYTE m_value[10]; // Must be the first field in the class

  static Pow10Calculator *s_p10Calculator;

public:
  inline Double80() {
  }

  explicit inline Double80(const BYTE *bytes) {
    memcpy(&m_value, bytes, sizeof(m_value));
  }

  inline Double80(short  x) { _D80FromI16( *this, x); }
  inline Double80(USHORT x) { _D80FromUI16(*this, x); }
  inline Double80(int    x) { _D80FromI32( *this, x); }
  inline Double80(UINT   x) { _D80FromUI32(*this, x); }
  inline Double80(INT64  x) { _D80FromI64( *this, x); }
  inline Double80(UINT64 x) { _D80FromUI64(*this, x); }
  inline Double80(long   x) { _D80FromI32( *this, x); }
  inline Double80(ULONG  x) { _D80FromUI32(*this, x); }
  Double80(float  x);
  Double80(double x);


  explicit inline operator int() const {
    const FPUControlWord cw     = FPU::disableExceptions();
    const int            result = _D80ToI32(*this);
    FPU::restoreControlWord(cw);
    return result;
  }
  explicit inline operator unsigned int() const {
    const FPUControlWord cw     = FPU::disableExceptions();
    const unsigned int   result = _D80ToUI32(*this);
    FPU::restoreControlWord(cw);
    return result;
  }
  explicit inline operator char() const {
    return (char)(int)(*this);
  }
  explicit inline operator unsigned char() const {
    return (unsigned char)(int)(*this);
  }
  explicit inline operator short() const {
    return (short)(int)(*this);
  }
  explicit inline operator unsigned short() const {
    return (unsigned short)(int)(*this); ;
  }
  explicit inline operator long() const {
    return (int)(*this);
  }
  explicit inline operator unsigned long() const {
    return (unsigned int)(*this);
  }
  explicit inline operator __int64() const {
    const FPUControlWord cw     = FPU::disableExceptions();
    const __int64        result = _D80ToI64(*this);
    FPU::restoreControlWord(cw);
    return result;
  }
  explicit inline operator unsigned __int64() const {
    const FPUControlWord   cw     = FPU::disableExceptions();
    const unsigned __int64 result = _D80ToUI64(*this);
    FPU::restoreControlWord(cw);
    return result;
  }
  explicit operator float()  const;
  explicit operator double() const;
  // x == 0 ? 0 : floor(log10(|x|))
  static inline int getExpo10(const Double80 &x) {
    return _D80getExpo10(x);
  }

  inline bool isZero() const      {
    return _D80isZero(*this) ? true : false;
  }
  static inline Double80 pow2(int p) {
    Double80 result;
    _D80pow2(result,p);
    return result;
  }

  // prefix-form
  inline Double80 &operator++()   {
    _D80inc(*this);
    return *this;
  }
  inline Double80 &operator--()   {
    _D80dec(*this);
    return *this;
  }
  // postfix-form
  inline Double80 operator++(int) {
    Double80 result(*this);
    _D80inc(*this);
    return result;
  }
  inline Double80 operator--(int) {
    Double80 result(*this);
    _D80dec(*this);
    return result;
  }

  friend inline UINT64 getSignificand( const Double80 &x) {
    return *((UINT64*)x.m_value);
  }
  friend inline UINT   getExponent(    const Double80 &x) {
    return (*(UINT*)(x.m_value+8)) & 0x7fff;
  }
  friend inline BYTE   getSign(        const Double80 &x) {
    return x.m_value[9] & 0x80;
  }

  inline bool isPositive() const {
    return (m_value[9] & 0x80) == 0;
  }
  inline bool isNegative() const {
    return (m_value[9] & 0x80) != 0;
  }

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
  static const Double80 &_DBL80_EPSILON;                  // Smallest such that 1.0+DBL80_EPSILON != 1.0 (=1.08420217248550443e-019)
  static const Double80 &_DBL80_MIN;                      // Min positive value (=3.36210314311209209e-4932)
  static const Double80 &_DBL80_MAX;                      // Max value          (=1.18973149535723227e+4932)
  static const Double80 &_DBL80_QNAN;                     // non-signaling NaN (quiet NaN)
  static const Double80 &_DBL80_SNAN;                     // signaling NaN
  static const Double80 &_DBL80_PINF;                     // +infinity;
  static const Double80 &_DBL80_NINF;                     // -infinity;
  static const Double80 &_DBL80_TRUE_MIN;                 // min positive value (3.64519953188247460325e-4951)

  static void initClass();
};

extern const Double80 DBL80_PI;                        // = 3.1415926535897932384626433
extern const Double80 DBL80_PI_05;                     // pi/2

inline Double80 operator+(const Double80 &x, const short    &y) { Double80 t(x); _D80addI16(    t, y); return t;}
inline Double80 operator-(const Double80 &x, const short    &y) { Double80 t(x); _D80subI16(    t, y); return t;}
inline Double80 operator*(const Double80 &x, const short    &y) { Double80 t(x); _D80mulI16(    t, y); return t;}
inline Double80 operator/(const Double80 &x, const short    &y) { Double80 t(x); _D80divI16(    t, y); return t;}
inline Double80 operator+(const Double80 &x, USHORT          y) { Double80 t(x); _D80addUI16(   t, y); return t;}
inline Double80 operator-(const Double80 &x, USHORT          y) { Double80 t(x); _D80subUI16(   t, y); return t;}
inline Double80 operator*(const Double80 &x, USHORT          y) { Double80 t(x); _D80mulUI16(   t, y); return t;}
inline Double80 operator/(const Double80 &x, USHORT          y) { Double80 t(x); _D80divUI16(   t, y); return t;}
inline Double80 operator+(const Double80 &x, const int      &y) { Double80 t(x); _D80addI32(    t, y); return t;}
inline Double80 operator-(const Double80 &x, const int      &y) { Double80 t(x); _D80subI32(    t, y); return t;}
inline Double80 operator*(const Double80 &x, const int      &y) { Double80 t(x); _D80mulI32(    t, y); return t;}
inline Double80 operator/(const Double80 &x, const int      &y) { Double80 t(x); _D80divI32(    t, y); return t;}
inline Double80 operator+(const Double80 &x, UINT            y) { Double80 t(x); _D80addUI32(   t, y); return t;}
inline Double80 operator-(const Double80 &x, UINT            y) { Double80 t(x); _D80subUI32(   t, y); return t;}
inline Double80 operator*(const Double80 &x, UINT            y) { Double80 t(x); _D80mulUI32(   t, y); return t;}
inline Double80 operator/(const Double80 &x, UINT            y) { Double80 t(x); _D80divUI32(   t, y); return t;}
inline Double80 operator+(const Double80 &x, const INT64    &y) { Double80 t(x); _D80addI64(    t, y); return t;}
inline Double80 operator-(const Double80 &x, const INT64    &y) { Double80 t(x); _D80subI64(    t, y); return t;}
inline Double80 operator*(const Double80 &x, const INT64    &y) { Double80 t(x); _D80mulI64(    t, y); return t;}
inline Double80 operator/(const Double80 &x, const INT64    &y) { Double80 t(x); _D80divI64(    t, y); return t;}
inline Double80 operator+(const Double80 &x, UINT64          y) { Double80 t(x); _D80addUI64(   t, y); return t;}
inline Double80 operator-(const Double80 &x, UINT64          y) { Double80 t(x); _D80subUI64(   t, y); return t;}
inline Double80 operator*(const Double80 &x, UINT64          y) { Double80 t(x); _D80mulUI64(   t, y); return t;}
inline Double80 operator/(const Double80 &x, UINT64          y) { Double80 t(x); _D80divUI64(   t, y); return t;}
inline Double80 operator+(const Double80 &x, const float    &y) { Double80 t(x); _D80addFlt(    t, y); return t;}
inline Double80 operator-(const Double80 &x, const float    &y) { Double80 t(x); _D80subFlt(    t, y); return t;}
inline Double80 operator*(const Double80 &x, const float    &y) { Double80 t(x); _D80mulFlt(    t, y); return t;}
inline Double80 operator/(const Double80 &x, const float    &y) { Double80 t(x); _D80divFlt(    t, y); return t;}
inline Double80 operator+(const Double80 &x, const double   &y) { Double80 t(x); _D80addDbl(    t, y); return t;}
inline Double80 operator-(const Double80 &x, const double   &y) { Double80 t(x); _D80subDbl(    t, y); return t;}
inline Double80 operator*(const Double80 &x, const double   &y) { Double80 t(x); _D80mulDbl(    t, y); return t;}
inline Double80 operator/(const Double80 &x, const double   &y) { Double80 t(x); _D80divDbl(    t, y); return t;}
inline Double80 operator+(const Double80 &x, const Double80 &y) { Double80 t(x); _D80addD80(    t, y); return t;}
inline Double80 operator-(const Double80 &x, const Double80 &y) { Double80 t(x); _D80subD80(    t, y); return t;}
inline Double80 operator-(const Double80 &x)                    { Double80 t(x); _D80neg(       t   ); return t;}
inline Double80 operator*(const Double80 &x, const Double80 &y) { Double80 t(x); _D80mulD80(    t, y); return t;}
inline Double80 operator/(const Double80 &x, const Double80 &y) { Double80 t(x); _D80divD80(    t, y); return t;}

inline Double80 &operator+=( Double80 &x, const short    &y) { _D80addI16(   x, y); return x; }
inline Double80 &operator-=( Double80 &x, const short    &y) { _D80subI16(   x, y); return x; }
inline Double80 &operator*=( Double80 &x, const short    &y) { _D80mulI16(   x, y); return x; }
inline Double80 &operator/=( Double80 &x, const short    &y) { _D80divI16(   x, y); return x; }
inline Double80 &operator+=( Double80 &x, USHORT          y) { _D80addUI16(  x, y); return x; }
inline Double80 &operator-=( Double80 &x, USHORT          y) { _D80subUI16(  x, y); return x; }
inline Double80 &operator*=( Double80 &x, USHORT          y) { _D80mulUI16(  x, y); return x; }
inline Double80 &operator/=( Double80 &x, USHORT          y) { _D80divUI16(  x, y); return x; }
inline Double80 &operator+=( Double80 &x, const int      &y) { _D80addI32(   x, y); return x; }
inline Double80 &operator-=( Double80 &x, const int      &y) { _D80subI32(   x, y); return x; }
inline Double80 &operator*=( Double80 &x, const int      &y) { _D80mulI32(   x, y); return x; }
inline Double80 &operator/=( Double80 &x, const int      &y) { _D80divI32(   x, y); return x; }
inline Double80 &operator+=( Double80 &x, UINT            y) { _D80addUI32(  x, y); return x; }
inline Double80 &operator-=( Double80 &x, UINT            y) { _D80subUI32(  x, y); return x; }
inline Double80 &operator*=( Double80 &x, UINT            y) { _D80mulUI32(  x, y); return x; }
inline Double80 &operator/=( Double80 &x, UINT            y) { _D80divUI32(  x, y); return x; }
inline Double80 &operator+=( Double80 &x, const INT64    &y) { _D80addI64(   x, y); return x; }
inline Double80 &operator-=( Double80 &x, const INT64    &y) { _D80subI64(   x, y); return x; }
inline Double80 &operator*=( Double80 &x, const INT64    &y) { _D80mulI64(   x, y); return x; }
inline Double80 &operator/=( Double80 &x, const INT64    &y) { _D80divI64(   x, y); return x; }
inline Double80 &operator+=( Double80 &x, UINT64          y) { _D80addUI64(  x, y); return x; }
inline Double80 &operator-=( Double80 &x, UINT64          y) { _D80subUI64(  x, y); return x; }
inline Double80 &operator*=( Double80 &x, UINT64          y) { _D80mulUI64(  x, y); return x; }
inline Double80 &operator/=( Double80 &x, UINT64          y) { _D80divUI64(  x, y); return x; }
inline Double80 &operator+=( Double80 &x, const float    &y) { _D80addFlt(   x, y); return x; }
inline Double80 &operator-=( Double80 &x, const float    &y) { _D80subFlt(   x, y); return x; }
inline Double80 &operator*=( Double80 &x, const float    &y) { _D80mulFlt(   x, y); return x; }
inline Double80 &operator/=( Double80 &x, const float    &y) { _D80divFlt(   x, y); return x; }
inline Double80 &operator+=( Double80 &x, const double   &y) { _D80addDbl(   x, y); return x; }
inline Double80 &operator-=( Double80 &x, const double   &y) { _D80subDbl(   x, y); return x; }
inline Double80 &operator*=( Double80 &x, const double   &y) { _D80mulDbl(   x, y); return x; }
inline Double80 &operator/=( Double80 &x, const double   &y) { _D80divDbl(   x, y); return x; }
inline Double80 &operator+=( Double80 &x, const Double80 &y) { _D80addD80(   x, y); return x; }
inline Double80 &operator-=( Double80 &x, const Double80 &y) { _D80subD80(   x, y); return x; }
inline Double80 &operator*=( Double80 &x, const Double80 &y) { _D80mulD80(   x, y); return x; }
inline Double80 &operator/=( Double80 &x, const Double80 &y) { _D80divD80(   x, y); return x; }

inline Double80 operator+(short           x, const Double80 &y) { return y + x; }
inline Double80 operator*(short           x, const Double80 &y) { return y * x; }
inline Double80 operator+(USHORT          x, const Double80 &y) { return y + x; }
inline Double80 operator*(USHORT          x, const Double80 &y) { return y * x; }
inline Double80 operator+(int             x, const Double80 &y) { return y + x; }
inline Double80 operator*(int             x, const Double80 &y) { return y * x; }
inline Double80 operator+(UINT            x, const Double80 &y) { return y + x; }
inline Double80 operator*(UINT            x, const Double80 &y) { return y * x; }
inline Double80 operator+(INT64           x, const Double80 &y) { return y + x; }
inline Double80 operator*(INT64           x, const Double80 &y) { return y * x; }
inline Double80 operator+(UINT64          x, const Double80 &y) { return y + x; }
inline Double80 operator*(UINT64          x, const Double80 &y) { return y * x; }
inline Double80 operator+(float           x, const Double80 &y) { return y + x; }
inline Double80 operator*(float           x, const Double80 &y) { return y * x; }
inline Double80 operator+(double          x, const Double80 &y) { return y + x; }
inline Double80 operator*(double          x, const Double80 &y) { return y * x; }
inline Double80 operator-(short           x, const Double80 &y) { Double80 t(y); _D80subrI16( t, x); return t; }
inline Double80 operator/(short           x, const Double80 &y) { Double80 t(y); _D80divrI16( t, x); return t; }
inline Double80 operator-(USHORT          x, const Double80 &y) { Double80 t(y); _D80subrUI16(t, x); return t; }
inline Double80 operator/(USHORT          x, const Double80 &y) { Double80 t(y); _D80divrUI16(t, x); return t; }
inline Double80 operator-(int             x, const Double80 &y) { Double80 t(y); _D80subrI32( t, x); return t; }
inline Double80 operator/(int             x, const Double80 &y) { Double80 t(y); _D80divrI32( t, x); return t; }
inline Double80 operator-(UINT            x, const Double80 &y) { Double80 t(y); _D80subrUI32(t, x); return t; }
inline Double80 operator/(UINT            x, const Double80 &y) { Double80 t(y); _D80divrUI32(t, x); return t; }
inline Double80 operator-(INT64           x, const Double80 &y) { Double80 t(y); _D80subrI64( t, x); return t; }
inline Double80 operator/(INT64           x, const Double80 &y) { Double80 t(y); _D80divrI64( t, x); return t; }
inline Double80 operator-(UINT64          x, const Double80 &y) { Double80 t(y); _D80subrUI64(t, x); return t; }
inline Double80 operator/(UINT64          x, const Double80 &y) { Double80 t(y); _D80divrUI64(t, x); return t; }
inline Double80 operator-(const float     x, const Double80 &y) { Double80 t(y); _D80subrFlt( t, x); return t; }
inline Double80 operator/(const float     x, const Double80 &y) { Double80 t(y); _D80divrFlt( t, x); return t; }
inline Double80 operator-(const double    x, const Double80 &y) { Double80 t(y); _D80subrDbl( t, x); return t; }
inline Double80 operator/(const double    x, const Double80 &y) { Double80 t(y); _D80divrDbl( t, x); return t; }

// operators for long/ulong same as int/UINT
inline Double80 operator+(const Double80 &x, long            y) { return x + (int )y; }
inline Double80 operator*(const Double80 &x, long            y) { return x * (int )y; }
inline Double80 operator+(const Double80 &x, ULONG           y) { return x + (UINT)y; }
inline Double80 operator*(const Double80 &x, ULONG           y) { return x * (UINT)y; }
inline Double80 operator-(const Double80 &x, long            y) { return x - (int )y; }
inline Double80 operator/(const Double80 &x, long            y) { return x / (int )y; }
inline Double80 operator-(const Double80 &x, ULONG           y) { return x - (UINT)y; }
inline Double80 operator/(const Double80 &x, ULONG           y) { return x / (UINT)y; }

inline Double80 operator+(long            x, const Double80 &y) { return y + (int )x; }
inline Double80 operator*(long            x, const Double80 &y) { return y * (int )x; }
inline Double80 operator+(ULONG           x, const Double80 &y) { return y * (UINT)x; }
inline Double80 operator*(ULONG           x, const Double80 &y) { return y * (UINT)x; }
inline Double80 operator-(long            x, const Double80 &y) { return (int )x - y; }
inline Double80 operator/(long            x, const Double80 &y) { return (int )x / y; }
inline Double80 operator-(ULONG           x, const Double80 &y) { return (UINT)x - y; }
inline Double80 operator/(ULONG           x, const Double80 &y) { return (UINT)x / y; }

inline Double80 &operator+=(Double80 &x    , long            y) { return x += (int )y; }
inline Double80 &operator-=(Double80 &x    , long            y) { return x -= (int )y; }
inline Double80 &operator*=(Double80 &x    , long            y) { return x *= (int )y; }
inline Double80 &operator/=(Double80 &x    , long            y) { return x /= (int )y; }
inline Double80 &operator+=(Double80 &x    , ULONG           y) { return x += (UINT)y; }
inline Double80 &operator-=(Double80 &x    , ULONG           y) { return x -= (UINT)y; }
inline Double80 &operator*=(Double80 &x    , ULONG           y) { return x *= (UINT)y; }
inline Double80 &operator/=(Double80 &x    , ULONG           y) { return x /= (UINT)y; }

#define _D80EQ(f) return f(x,y)==0
#define _D80NE(f) return f(x,y)!=0
#define _D80LE(f) return f(x,y)<=0
#define _D80GE(f) return (f(x, y)&3)<=1
#define _D80LT(f) return f(x,y)< 0
#define _D80GT(f) return f(x,y)==1

#define _D80DEFINE_COMPAREOPERATORS(type, cmp)                     \
inline bool operator==(const Double80 &x, type y) { _D80EQ(cmp); } \
inline bool operator!=(const Double80 &x, type y) { _D80NE(cmp); } \
inline bool operator<=(const Double80 &x, type y) { _D80LE(cmp); } \
inline bool operator>=(const Double80 &x, type y) { _D80GE(cmp); } \
inline bool operator< (const Double80 &x, type y) { _D80LT(cmp); } \
inline bool operator> (const Double80 &x, type y) { _D80GT(cmp); }

_D80DEFINE_COMPAREOPERATORS(short   , _D80cmpI16 )
_D80DEFINE_COMPAREOPERATORS(USHORT  , _D80cmpUI16)
_D80DEFINE_COMPAREOPERATORS(INT     , _D80cmpI32 )
_D80DEFINE_COMPAREOPERATORS(UINT    , _D80cmpUI32)
_D80DEFINE_COMPAREOPERATORS(LONG    , _D80cmpI32 )
_D80DEFINE_COMPAREOPERATORS(ULONG   , _D80cmpUI32)
_D80DEFINE_COMPAREOPERATORS(INT64   , _D80cmpI64 )
_D80DEFINE_COMPAREOPERATORS(UINT64  , _D80cmpUI64)
_D80DEFINE_COMPAREOPERATORS(float   , _D80cmpFlt )
_D80DEFINE_COMPAREOPERATORS(double  , _D80cmpDbl )
_D80DEFINE_COMPAREOPERATORS(Double80, _D80cmpD80 )

#define _D80DEFINE_REVERSECOMPAREOPERATORS(type)                     \
inline bool operator==(type x, const Double80 &y) { return y == x; } \
inline bool operator!=(type x, const Double80 &y) { return y != x; } \
inline bool operator<=(type x, const Double80 &y) { return y >= x; } \
inline bool operator>=(type x, const Double80 &y) { return y <= x; } \
inline bool operator< (type x, const Double80 &y) { return y >  x; } \
inline bool operator> (type x, const Double80 &y) { return y <  x; }

_D80DEFINE_REVERSECOMPAREOPERATORS(SHORT )
_D80DEFINE_REVERSECOMPAREOPERATORS(USHORT)
_D80DEFINE_REVERSECOMPAREOPERATORS(INT   )
_D80DEFINE_REVERSECOMPAREOPERATORS(UINT  )
_D80DEFINE_REVERSECOMPAREOPERATORS(LONG  )
_D80DEFINE_REVERSECOMPAREOPERATORS(ULONG )
_D80DEFINE_REVERSECOMPAREOPERATORS(INT64 )
_D80DEFINE_REVERSECOMPAREOPERATORS(UINT64)
_D80DEFINE_REVERSECOMPAREOPERATORS(float )
_D80DEFINE_REVERSECOMPAREOPERATORS(double)

inline Double80 fabs(const Double80 &x) {
  Double80 tmp(x);
  _D80fabs(tmp);
  return tmp;
}
inline Double80 fmod(const Double80 &x, const Double80 &y) {
  Double80 tmp(x);
  _D80rem(tmp, y);
  return tmp;
}
inline Double80 sqr(const Double80 &x) {
  Double80 tmp(x);
  _D80sqr(tmp);
  return tmp;
}
inline Double80 sqrt(const Double80 &x) {
  Double80 tmp(x);
  _D80sqrt(tmp);
  return tmp;
}
inline Double80 sin(const Double80 &x) {
  Double80 tmp(x);
  _D80sin(tmp);
  return tmp;
}
inline Double80 cos(const Double80 &x) {
  Double80 tmp(x);
  _D80cos(tmp);
  return tmp;
}
inline Double80 tan(const Double80 &x) {
  Double80 tmp(x);
  _D80tan(tmp);
  return tmp;
}
inline Double80 atan(const Double80 &x) {
  Double80 tmp(x);
  _D80atan(tmp);
  return tmp;
}
inline Double80 atan2(const Double80 &y, const Double80 &x) {
  Double80 tmp(y);
  _D80atan2(tmp, x);
  return tmp;
}
// calculate both cos and sin. c:inout c, s:out
inline void sincos(Double80 &c, Double80 &s) {
  _D80sincos(c, s);
}
inline Double80 exp(const Double80 &x) {
  Double80 tmp(x);
  _D80exp(tmp);
  return tmp;
}
inline Double80 exp10(const Double80 &x) {
  Double80 tmp(x);
  _D80exp10(tmp);
  return tmp;
}
inline Double80 exp2(const Double80 &x) {
  Double80 tmp(x);
  _D80exp2(tmp);
  return tmp;
}
inline Double80 log(const Double80 &x) {
  Double80 tmp(x);
  _D80log(tmp);
  return tmp;
}
inline Double80 log10(const Double80 &x) {
  Double80 tmp(x);
  _D80log10(tmp);
  return tmp;
}
inline Double80 log2(const Double80 &x) {
  Double80 tmp(x);
  _D80log2(tmp);
  return tmp;
}
Double80 pow(  const Double80 &x, const Double80 &y); // pow(0,0) = 1! as pow for double
Double80 mypow(const Double80 &x, const Double80 &y); // pow == mypow, except mypow(0,0) = nan
Double80 root( const Double80 &x, const Double80 &y);
inline Double80 floor(const Double80 &x) {
  Double80 tmp(x);
  _D80floor(tmp);
  return tmp;
}
inline Double80 ceil(const Double80 &x) {
  Double80 tmp(x);
  _D80ceil(tmp);
  return tmp;
}

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

Double80 gamma(  const Double80 &x);
Double80 lnGamma(const Double80 &x);

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

// returns one of
// FP_INFINITE
// FP_NAN
// FP_NORMAL
// FP_SUBNORMAL
// FP_ZERO
int fpclassify(const Double80 &v);

inline bool isPInfinity(const Double80 &v) {
  return isinf(v) && v.isPositive();
}
inline bool isNInfinity(const Double80 &v) {
  return isinf(v) && v.isNegative();
}

inline bool   isChar(   const Double80 &v) {  return isfinite( v) && (v == (char            )v); }
inline bool   isUchar(  const Double80 &v) {  return isfinite( v) && (v == (unsigned char   )v); }
inline bool   isShort(  const Double80 &v) {  return isfinite( v) && (v == (short           )v); }
inline bool   isUshort( const Double80 &v) {  return isfinite( v) && (v == (unsigned short  )v); }
inline bool   isInt(    const Double80 &v) {  return isfinite( v) && (v == (int             )v); }
inline bool   isUint(   const Double80 &v) {  return isfinite( v) && (v == (unsigned int    )v); }
inline bool   isInt64(  const Double80 &v) {  return isfinite( v) && (v == (__int64         )v); }
inline bool   isUint64( const Double80 &v) {  return isfinite( v) && (v == (unsigned __int64)v); }
inline bool   isFloat(  const Double80 &v) {  return !isnormal(v) || (v == (float           )v); }
inline bool   isDouble( const Double80 &v) {  return !isnormal(v) || (v == (double          )v); }

Double80 randDouble80(                                               RandomGenerator &rnd = *RandomGenerator::s_stdGenerator);
Double80 randDouble80(   const Double80 &low , const Double80 &high, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator);
Double80 randGaussianD80(const Double80 &mean, const Double80 &s   , RandomGenerator &rnd = *RandomGenerator::s_stdGenerator);

// dst must point to memory with at least 26 free char
char    *d80toa(char    *dst, Double80 x);
// dst must point to memory with at least 26 free wchar_t
wchar_t *d80tow(wchar_t *dst, Double80 x);

#if defined(_UNICODE)
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

#if defined(_UNICODE)
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

class Packer;
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
