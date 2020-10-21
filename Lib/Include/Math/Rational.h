#pragma once

#include <CommonHashFunctions.h>
#include "Double80.h"

class Rational {
private:
  INT64 m_num; // numerator
  INT64 m_den; // denominator

  void init(INT64 numerator, INT64 denominator);
  // Assume e >= 0, return base^e. pow(0,0) == 1
  static INT64 pow(INT64 base, UINT e);
  static Rational sum128( const Rational &l, const Rational &r);
  static Rational dif128( const Rational &l, const Rational &r);
  static Rational prod128(const Rational &l, const Rational &r);
  static Rational mod128( const Rational &l, const Rational &r);
public:

  inline Rational() : m_num(0), m_den(1) {
  }
  inline Rational(INT64 numerator, INT64 denominator) {
    init(numerator, denominator);
  }
  inline Rational(INT64 numerator, int denominator) {
    init(numerator, denominator);
  }
  inline Rational(int numerator, int denominator) {
    init(numerator, denominator);
  }
  inline Rational(int numerator, INT64 denominator) {
    init(numerator, denominator);
  }
  inline Rational(INT64 n) : m_num(n), m_den(1) {
  }
  inline Rational(int n) : m_num(n), m_den(1) {
  }
  inline Rational(UINT n) : m_num(n), m_den(1) {
  }
  explicit Rational(float           f  , UINT   maxND = _I16_MAX);
  explicit Rational(double          d  , UINT   maxND = _I32_MAX);
  explicit Rational(const Double80 &d80, UINT64 maxND = _I64_MAX);

  inline explicit operator short()    const { return (short )operator __int64();  }
  inline explicit operator USHORT()   const { return (USHORT)operator UINT64();   }
  inline explicit operator int()      const { return (int   )operator __int64();  }
  inline explicit operator UINT()     const { return (UINT  )operator UINT64();   }
  inline explicit operator long()     const { return (long  )operator __int64();  }
  inline explicit operator ULONG()    const { return (ULONG )operator UINT64();   }
  inline explicit operator __int64()  const { return         m_num / m_den;       }
  inline explicit operator UINT64()   const { return (UINT64)m_num / m_den;       }
  inline explicit operator float()    const { return (float )operator Double80(); }
  inline explicit operator double()   const { return (double)operator Double80(); }
  inline explicit operator Double80() const { return Double80(m_num) / m_den;     }

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
  // return NaN if !isfinite(base)
  // if(e==0) return NaN for base == 0, else 1
  // if(e< 0) return +inf for base == 0
  friend Rational pow( const Rational &base, int e);
  friend Rational reciprocal(const Rational &r);

  inline bool isZero() const {
    return (m_num == 0) && (m_den == 1);
  }
  inline bool isNegative() const {
    return m_num < 0;
  }
  inline bool isPositive() const {
    return m_num > 0;
  }
  inline bool isInteger() const {
    return m_den == 1;
  }
  static UINT64 findGCD(UINT64 a, UINT64 b);

  inline INT64 getNumerator() const {
    return m_num;
  }

  inline INT64 getDenominator() const {
    return m_den;
  }
  // Return true if x is a rational with denominator<=400.
  // if(r != nullptr), *r will contain converted Rational
  static bool isRational(float           x, Rational *r);
  static bool isRational(double          x, Rational *r);
  static bool isRational(const Double80 &x, Rational *r);
  // Return true if base^e is rational
  // if(r != nullptr), *r will contain the calculated value
  static bool isRationalPow(const Rational &base, const Rational &e, Rational *r);

  // return NaN if(!isfinite(base) || !isfinite(e))
  // if(e==0) return NaN for base == 0, else 1
  // if(e< 0) return +inf for base == 0
  template<typename T> static T pow(T base, const Rational &e) {
    if(!isfinite(base) || !isfinite(e)) {
      return numeric_limits<T>::quiet_NaN();
    }
    switch(sign(e)) {
    case 0: // e == 0
      return (base == 0)
           ? numeric_limits<T>::quiet_NaN()
           : 1;
    case -1: // e < 0
      if(base == 0) {
        return numeric_limits<T>::infinity();
      }
      // continue case
    default:
      { const INT64 eDen = e.getDenominator();
        if(isEven(eDen) && (base < 0)) {
          return numeric_limits<T>::quiet_NaN();
        }
        const T result = (eDen == 1) ? base : root(base, (T)eDen);
        return (e.getNumerator() == 1) ? result : mypow(result, (T)e.getNumerator());
      }
    }
  }

  inline ULONG hashCode() const {
    return int64Hash(m_num) + 100999 * int64Hash(m_den);
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
Rational randRational(UINT64 maxDenominator = INT64_MAX        , RandomGenerator &rnd = *RandomGenerator::s_stdGenerator);
// Return uniform distributed random rational in range [0;1[ using rnd to generate numerator and denominator
// The denominator (den) will be uniform distributed in the range [2; INT64_MAX]
// The numerator will be uniform distributed in range [0; den[
inline Rational randRational(RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
  return randRational(INT64_MAX, rnd);
}
// Return uniform distributed random rational in range [from;to] (both inclusive)
// Assume the 3 products:
// n1 = from.num*to.den, n2 = to.num*from.den, d = from.den*to.den are all <= _I64_MAX
// if this is not the case, an exception is thrown
// The return rational will have the value rn/(d*f), where rn is a random int in the range [n1*f..n2*f],
// where f is a random int in range [1..min(maxScaleFactor, _I64_MAX/max(n1,n2,d))]
// If maxScaleFactor <= 1, no scaling is done
// To avoid overflow in calculation, keep the involved factors < _I32_MAX
Rational randRational(const Rational &from, const Rational &to, UINT64 maxScaleFactor=INT64_MAX, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator);
// Return uniform distributed random rational in range [from;to] (both inclusive)
// Assume the 3 products: from.num * to.den, to.num * from.den, from.den * to.den are all <= _I64_MAX
// If this is not the case, an exception is thrown
// To avoid overflow in calculation, keep the involved factors < _I32_MAX
inline Rational randRational(const Rational &from, const Rational &to, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
  return randRational(from, to, INT64_MAX, rnd);
}

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
  return (_fpclass(r) & (_FPCLASS_NINF | _FPCLASS_PINF)) != 0;
}
inline bool isnan(const Rational &r) {
  return (_fpclass(r) & (_FPCLASS_SNAN | _FPCLASS_QNAN)) != 0;
}
inline bool isnormal(const Rational &r) {
  return fpclassify(r) == FP_NORMAL;
}
inline bool isunordered(const Rational &x, const Rational &y) {
  return isnan(x) || isnan(y);
}
inline bool isPInfinity(const Rational &r) {
  return _fpclass(r) == _FPCLASS_PINF;
}
inline bool isNInfinity(const Rational &r) {
  return _fpclass(r) == _FPCLASS_NINF;
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

#if defined(_UNICODE)
#define _rattot rattow
#else
#define _rattot rattoa
#endif

String toString(const Rational &r, StreamSize precision=0, StreamSize width=0, FormatFlags flags=0);

Rational strtorat(const char *s   , char    **end, int radix);
Rational wcstorat(const wchar_t *s, wchar_t **end, int radix);

#if defined(_UNICODE)
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
