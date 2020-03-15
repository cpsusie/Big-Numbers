#pragma once

#include <MyString.h>
#include "Rational.h"

class Double80;

typedef enum {
  NUMBERTYPE_FLOAT
 ,NUMBERTYPE_DOUBLE
 ,NUMBERTYPE_DOUBLE80
 ,NUMBERTYPE_RATIONAL
} NumberType;

#ifdef LONGDOUBLE
#define NUMBERTYPE_REAL NUMBERTYPE_DOUBLE80
#else
#define NUMBERTYPE_REAL NUMBERTYPE_DOUBLE
#endif

class Number {
private:
  NumberType   m_type;
  union {
    float     *m_flt;
    double    *m_d64;
    Double80  *m_d80;
    Rational  *m_rational;
    void      *m_datap;
  };

  // Must be private
  void setType(NumberType type, bool init = false);
  inline void initType(NumberType type) {
    setType(type, true);
  }
  // fpclass must be one of:
  // _FPCLASS_SNAN  0x0001   signaling NaN
  // _FPCLASS_QNAN  0x0002   quiet NaN
  // _FPCLASS_NINF  0x0004   negative infinity
  // _FPCLASS_PINF  0x0200   positive infinity
  void setToNaN(int fpclass = _FPCLASS_QNAN, bool init = false);
  inline void initToNaN(int fpclass = _FPCLASS_QNAN) {
    setToNaN(fpclass, true);
  }
  void cleanup();

  template<typename T> bool handleInfAndRationalValue(T v, bool init = false) {
    if(!::isfinite(v)) {
      setToNaN(_fpclass(v), init);
      return true;
    } else {
      Rational tmp;
      if(Rational::isRational(v, &tmp)) {
        setType(NUMBERTYPE_RATIONAL, init);
        *m_rational = tmp;
        return true;
      }
    }
    return false;
  }

public:
  inline Number() {
    initToNaN();
  }
  Number(const Number   &v);
  Number(int             v);
  Number(UINT            v);
  Number(const float    &v);
  Number(const double   &v);
  Number(const Double80 &v);
  Number(const Rational &v);

  virtual ~Number() {
    cleanup();
  }

  Number &operator=(const Number   &v);
  Number &operator=(int             v);
  Number &operator=(UINT            v);
  Number &operator=(const float    &v);
  Number &operator=(const double   &v);
  Number &operator=(const Double80 &v);
  Number &operator=(const Rational &v);

  inline NumberType getType() const {
    return m_type;
  }
  static String getTypeName(NumberType nt);
  String getTypeName() const {
    return getTypeName(getType());
  }

  inline bool isInteger() const {
    return isRational() && m_rational->isInteger();
  }
  inline bool isRational() const {
    return getType() == NUMBERTYPE_RATIONAL;
  }

  friend inline bool isInteger(const Number &n) {
    return n.isRational() && n.m_rational->isInteger();
  }
  friend inline bool isRational(const Number &n) {
    return n.getType() == NUMBERTYPE_RATIONAL;
  }
  friend inline bool isInt(const Number &n) {
    return n.isRational() && isInt(*n.m_rational);
  }
  friend inline bool isEven(const Number &n) {
    return n.isRational() && isEven(*n.m_rational);
  }
  friend inline bool isOdd(const Number &n) {
    return n.isRational() && isOdd(*n.m_rational);
  }
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
  friend int _fpclass(const Number &n);

  inline bool isPositive() const {
    return (_fpclass(*this) & (_FPCLASS_PN | _FPCLASS_PD | _FPCLASS_PINF)) != 0;
  }
  inline bool isNegative() const {
    return (_fpclass(*this) & (_FPCLASS_NN | _FPCLASS_ND | _FPCLASS_NINF)) != 0;
  }
  inline bool isZero() const {
    return (_fpclass(*this) & (_FPCLASS_PZ | _FPCLASS_NZ)) != 0;
  }
  friend int      getInt(     const Number &n);
  friend float    getFloat(   const Number &n);
  friend double   getDouble(  const Number &n);
  friend Double80 getDouble80(const Number &n);
  friend Rational getRational(const Number &n);
  friend Number operator+( const Number &n1, const Number &n2);
  friend Number operator-( const Number &n1, const Number &n2);
  friend Number operator-( const Number &v);
  friend Number operator*( const Number &n1, const Number &n2);
  friend Number operator/( const Number &n1, const Number &n2);
  friend Number operator%( const Number &n1, const Number &n2);
  friend Number reciprocal(const Number &n);
  friend Number pow(       const Number &n1, const Number &n2);

  friend int numberCmp(const Number &n1, const Number &n2);

  static void throwUnknownTypeException(const TCHAR *method, NumberType type);
  void        throwUnknownTypeException(const TCHAR *method) const;
};

inline Real getReal( const Number &n) {
#ifdef LONGDOUBLE
  return getDouble80(n);
#else
  return getDouble(n);
#endif
}

inline Number fabs(const Number &n) {
  return n.isNegative() ? -n : n;
}

// returns one of
// FP_INFINITE
// FP_NAN
// FP_NORMAL
// FP_SUBNORMAL
// FP_ZERO
int fpclassify(const Number &n);

inline bool isfinite(const Number &n) {
  return fpclassify(n) <= 0;
}
inline bool isinf(const Number &n) {
  return fpclassify(n) == FP_INFINITE;
}
inline bool isnan(const Number &n) {
  return fpclassify(n) == FP_NAN;
}
inline bool isnormal(const Number &n) {
  return fpclassify(n) == FP_NORMAL;
}
inline bool isunordered(const Number &x, const Number &y) {
  return isnan(x) || isnan(y);
}
inline bool isPInfinity(const Number &n) {
  return isinf(n) && n.isPositive();
}
inline bool isNInfinity(const Number &n) {
  return isinf(n) && n.isNegative();
}

inline bool operator< (const Number &x, const Number &y) {
  return !isunordered(x, y) && (numberCmp(x, y) < 0);
}
inline bool operator> (const Number &x, const Number &y) {
  return !isunordered(x, y) && (numberCmp(x, y) > 0);
}
inline bool operator<=(const Number &x, const Number &y) {
  return !isunordered(x, y) && (numberCmp(x, y) <= 0);
}
inline bool operator>=(const Number &x, const Number &y) {
  return !isunordered(x, y) && (numberCmp(x, y) >= 0);
}
inline bool operator==(const Number &x, const Number &y) {
  return !isunordered(x, y) && (numberCmp(x, y) == 0);
}
inline bool operator!=(const Number &x, const Number &y) {
  return !isunordered(x, y) && (numberCmp(x, y) != 0);
}

char    *numtoa(char    *dst, const Number &n);
wchar_t *numtow(wchar_t *dst, const Number &n);

#ifdef _UNICODE
#define numtot numtow
#else
#define numtot numtoa
#endif

String toString(const Number &n, StreamSize precision = 6, StreamSize width = 0, FormatFlags flags = 0);

Number _strtonum_l(const char    *s, char    **end, _locale_t locale);
Number _wcstonum_l(const wchar_t *s, wchar_t **end, _locale_t locale);

inline Number strtonum(const char    *s, char    **end) {
  return _strtonum_l(s, end, _get_current_locale());
}
inline Number wcstonum(const wchar_t *s, wchar_t **end) {
  return _wcstonum_l(s, end, _get_current_locale());
}

#ifdef _UNICODE
#define _tcstonum_l _wcstonum_l
#define _tcstonum    wcstonum
#else
#define _tcstonum_l _strtonum_l
#define _tcstonum    strtonum
#endif // _UNICODE

// input/output always in decimal. if hexfloat, use scientific
std::istream  &operator>>(std::istream  &in,        Number &n);
std::ostream  &operator<<(std::ostream  &out, const Number &n);

std::wistream &operator>>(std::wistream &in,        Number &n);
std::wostream &operator<<(std::wostream &out, const Number &n);
