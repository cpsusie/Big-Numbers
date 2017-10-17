#pragma once

#include <StrStream.h>
#include <Random.h>
#include "PragmaLib.h"

#pragma check_stack(off)

typedef enum {
  FPU_LOW_PRECISION            // 32-bits floatingpoint
 ,FPU_NORMAL_PRECISION         // 64-bits floatingpoint
 ,FPU_HIGH_PRECISION           // 80-bits floatingpoint
} FPUPrecisionMode;

typedef enum {
  FPU_ROUNDCONTROL_ROUND       // Round to nearest, or to even if equidistant
 ,FPU_ROUNDCONTROL_ROUNDDOWN   // Round down (toward -infinity)
 ,FPU_ROUNDCONTROL_ROUNDUP     // Round up (toward +infinity)
 ,FPU_ROUNDCONTROL_TRUNCATE    // Truncate (toward 0)
} FPURoundMode;

#define FPU_INVALID_OPERATION_EXCEPTION 0x01
#define FPU_DENORMALIZED_EXCEPTION      0x02
#define FPU_DIVIDE_BY_ZERO_EXCEPTION    0x04
#define FPU_OVERFLOW_EXCEPTION          0x08
#define FPU_UNDERFLOW_EXCEPTION         0x10
#define FPU_PRECISION_EXCEPTION         0x20

#define FPU_ALL_EXCEPTIONS              0x3f

#ifdef IS64BIT
extern "C" {
void FPUinit();
void FPUgetStatusWord(WORD &dst);
void FPUgetControlWord(WORD &dst);
void FPUsetControlWord(const WORD &flags);
void FPUgetTagsWord(WORD *buffer); // WORD[14]
void FPUclearExceptions();
};
#endif

class FPU {
private:
  DECLARECLASSNAME;
  FPU() {} // Cannot be instatiated
public:
#ifdef IS32BIT
  static void     init();
  static USHORT   getStatusWord();
  static USHORT   getControlWord();
  static void     setControlWord(USHORT flags);
  static USHORT   getTagsWord();
  static void     clearExceptions();
#else
  static inline void    init() {
    FPUinit();
  }
  static inline USHORT   getStatusWord() {
    WORD tmp;
    FPUgetStatusWord(tmp);
    return tmp;
  }
  static inline USHORT   getControlWord() {
    WORD tmp;
    FPUgetControlWord(tmp);
    return tmp;
  }
  static inline void setControlWord(USHORT flags) {
    FPUsetControlWord(flags);
  }
  static USHORT          getTagsWord() {
    WORD buffer[14];
    FPUgetTagsWord(buffer);
    return buffer[4];
  }
  static inline void      clearExceptions() {
    FPUclearExceptions();
  }
#endif // IS32BIT

  static void             clearStatusWord();
  // returns current FPU controlwoed
  static USHORT           setPrecisionMode(FPUPrecisionMode p);
  static FPUPrecisionMode getPrecisionMode();
  // returns current FPU controlword
  static USHORT           setRoundMode(FPURoundMode mode);
  static FPURoundMode     getRoundMode();
  static inline void      restoreControlWord(USHORT ctrlWord) {
    setControlWord(ctrlWord);
  }
  static int              getStackHeight();
  static bool             stackOverflow();
  static bool             stackUnderflow();
  static bool             stackFault();
  static void             enableExceptions(bool enable, USHORT flags);
};

class Double80;

#ifdef IS64BIT
extern "C" {
void D80consLong(       Double80 &s, const long            &x);
void D80consULong(      Double80 &s, const ULONG            x);
void D80consLongLong(   Double80 &s, const INT64           &x);
void D80consULongLong(  Double80 &s, const UINT64           x);
void D80consFloat(      Double80 &s, float                 &x);
void D80consDouble(     Double80 &s, const double          &x);
void D80ToLong(         long     &dst, const Double80 &src);
void D80ToULong(        ULONG    &dst, const Double80 &src);
void D80ToLongLong(     INT64    &dst, const Double80 &src);
void D80ToULongLong(    UINT64   &dst, const Double80 &src);
void D80ToFloat(        float    &dst, const Double80 &src);
void D80ToDouble(       double   &dst, const Double80 &src);
int  D80cmp(            const Double80 &x, const Double80 &y);
int  D80isZero(         const Double80 &x);
void D80add(            Double80 &dst, const Double80 &x);
void D80sub(            Double80 &dst, const Double80 &x);
void D80mul(            Double80 &dst, const Double80 &x);
void D80div(            Double80 &dst, const Double80 &x);
void D80rem(            Double80 &dst, const Double80 &x);
void D80neg(            Double80 &x);
void D80inc(            Double80 &x);
void D80dec(            Double80 &x);
void D80getPi(          Double80 &dst);
void D80getEps(         Double80 &dst);
void D80getMin(         Double80 &dst);
void D80getMax(         Double80 &dst);
void D80getExpo2(       int      &dst, const Double80 &x);
void D80getExpo10(      int      &dst, const Double80 &x);
void D80fabs(           Double80 &x);
void D80sqr(            Double80 &x);
void D80sqrt(           Double80 &x);
void D80sin(            Double80 &x);
void D80cos(            Double80 &x);
void D80tan(            Double80 &x);
void D80atan(           Double80 &x);
void D80atan2(          Double80 &y, const Double80 &x);
void D80sincos(         Double80 &c, Double80       &s); // inout is c, out s
void D80exp(            Double80 &x);
void D80log(            Double80 &x);
void D80log10(          Double80 &x);
void D80log2(           Double80 &x);
void D80pow(            Double80 &x, const Double80 &y); // x = pow(x,y)
void D80pow10(          Double80 &x);
void D80pow2(           Double80 &x);
void D80floor(          Double80 &x);
void D80ceil(           Double80 &x);
void D80ToBCD(BYTE bcd[10], const Double80 &src);
void D80ToBCDAutoScale(BYTE bcd[10], const Double80 &x, int &expo10);
}
#endif // IS64BIT

class Double80 {
private:
  DECLARECLASSNAME;
  BYTE m_value[10]; // Must be the first field in the class
  void init(const _TUCHAR *s);

public:
  inline Double80() {
  }

  inline Double80(int x) {
    *this = (Double80)(long)(x);
  }

  inline Double80(UINT x) {
    *this = (Double80)(ULONG)(x);
  }

#ifdef IS32BIT
  Double80(long             x);
  Double80(ULONG            x);
  Double80(INT64            x);
  Double80(UINT64           x);
  Double80(float            x);
  Double80(double           x);
#else
  inline Double80(long x) {
    D80consLong(*this, x);
  }
  inline Double80(ULONG x) {
    D80consULong(*this, x);
  }
  inline Double80(INT64 x) {
    D80consLongLong(*this, x);
  }
  inline Double80(UINT64 x) {
    D80consULongLong(*this, x);
  }
  inline Double80(float x) {
    D80consFloat(*this, x);
  }
  inline Double80(double x) {
    D80consDouble(*this, x);
  }
#endif // IS32BIT

  explicit Double80(const String  &s);
  explicit Double80(const wchar_t *s);
  explicit Double80(const char    *s);
  explicit Double80(const BYTE    *bytes);

  inline friend int getInt(const Double80 &x) {
    return getLong(x);
  }

  inline friend UINT getUint(const Double80 &x) {
    return getUlong(x);
  }

#ifdef IS32BIT
  friend long   getLong(  const Double80 &x);
  friend ULONG  getUlong( const Double80 &x);
  friend INT64  getInt64( const Double80 &x);
  friend UINT64 getUint64(const Double80 &x);
  friend float  getFloat( const Double80 &x);
  friend double getDouble(const Double80 &x);

  static int    getExpo2( const Double80 &x);
  static int    getExpo10(const Double80 &x); // x == 0 ? 0 : floor(log10(|x|))

  Double80 &operator+=(const Double80 &x);
  Double80 &operator-=(const Double80 &x);
  Double80 &operator*=(const Double80 &x);
  Double80 &operator/=(const Double80 &x);

  Double80 &operator++();   // prefix-form
  Double80 &operator--();   // prefix-form
  Double80 operator++(int); // postfix-form
  Double80 operator--(int); // postfix-form

  bool isZero()     const;

#else // !IS32BIT (ie IS64BIT)

  friend inline long getLong(const Double80 &x) {
    long tmp;
    D80ToLong(tmp, x);
    return tmp;
  }

  friend inline ULONG getUlong(const Double80 &x) {
    ULONG tmp;
    D80ToULong(tmp, x);
    return tmp;
  }

  friend inline INT64 getInt64(const Double80 &x) {
    INT64 tmp;
    D80ToLongLong(tmp, x);
    return tmp;
  }

  friend inline UINT64 getUint64(const Double80 &x) {
    UINT64 tmp;
    D80ToULongLong(tmp, x);
    return tmp;
  }

  friend inline float getFloat(const Double80 &x) {
    float tmp;
    D80ToFloat(tmp, x);
    return tmp;
  }

  friend inline double getDouble(const Double80 &x) {
    double tmp;
    D80ToDouble(tmp, x);
    return tmp;
  }

  static inline int getExpo2(const Double80 &x) {
    int result;
    D80getExpo2(result, x);
    return result;
  }

  static inline int getExpo10(const Double80 &x) { // x == 0 ? 0 : floor(log10(|x|))
    int result;
    D80getExpo10(result, x);
    return result;
  }

  inline Double80 &operator+=(const Double80 &x) {
    D80add(*this, x);
    return *this;
  }

  inline Double80 &operator-=(const Double80 &x) {
    D80sub(*this, x);
    return *this;
  }

  inline Double80 &operator*=(const Double80 &x) {
    D80mul(*this, x);
    return *this;
  }

  inline Double80 &operator/=(const Double80 &x) {
    D80div(*this, x);
    return *this;
  }

  inline Double80 &operator++() {   // prefix-form
    D80inc(*this);
    return *this;
  }

  inline Double80 &operator--() {   // prefix-form
    D80dec(*this);
    return *this;
  }

  inline Double80 operator++(int) { // postfix-form
    Double80 result(*this);
    D80inc(*this);
    return result;
  }

  inline Double80 operator--(int) { // postfix-form
    Double80 result(*this);
    D80dec(*this);
    return result;
  }

  inline bool isZero() const {
    return D80isZero(*this) ? true : false;
  }

#endif // IS32BIT

  static TCHAR   *d80tot(TCHAR   *dst, const Double80 &x); // dst must point to memory with at least 26 free TCHAR
  static char    *d80toa(char    *dst, const Double80 &x); // dst must point to memory with at least 26 free char
  static wchar_t *d80tow(wchar_t *dst, const Double80 &x); // dst must point to memory with at least 26 free wchar_t

  inline bool isPositive() const { return (m_value[9] & 0x80) == 0; }
  inline bool isNegative() const { return (m_value[9] & 0x80) != 0; }

  ULONG hashCode() const;

  String toString() const;

  inline void save(ByteOutputStream &s) const {
    s.putBytes((BYTE*)m_value, sizeof(m_value));
  }

  inline void load(ByteInputStream &s) {
    s.getBytesForced((BYTE*)m_value, sizeof(m_value));
  }

  friend inline Packer &operator<<(Packer &p, const Double80 &d) {
    return p.addElement(Packer::E_DOUBLE, d.m_value, sizeof(d.m_value));
  }

  friend inline Packer &operator>>(Packer &p, Double80 &d) {
    p.getElement(Packer::E_DOUBLE, d.m_value, sizeof(d.m_value));
    return p;
  }

  static const Double80 zero;          // = 0
  static const Double80 one;           // = 1
  static const Double80 M_PI;          // = 3.1415926535897932384626433
  static const Double80 DBL80_EPSILON; // Smallest such that 1.0+DBL80_EPSILON != 1.0 (=1.08420217248550443e-019)
  static const Double80 DBL80_MIN;     // Min positive value (=3.36210314311209209e-4932)
  static const Double80 DBL80_MAX;     // Max value          (=1.18973149535723227e+4932)
  static const int      DBL80_DIG;     // # of decimal digits of precision

  static void initClass();
};

#ifdef IS32BIT

Double80 operator+(const Double80 &x, const Double80 &y);
Double80 operator-(const Double80 &x, const Double80 &y);
Double80 operator-(const Double80 &x);
Double80 operator*(const Double80 &x, const Double80 &y);
Double80 operator/(const Double80 &x, const Double80 &y);

bool operator==(const Double80 &x, const Double80 &y);
bool operator!=(const Double80 &x, const Double80 &y);
bool operator<=(const Double80 &x, const Double80 &y);
bool operator>=(const Double80 &x, const Double80 &y);
bool operator< (const Double80 &x, const Double80 &y);
bool operator> (const Double80 &x, const Double80 &y);

Double80 fabs(  const Double80 &x);
Double80 fmod(  const Double80 &x, const Double80 &y);
Double80 sqr(   const Double80 &x);
Double80 sqrt(  const Double80 &x);
Double80 sin(   const Double80 &x);
Double80 cos(   const Double80 &x);
Double80 tan(   const Double80 &x);
Double80 atan(  const Double80 &x);
Double80 atan2( const Double80 &y, const Double80 &x);
void     sincos(Double80 &c, Double80 &s); // calculate both cos and sin. c:inout c, s:out
Double80 exp(  const Double80 &x);
Double80 log(  const Double80 &x);
Double80 log10(const Double80 &x);
Double80 log2( const Double80 &x);
Double80 pow(  const Double80 &x, const Double80 &y);
Double80 pow10(const Double80 &x);
Double80 pow2( const Double80 &x);
Double80 floor(const Double80 &x);
Double80 ceil( const Double80 &x);

#else // !IS32BIT (ie IS64BIT)

inline Double80 operator+(const Double80 &x, const Double80 &y) {
  Double80 tmp(x);
  D80add(tmp, y);
  return tmp;
}

inline Double80 operator-(const Double80 &x, const Double80 &y) {
  Double80 tmp(x);
  D80sub(tmp, y);
  return tmp;
}

inline Double80 operator-(const Double80 &x) {
  Double80 tmp(x);
  D80neg(tmp);
  return tmp;
}

inline Double80 operator*(const Double80 &x, const Double80 &y) {
  Double80 tmp(x);
  D80mul(tmp, y);
  return tmp;
}

inline Double80 operator/(const Double80 &x, const Double80 &y) {
  Double80 tmp(x);
  D80div(tmp, y);
  return tmp;
}

inline bool operator==(const Double80 &x, const Double80 &y) {
  return D80cmp(x, y) == 0;
}

inline bool operator!=(const Double80 &x, const Double80 &y) {
  return D80cmp(x, y) != 0;
}

inline bool operator<=(const Double80 &x, const Double80 &y) {
  return D80cmp(x, y) <= 0;
}

inline bool operator>=(const Double80 &x, const Double80 &y) {
  return D80cmp(x, y) >= 0;
}

inline bool operator< (const Double80 &x, const Double80 &y) {
  return D80cmp(x, y) < 0;
}

inline bool operator> (const Double80 &x, const Double80 &y) {
  return D80cmp(x, y) > 0;
}

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

inline void sincos(Double80 &c, Double80 &s) { // calculate both cos and sin. c:inout c, s:out
  D80sincos(c, s);
}

inline Double80 exp(const Double80 &x) {
  Double80 tmp(x);
  D80exp(tmp);
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

inline Double80 pow10(const Double80 &x) {
  Double80 tmp(x);
  D80pow10(tmp);
  return tmp;
}

inline Double80 pow2(const Double80 &x) {
  Double80 tmp(x);
  D80pow2(tmp);
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

#endif // IS32BIT

Double80 cot(  const Double80 &x);
Double80 asin( const Double80 &x);
Double80 acos( const Double80 &x);
Double80 acot( const Double80 &x);
Double80 root( const Double80 &x, const Double80 &y);
Double80 cosh( const Double80 &x);
Double80 sinh( const Double80 &x);
Double80 tanh( const Double80 &x);
Double80 acosh(const Double80 &x);
Double80 asinh(const Double80 &x);
Double80 atanh(const Double80 &x);
Double80 hypot(const Double80 &x, const Double80 &y);
Double80 fraction(   const Double80 &x);
int      sign(       const Double80 &x);
Double80 round(      const Double80 &x, int prec = 0);
Double80 Max(        const Double80 &x, const Double80 &y);
Double80 Min(        const Double80 &x, const Double80 &y);
Double80 minMax(     const Double80 &x, const Double80 &x1, const Double80 &x2);
bool     isNan(      const Double80 &x);
bool     isPInfinity(const Double80 &x);
bool     isNInfinity(const Double80 &x);
bool     isInfinity( const Double80 &x);

Double80 randDouble80(Random *rnd = NULL);
Double80 randDouble80(const Double80 &low, const Double80 &high, Random *rnd = NULL);

String toString(      const Double80 &x, int precision=6, int width=0, int flags=0);

tistream &operator>>(tistream &s,       Double80 &x);
tostream &operator<<(tostream &s, const Double80 &x);

StrStream &operator<<(StrStream &stream, const Double80 &x);
