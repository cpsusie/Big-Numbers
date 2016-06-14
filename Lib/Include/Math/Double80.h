#pragma once

#include <StrStream.h>
#include <Random.h>
#include "PragmaLib.h"

#ifdef _DEBUG1
#define _DEBUGD80
#endif

#ifdef _DEBUGD80
#include <Semaphore.h>
#endif

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

class FPU {
private:
  FPU() {} // Cannot be instatiated
public:
  static unsigned short   getStatusWord();
  static unsigned short   getTagsWord();
  static unsigned short   getControlWord();
  static void             setControlWord(unsigned short flags);
  static void             setPrecisionMode(FPUPrecisionMode p); // returns old precision mode
  static FPUPrecisionMode getPrecisionMode();
  static void             setRoundMode(FPURoundMode mode);      // returns old round mode
  static FPURoundMode     getRoundMode();
  static int              getStackHeight();
  static bool             stackOverflow();
  static bool             stackUnderflow();
  static bool             stackFault();
  static void             enableExceptions(bool enable, unsigned short flags);

  static void             init();
  static void             clearStatusWord();
  static void             clearExceptions();
};

#ifdef _DEBUGD80
#define SETD80DEBUGSTRING(x) { if(Double80::s_debugStringEnabled) (x).ajourDebugString(); }
#else
#define SETD80DEBUGSTRING(x)
#endif

#ifdef _DEBUGD80

class _TenByte {
private:
  char m_value[10];
public:
  inline _TenByte() {};
  _TenByte(const Double80 &src);
};

#define TenByteClass _TenByte

#else

class Double80;
#define TenByteClass Double80

#endif

#ifdef IS64BIT
extern "C" void consD80Long(       TenByteClass &s, const long             &x);
extern "C" void consD80ULong(      TenByteClass &s, const unsigned long     x);
extern "C" void consD80LongLong(   TenByteClass &s, const __int64          &x);
extern "C" void consD80ULongLong(  TenByteClass &s, const unsigned __int64  x);
extern "C" void consD80Float(      TenByteClass &s, float                  &x);
extern "C" void consD80Double(     TenByteClass &s, const double           &x);
extern "C" long               &D80ToLong(      long               &dst, const TenByteClass &src);
extern "C" unsigned long      &D80ToULong(     unsigned long      &dst, const TenByteClass &src);
extern "C" long long          &D80ToLongLong(  long long          &dst, const TenByteClass &src);
extern "C" unsigned long long &D80ToULongLong( unsigned long long &dst, const TenByteClass &src);
extern "C" float              &D80ToFloat(     float              &dst, const TenByteClass &src);
extern "C" double             &D80ToDouble(    double             &dst, const TenByteClass &src);
extern "C" TenByteClass       &sumD80D80(TenByteClass &s, const TenByteClass &x, const TenByteClass &y);
#endif

class Double80 {
private:
  char m_value[10]; // Must be the first field in the class
  void init(const _TUCHAR *s);

#ifdef _DEBUGD80
  mutable char m_debugString[30];
  friend class InitDouble80;
  friend class _TenByte;
public:
  Double80(const _TenByte &src);
  inline void ajourDebugString() const {
    d80toa(m_debugString, *this);
  }
  static Semaphore s_debugStringGate;
  static bool      s_debugStringEnabled;
#endif

public:
  Double80();
  Double80(int              x);
  Double80(unsigned int     x);

#ifdef IS32BIT
  Double80(long             x);
  Double80(unsigned long    x);
  Double80(__int64          x);
  Double80(unsigned __int64 x);
  Double80(float            x);
  Double80(double           x);
#else
  inline Double80(long x) {
    consD80Long(*this, x);
  }
  inline Double80(unsigned long x) {
    consD80ULong(*this, x);
  }
  inline Double80(__int64 x) {
    consD80LongLong(*this, x);
  }
  inline Double80(unsigned __int64 x) {
    consD80ULongLong(*this, x);
  }
  inline Double80(float x) {
    consD80Float(*this, x);
  }
  inline Double80(double x) {
    consD80Double(*this, x);
  }
#endif
  explicit Double80(const String &s);
  explicit Double80(const TCHAR  *s);
  explicit Double80(const BYTE   *bytes);
#ifdef UNICODE
  explicit Double80(const char   *s);
#endif
  friend          int     getInt( const Double80 &x);
  friend unsigned int     getUint(const Double80 &x);

#ifdef IS32BIT
  friend          long    getLong(  const Double80 &x);
  friend unsigned long    getUlong( const Double80 &x);
  friend          __int64 getInt64( const Double80 &x);
  friend unsigned __int64 getUint64(const Double80 &x);
  friend float            getFloat( const Double80 &x);
  friend double           getDouble(const Double80 &x);
#else
  friend long getLong(const Double80 &x) {
    long tmp; return D80ToLong(tmp, x);
  }
  friend unsigned long getUlong(const Double80 &x) {
    unsigned long tmp; return D80ToULong(tmp, x);
  }
  friend __int64 getInt64(const Double80 &x) {
    long long tmp; return D80ToLongLong(tmp, x);
  }
  friend unsigned __int64 getUint64(const Double80 &x) {
    unsigned long long tmp; return D80ToULongLong(tmp, x);
  }
  friend float getFloat(const Double80 &x) {
    float tmp; return D80ToFloat(tmp, x);
  }
  friend inline double getDouble(const Double80 &x) {
    double tmp; return D80ToDouble(tmp, x);
  }
#endif

  static int              getExpo2(const Double80 &x);
  static int              getExpo10(const Double80 &x); // x == 0 ? 0 : floor(log10(|x|))

  Double80 &operator+=(const Double80 &x);
  Double80 &operator-=(const Double80 &x);
  Double80 &operator*=(const Double80 &x);
  Double80 &operator/=(const Double80 &x);
  Double80 &operator++();   // prefix-form
  Double80 &operator--();   // prefix-form
  Double80 operator++(int); // postfix-form
  Double80 operator--(int); // postfix-form

  static TCHAR *d80tot(TCHAR *dst, const Double80 &x); // dst must point to memory with at least 26 free TCHAR
  static char  *d80toa(char  *dst, const Double80 &x); // dst must point to memory with at least 26 free char
  bool isZero()     const;
  bool isPositive() const { return (m_value[9] & 0x80) == 0; }
  bool isNegative() const { return (m_value[9] & 0x80) != 0; }

  unsigned long hashCode() const;

  String toString() const;

  inline void save(ByteOutputStream &s) const {
    s.putBytes((BYTE*)m_value, sizeof(m_value));
  }

  inline void load(ByteInputStream &s) {
    s.getBytesForced((BYTE*)m_value, sizeof(m_value));
    SETD80DEBUGSTRING(*this);
  }

  friend inline Packer &operator<<(Packer &p, const Double80 &d) {
    return p.addElement(Packer::E_DOUBLE, d.m_value, sizeof(d.m_value));
  }

  friend inline Packer &operator >> (Packer &p, Double80 &d) {
    p.getElement(Packer::E_DOUBLE, d.m_value, sizeof(d.m_value));
    SETD80DEBUGSTRING(d);
    return p;
  }

  static void enableDebugString(bool enabled);
  static const Double80 zero;          // = 1
  static const Double80 one;           // = 0
  static const Double80 M_PI;          // = 3.1415926535897932384626433
  static const Double80 DBL80_EPSILON; // Smallest such that 1.0+DBL80_EPSILON != 1.0 (=1.08420217248550443e-019)
  static const Double80 DBL80_MIN;     // Min positive value (=3.36210314311209209e-4932)
  static const Double80 DBL80_MAX;     // Max value          (=1.18973149535723227e+4932)
  static const int      DBL80_DIG;     // # of decimal digits of precision

  static void initClass();
};

#ifdef IS32BIT
Double80 operator+(const Double80 &x, const Double80 &y);
#else
inline Double80 operator+(const Double80 &x, const Double80 &y) {
  TenByteClass sum;
  return sumD80D80(sum, x, y);
}
#endif

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
Double80 fabs(const Double80 &x);
Double80 fmod(const Double80 &x, const Double80 &y);
Double80 sqr(const Double80 &x);
Double80 sqrt(const Double80 &x);
Double80 sin(const Double80 &x);
Double80 cos(const Double80 &x);
void     sincos(Double80 &c, Double80 &s); // calculate both cos and sin. c:inout c, s:out
Double80 tan(const Double80 &x);
Double80 cot(const Double80 &x);
Double80 asin(const Double80 &x);
Double80 acos(const Double80 &x);
Double80 atan(const Double80 &x);
Double80 acot(const Double80 &x);
Double80 atan2(const Double80 &y, const Double80 &x);
Double80 exp(const Double80 &x);
Double80 log(const Double80 &x);
Double80 log10(const Double80 &x);
Double80 log2(const Double80 &x);
Double80 pow(const Double80 &x, const Double80 &y);
Double80 pow10(const Double80 &x);
Double80 pow2(const Double80 &x);
Double80 root(const Double80 &x, const Double80 &y);
Double80 floor(const Double80 &x);
Double80 ceil(const Double80 &x);
Double80 fraction(const Double80 &x);
int      sign(const Double80 &x);
Double80 round(const Double80 &x, int prec = 0);
Double80 Max(const Double80 &x, const Double80 &y);
Double80 Min(const Double80 &x, const Double80 &y);
bool     isNan(const Double80 &x);
bool     isPInfinity(const Double80 &x);
bool     isNInfinity(const Double80 &x);
bool     isInfinity(const Double80 &x);


class RandomD80 : public Random {
public:
  Double80 nextDouble80();
  Double80 nextDouble80(const Double80 &low, const Double80 &high);
};

String toString(      const Double80 &x, int precision=6, int width=0, int flags=0);

tistream &operator>>(tistream &s,       Double80 &x);
tostream &operator<<(tostream &s, const Double80 &x);

StrStream &operator<<(StrStream &stream, const Double80 &x);
