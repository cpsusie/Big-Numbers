#pragma once

#include <MyString.h>
#include <Math/Real.h>
#include <Math/Rational.h>

typedef enum {
  NUMBERTYPE_UNDEFINED
 ,NUMBERTYPE_REAL
 ,NUMBERTYPE_RATIONAL
} NumberType;

class Number {
private:
  DECLARECLASSNAME;

  NumberType   m_type;
  union {
    Real      *m_real;
    Rational  *m_rational;
  };

  void setType(NumberType type);
  void forceTypeToReal();
  void cleanup();
  static void throwTypeIsUndefinedException(const TCHAR *method);
  static void throwUnknownTypeException(    const TCHAR *method, NumberType type);
         void throwUnknownTypeException(    const TCHAR *method) const;
public:
  Number();
  Number(const Number   &v);
  Number(const Real     &v);
  Number(const Rational &v);
  Number(int             v);
  Number(unsigned int    v);

  virtual ~Number();

  Number &operator=(const Number   &v);
  Number &operator=(const Real     &v);
  Number &operator=(const Rational &v);
  Number &operator=(int             v);
  Number &operator=(unsigned int    v);

  inline NumberType getType() const {
    return m_type;
  }
  inline bool isUndefined() const {
    return m_type == NUMBERTYPE_UNDEFINED;
  }

  inline bool isInteger() const {
    return isRational() && m_rational->isInteger();
  }
  inline bool isRational() const {
    return getType() == NUMBERTYPE_RATIONAL;
  }

  inline bool isEven()     const {
    return isRational() && m_rational->isEven();
  }

  inline bool isOdd()      const {
    return isRational() && m_rational->isOdd();
  }

  int      getIntValue()      const;
  Real     getRealValue()     const;
  Rational getRationalValue() const;

  friend Number operator+( const Number &n1, const Number &n2);
  friend Number operator-( const Number &n1, const Number &n2);
  friend Number operator-( const Number &v);
  friend Number operator*( const Number &n1, const Number &n2);
  friend Number operator/( const Number &n1, const Number &n2);
  friend Number operator%( const Number &n1, const Number &n2);
  friend Number reciprocal(const Number &n);
  friend Number pow(       const Number &n1, const Number &n2);

  friend int numberCmp(const Number &n1, const Number &n2);

  inline bool operator<( const Number &n) const {
    return numberCmp(*this, n) < 0;
  }
  inline bool operator>( const Number &n) const {
    return numberCmp(*this, n) > 0;
  }
  inline bool operator<=(const Number &n) const {
    return numberCmp(*this, n) <= 0;
  }

  inline bool operator>=(const Number &n) const {
    return numberCmp(*this, n) >= 0;
  }

  bool operator==(const Number &n) const;
  inline bool operator!=(const Number &n) const {
    return !(*this == n);
  }

  const Real *getRealAddress() const; // will convert this to a Real by calling forceTypeToReal(), if its a Rational

  String toString() const;
};

template<class T> bool isOdd(T x) {
  return x & 1;
}

template<class T> bool isEven(T x) {
  return (x & 1) == 0;
}

inline bool isSymmetricExponent(const Rational &r) {
  return ::isEven(r.getNumerator()) || ::isEven(r.getDenominator());
}

inline bool isAsymmetricExponent(const Rational &r) {
  return isOdd(r.getNumerator()) && isOdd(r.getDenominator());
}
