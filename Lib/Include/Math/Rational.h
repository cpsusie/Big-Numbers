#pragma once

#include <CommonHashFunctions.h>
#include "Double80.h"
#include "Real.h"

class Rational {
private:
  INT64 m_numerator, m_denominator;

  void init(const INT64 &numerator, const INT64 &denominator);
  static INT64 pow(INT64 n, UINT y);
public:

  inline Rational() : m_numerator(0), m_denominator(1) {
  }
  inline Rational(const INT64 &numerator, const INT64 &denominator) {
    init(numerator, denominator);
  }
  inline Rational(const INT64 &numerator, int denominator) {
    init(numerator, denominator);
  }
  inline Rational(int numerator, int denominator) {
    init(numerator, denominator);
  }
  inline Rational(int numerator, const INT64 &denominator) {
    init(numerator, denominator);
  }
  inline Rational(const INT64 &n) : m_numerator(n), m_denominator(1) {
  }
  inline Rational(int n) : m_numerator(n), m_denominator(1) {
  }
  inline Rational(UINT n) : m_numerator(n), m_denominator(1) {
  }
  explicit Rational(float           f  , UINT   maxND = _I16_MAX);
  explicit Rational(double          d  , UINT   maxND = _I32_MAX);
  explicit Rational(const Double80 &d80, UINT64 maxND = _I64_MAX);

  static INT64 safeSum( const TCHAR *method, int line, const INT64 &a, const INT64 &b);
  static INT64 safeDif( const TCHAR *method, int line, const INT64 &a, const INT64 &b);
  static INT64 safeProd(const TCHAR *method, int line, const INT64 &a, const INT64 &b);

  friend Rational operator+(const Rational &l, const Rational &r);
  friend Rational operator-(const Rational &l, const Rational &r);
  friend Rational operator-(const Rational &r);
  friend Rational operator*(const Rational &l, const Rational &r);
  friend Rational operator/(const Rational &l, const Rational &r);
  // sign(l % r) = sign(l), equivalent to built-in % operator
  friend Rational operator%(const Rational &l, const Rational &r);

  inline Rational &operator+=(const Rational &r) {
    return *this = *this + r;
  }
  inline Rational &operator-=(const Rational &r) {
    return *this = *this - r;
  }
  inline Rational &operator*=(const Rational &r) {
    return *this = *this * r;
  }
  inline Rational &operator/=(const Rational &r) {
    return *this = *this / r;
  }
  inline Rational &operator%=(const Rational &r) {
    return *this = *this % r;
  }
  inline Rational &operator++() {   // prefix-form
    return *this += _1;
  }
  inline Rational &operator--() {   // prefix-form
    return *this -= _1;
  }
  inline Rational operator++(int) { // postfix-form
    const Rational result(*this);
    ++(*this);
    return result;
  }
  inline Rational operator--(int) { // postfix-form
    const Rational result(*this);
    --(*this);
    return result;
  }

  // Assume isfinite(r1) && isfinite(r2)
  friend int rationalCmp(const Rational &r1, const Rational &r2);

  friend Rational fabs(const Rational &r);
  friend Rational pow( const Rational &r, int e);
  friend Rational reciprocal(const Rational &r);

  inline bool isZero() const {
    return (m_numerator == 0) && (m_denominator == 1);
  }
  inline bool isNegative() const {
    return m_numerator < 0;
  }
  inline bool isPositive() const {
    return m_numerator > 0;
  }
  inline bool isInteger() const {
    return m_denominator == 1;
  }
  static UINT64 findGCD(UINT64 a, UINT64 b);

  inline const INT64 &getNumerator() const {
    return m_numerator;
  }

  inline const INT64 &getDenominator() const {
    return m_denominator;
  }
  // Return true if x is a rational with denominator<=400.
  // if(r != NULL), *r will contain converted Rational
  static bool isRational(float           x, Rational *r);
  static bool isRational(double          x, Rational *r);
  static bool isRational(const Double80 &x, Rational *r);
  // Return true if base^e is rational
  // if(r != NULL), *r will contain the calculated value
  static bool isRationalPow(const Rational &base, const Rational &e, Rational *r);

  template<class T> static T pow(T b, const Rational &e) {
    if(!isfinite(b) || !isfinite(e) || (e.isZero() && (b <= 0))) {
      return numeric_limits<T>::quiet_NaN();
    }
    const INT64 den = e.getDenominator();
    if(isEven(den) && (b < 0)) {
      return numeric_limits<T>::quiet_NaN();
    }
    const T result = (den == 1) ? b : root(b, den);
    return (e.getNumerator() == 1) ? result : mypow(result, e.getNumerator());
  }

  inline ULONG hashCode() const {
    return int64Hash(m_numerator) + 100999 * int64Hash(m_denominator);
  }

  inline void save(ByteOutputStream &s) const {
    s.putBytes((BYTE*)this, sizeof(Rational));
  }
  inline void load(ByteInputStream  &s) {
    s.getBytesForced((BYTE*)this, sizeof(Rational));
  }

  static const Rational _0;           // = 0
  static const Rational _05;          // 0.5
  static const Rational _1;           // = 1
  static const Rational _RAT_MIN;     // Min positive value (=1/_I64_MAX)
  static const Rational _RAT_MAX;     // Max value          (=_I64_MAX)
  static const Rational _RAT_NAN;     // nan (undefined)    ( 0/0)
  static const Rational _RAT_PINF;    // +infinity;         ( 1/0)
  static const Rational _RAT_NINF;    // -infinity;         (-1/0)
};

// Return uniform distributed random rational in range [0;1[ using rnd to generate numerator and denominator
// The denominator (den) will be uniform distributed in the range [2; maxDenominator]
// The numerator will be uniform distributed in range [0; den[
// The valid range for maxDenominator is [2;INT64_MAX]. Default:INT64_MAX
Rational randRational(UINT64 maxDenominator = INT64_MAX        , RandomGenerator *rnd = _standardRandomGenerator);
// Return uniform distributed random rational in range [0;1[ using rnd to generate numerator and denominator
// The denominator (den) will be uniform distributed in the range [2; INT_MAX]
// The numerator will be uniform distributed in range [0; den[
inline Rational randRational(RandomGenerator *rnd = _standardRandomGenerator) {
  return randRational(INT64_MAX, rnd);
}
// Return uniform distributed random rational in range [low;high] (both inclusive)
// Assume the 3 products: low.num * high.den, high.num * low.den, low.den * high.den are all <= _I64_MAX
// if this is not the case, an exception is thrown
// To avoid overflow in calculation, keep the involved factors < _I32_MAX
Rational randRational(const Rational &low, const Rational &high, RandomGenerator *rnd = _standardRandomGenerator);

// Return true, if denominator == 1
inline bool     isInteger(  const Rational &r) {  return r.getDenominator() == 1;                             }
// Return true, if isInteger() && isShort(numerator)
inline bool     isShort(    const Rational &r) { return r.isInteger() && isShort( r.getNumerator());          }
// Return true, if isInteger() && isUshort(numerator)
inline bool     isUshort(   const Rational &r) { return r.isInteger() && isUshort(r.getNumerator());          }
// Return true, if isInteger() && isInt(numerator)
inline bool     isInt(      const Rational &r) { return r.isInteger() && isInt(   r.getNumerator());          }
// Return true, if isInteger() && isUint(numerator)
inline bool     isUiInt(    const Rational &r) { return r.isInteger() && isUint(  r.getNumerator());          }
// Return true, if isInteger() && isInt64(numerator)
inline bool     isInt64(    const Rational &r) { return r.isInteger() && isInt64( r.getNumerator());          }
// Return true, if isInteger() && isUint64(numerator)
inline bool     isUint64(   const Rational &r) { return r.isInteger() && isUint64(r.getNumerator());          }
inline INT64    getInt64(   const Rational &r) { return r.getNumerator() / r.getDenominator();                }
inline UINT64   getUint64(  const Rational &r) { return (UINT64)r.getNumerator() / r.getDenominator();        }
inline short    getShort(   const Rational &r) { return (short)getInt64(r);                                   }
inline USHORT   getUshort(  const Rational &r) { return (USHORT)getUint64(r);                                 }
inline int      getInt(     const Rational &r) { return (int)getInt64(r);                                     }
inline UINT     getUint(    const Rational &r) { return (UINT)getUint64(r);                                   }
inline long     getLong(    const Rational &r) { return getInt(r);                                            }
inline ULONG    getUlong(   const Rational &r) { return getUint(r);                                           }
inline float    getFloat(   const Rational &r) { return (float)((double)r.getNumerator()/r.getDenominator()); }
inline double   getDouble(  const Rational &r) { return (double)r.getNumerator()/r.getDenominator();          }
inline Double80 getDouble80(const Rational &r) { return Double80(r.getNumerator())/r.getDenominator();        }

inline Real     getReal(    const Rational &r) {
#ifdef LONGDOUBLE
  return getDouble80(r);
#else
  return getDouble(r);
#endif
}

inline int sign(const Rational &r) {
  return ::sign(r.getNumerator());
}

inline bool isEven(const Rational &r) {
  return isInteger(r) && isEven(r.getNumerator());
}
inline bool isOdd(const Rational &r) {
  return isInteger(r) && isOdd(r.getNumerator());
}
inline bool isSymmetricExponent(const Rational &r) {
  return isEven(r.getNumerator()) || isEven(r.getDenominator());
}

inline bool isAsymmetricExponent(const Rational &r) {
  return isOdd(r.getNumerator()) && isOdd(r.getDenominator());
}

inline bool rationalExponentsMultiply(const Rational &r1, const Rational &r2) {
  return isAsymmetricExponent(r1) || isAsymmetricExponent(r2);
}

// returns one of
//  FP_NORMAL
//  FP_ZERO
//  FP_INFINITE
//  FP_NAN
int fpclassify(const Rational &r);

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
int _fpclass(const Rational &r);

inline bool isfinite(const Rational &r) {
  return fpclassify(r) <= 0;
}
inline bool isinf(const Rational &r) {
  return fpclassify(r) == FP_INFINITE;
}
inline bool isnan(const Rational &r) {
  return fpclassify(r) == FP_NAN;
}
inline bool isnormal(const Rational &r) {
  return fpclassify(r) == FP_NORMAL;
}
inline bool isunordered(const Rational &x, const Rational &y) {
  return isnan(x) || isnan(y);
}
inline bool isPInfinity(const Rational &r) {
  return isinf(r) && r.isPositive();
}
inline bool isNInfinity(const Rational &r) {
  return isinf(r) && r.isNegative();
}

inline bool operator< (const Rational &x, const Rational &y) {
  return !isunordered(x,y) && (rationalCmp(x, y) < 0);
}
inline bool operator> (const Rational &x, const Rational &y) {
  return !isunordered(x,y) && (rationalCmp(x, y) > 0);
}
inline bool operator<=(const Rational &x, const Rational &y) {
  return !isunordered(x,y) && (rationalCmp(x, y) <= 0);
}
inline bool operator>=(const Rational &x, const Rational &y) {
  return !isunordered(x,y) && (rationalCmp(x, y) >= 0);
}
inline bool operator==(const Rational &x, const Rational &y) {
  return !isunordered(x,y) && ((x.getNumerator() == y.getNumerator()) && (x.getDenominator() == y.getDenominator()));
}
inline bool operator!=(const Rational &x, const Rational &y) {
  return !isunordered(x,y) && ((x.getNumerator() != y.getNumerator()) || (x.getDenominator() != y.getDenominator()));
}


char    *rattoa(char    *dst, const Rational &r, int radix);
wchar_t *rattow(wchar_t *dst, const Rational &r, int radix);

#ifdef _UNICODE
#define _rattot rattow
#else
#define _rattot rattoa
#endif

String toString(const Rational &r, StreamSize precision=0, StreamSize width=0, FormatFlags flags=0);

Rational strtorat(const char *s   , char    **end, int radix);
Rational wcstorat(const wchar_t *s, wchar_t **end, int radix);

#ifdef _UNICODE
#define _tcstorat wcstorat
#else
#define _tcstorat strtorat
#endif // _UNICODE

std::istream  &operator>>(std::istream  &in,        Rational &r);
std::ostream  &operator<<(std::ostream  &out, const Rational &r);

std::wistream &operator>>(std::wistream &in,        Rational &r);
std::wostream &operator<<(std::wostream &out, const Rational &r);

Packer &operator<<(Packer &p, const Rational &r);
Packer &operator>>(Packer &p,       Rational &r);

// STRUCT _Num_int_base
struct _Num_Rational_base
  : std::_Num_base
{	// base for Rational types
  static constexpr bool is_bounded     = true;
  static constexpr bool is_exact       = true;
  static constexpr bool is_integer     = false;
  static constexpr bool has_infinity   = true;
  static constexpr bool has_quiet_NaN  = true;
  static constexpr bool is_modulo      = false;
  static constexpr bool is_signed      = true;
  static constexpr bool is_specialized = true;
  static constexpr int  radix          = 2;
};

// CLASS numeric_limits<Double80>
template<> class std::numeric_limits<Rational>
    : public _Num_Rational_base
{	// limits for type Rational
public:
  _NODISCARD static Rational (min)() noexcept
  {	// return minimum value
    return Rational::_RAT_MIN;
  }

  _NODISCARD static Rational (max)() noexcept
  {	// return maximum value
    return Rational::_RAT_MAX;
  }

  _NODISCARD static Rational lowest() noexcept
  {	// return most negative value
    return (-(max)());
  }

  _NODISCARD static Rational epsilon() noexcept
  {	// return smallest effective increment from 1.0
    return Rational::_RAT_MIN;
  }

  _NODISCARD static Rational round_error() noexcept
  {	// return largest rounding error
    return Rational::_0;
  }

  _NODISCARD static Rational denorm_min() noexcept
  {	// return minimum denormalized value
    return Rational::_RAT_MIN;
  }

  _NODISCARD static Rational infinity() noexcept
  {	// return positive infinity
    return Rational::_RAT_PINF;
  }

  _NODISCARD static Rational quiet_NaN() noexcept
  {	// return non-signaling NaN
    return Rational::_RAT_NAN;
  }

  _NODISCARD static Rational signaling_NaN() noexcept
  {	// return signaling NaN
    return Rational::_RAT_NAN;
  }
};

#define RAT_MIN  Rational::_RAT_MIN                     // Min positive value (=1/_I64_MAX)
#define RAT_MAX  Rational::_RAT_MAX                     // Max value          (=_I64_MAX)
#define RAT_NAN  Rational::_RAT_NAN                     // nan (undefined)    ( 0/0)
#define RAT_PINF Rational::_RAT_PINF                    // +infinity;         ( 1/0)
#define RAT_NINF Rational::_RAT_NINF
