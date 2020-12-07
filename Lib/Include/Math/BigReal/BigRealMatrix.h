#pragma once

#include <MatrixTemplate.h>
#include <VectorTemplate.h>
#include "BigReal.h"

#define _USEDEFAULTALLOCATORIFNULL() _SELECTPOINTER(digitPool, DEFAULT_DIGITPOOL)
#define _CHECKPRECISION(prec) checkPrecision(prec, __TFUNCTION__)

class BigRealVector : public VectorTemplate<BigReal> {
private:
  UINT m_digits;
  void checkPrecision(UINT digits, const TCHAR *method) const;
protected:
public:
  explicit BigRealVector(DigitPool *digitPool = nullptr);
  explicit BigRealVector(size_t dim, UINT digits = 16, DigitPool *digitPool = nullptr);
  BigRealVector(const BigRealVector &v, DigitPool *digitPool = nullptr);
  BigRealVector(const VectorTemplate<BigReal> &v, UINT digits, DigitPool *digitPool = nullptr);
  BigRealVector &setDimAndPrecision(size_t dim, UINT digits) {
    setDimension(dim); setPrecision(digits);
    return *this;
  }
  UINT setPrecision(UINT digits);
  inline UINT getPrecision() const {
    return m_digits;
  }
  inline DigitPool *getDigitPool() const {
    return (DigitPool*)__super::getVectorAllocator();
  }
  BigRealVector &operator*=(const BigReal &d);
  BigRealVector &operator/=(const BigReal &d);
  BigRealVector &operator+=(const BigRealVector &rhs);
  BigRealVector &operator-=(const BigRealVector &rhs);
  BigReal length(DigitPool *digitPool = nullptr) const;
  friend class BigRealMatrix;
};

BigRealVector prod(const BigRealVector &v , const BigReal       &d,  DigitPool *digitPool = nullptr);
BigRealVector quot(const BigRealVector &v , const BigReal       &d,  DigitPool *digitPool = nullptr);
BigRealVector sum( const BigRealVector &v1, const BigRealVector &v2, DigitPool *digitPool = nullptr);
BigRealVector dif( const BigRealVector &v1, const BigRealVector &v2, DigitPool *digitPool = nullptr);
BigReal       dotp(const BigRealVector &v1, const BigRealVector &v2, DigitPool *digitPool = nullptr);

inline BigRealVector  operator*(const BigReal       &d  , const BigRealVector &rhs) {
  return prod(rhs, d, d.getDigitPool());
}
inline BigRealVector  operator*(const BigRealVector &lts, const BigReal       &d  ) {
  return prod(lts, d, lts.getDigitPool());
}
inline BigRealVector  operator/(const BigRealVector &lts, const BigReal       &d  ) {
  return quot(lts, d, lts.getDigitPool());
}
inline BigRealVector  operator+(const BigRealVector &lts, const BigRealVector &rhs) {
  return sum(lts, rhs, lts.getDigitPool());
}
inline BigRealVector  operator-(const BigRealVector &lts, const BigRealVector &rhs) {
  return dif(lts, rhs, lts.getDigitPool());
}
inline BigReal        operator*(const BigRealVector &lts, const BigRealVector &rhs) {
  return dotp(lts, rhs, lts.getDigitPool());
}

class BigRealMatrix : public MatrixTemplate<BigReal> {
protected:
  UINT m_digits;
  void checkPrecision(UINT digits, const TCHAR *method) const;
public:
  BigRealMatrix(DigitPool *digitPool = nullptr) : MatrixTemplate<BigReal>(_USEDEFAULTALLOCATORIFNULL()), m_digits(16) {
    assert(getDigitPool());
    _CHECKPRECISION(m_digits);
  }
  BigRealMatrix(size_t rows, size_t cols, UINT digits = 16, DigitPool *digitPool = nullptr)
    : MatrixTemplate<BigReal>(rows, cols, _USEDEFAULTALLOCATORIFNULL())
    , m_digits(digits) {
    assert(getDigitPool());
    _CHECKPRECISION(digits);
  }
  explicit BigRealMatrix(const MatrixDimension &dim, UINT digits = 16, DigitPool *digitPool = nullptr)
    : MatrixTemplate<BigReal>(dim, _USEDEFAULTALLOCATORIFNULL())
    , m_digits(digits)
  {
    assert(getDigitPool());
    _CHECKPRECISION(digits);
  }
  BigRealMatrix(const BigRealMatrix &m, DigitPool *digitPool = nullptr)
    : MatrixTemplate<BigReal>(m, _SELECTPOINTER(digitPool, m.getDigitPool()))
    , m_digits(m.getPrecision())
  {
    assert(getDigitPool());
  }

  UINT setPrecision(UINT digits);
  inline UINT getPrecision() const {
    return m_digits;
  }
  inline DigitPool *getDigitPool() const {
    return (DigitPool*)__super::getVectorAllocator();
  }
  static BigRealMatrix _0(size_t rows, size_t columns, UINT digits = 16, DigitPool *digitPool = nullptr);
  static BigRealMatrix _1(size_t dim,                  UINT digits = 16, DigitPool *digitPool = nullptr);
  BigRealVector getRow(     size_t row   , DigitPool *digitPool = nullptr) const;
  BigRealVector getColumn(  size_t column, DigitPool *digitPool = nullptr) const;

  BigRealMatrix &operator+=(const BigRealMatrix &rhs);
  BigRealMatrix &operator-=(const BigRealMatrix &rhs);
  BigRealMatrix &operator*=(const BigReal &d);
  BigRealMatrix &operator/=(const BigReal &d);

};

BigRealMatrix  prod(const BigRealMatrix &lts, const BigReal       &d  , DigitPool *digitPool = nullptr);
BigRealMatrix  quot(const BigRealMatrix &lts, const BigReal       &d  , DigitPool *digitPool = nullptr);
BigRealVector  prod(const BigRealVector &lts, const BigRealMatrix &rhs, DigitPool *digitPool = nullptr);
BigRealVector  prod(const BigRealMatrix &lts, const BigRealVector &rhs, DigitPool *digitPool = nullptr);
BigRealMatrix  sum( const BigRealMatrix &lts, const BigRealMatrix &rhs, DigitPool *digitPool = nullptr);
BigRealMatrix  dif( const BigRealMatrix &lts, const BigRealMatrix &rhs, DigitPool *digitPool = nullptr);
BigRealMatrix  prod(const BigRealMatrix &lts, const BigRealMatrix &rhs, DigitPool *digitPool = nullptr);

inline BigRealMatrix  operator*(const BigReal       &d, const BigRealMatrix &m) {
  return prod(m, d, d.getDigitPool());
}
inline BigRealMatrix  operator*(const BigRealMatrix &m, const BigReal       &d) {
  return prod(m, d, m.getDigitPool());
}
inline BigRealMatrix  operator/(const BigRealMatrix &m, const BigReal       &d) {
  return quot(m, d, m.getDigitPool());
}
inline BigRealVector  operator*(const BigRealVector &v, const BigRealMatrix &m) {
  return prod(v, m, v.getDigitPool());
}
inline BigRealVector  operator*(const BigRealMatrix &m, const BigRealVector &v) {
  return prod(m, v, m.getDigitPool());
}
inline BigRealMatrix  operator+(const BigRealMatrix &a, const BigRealMatrix &b) {
  return sum(a, b, a.getDigitPool());
}
inline BigRealMatrix  operator-(const BigRealMatrix &a, const BigRealMatrix &b) {
  return dif(a, b, a.getDigitPool());
}
inline BigRealMatrix  operator*(const BigRealMatrix &a, const BigRealMatrix &b) {
  return prod(a, b, a.getDigitPool());
}

inline BigRealMatrix &operator*=(BigRealMatrix &lts, const BigRealMatrix &rhs) {
  return lts = lts * rhs;
}

BigRealMatrix  operator-(const BigRealMatrix &m);
BigReal        det(      const BigRealMatrix &m, DigitPool *digitPool = nullptr);
BigRealMatrix  inverse(  const BigRealMatrix &m, DigitPool *digitPool = nullptr);
BigRealMatrix  transpose(const BigRealMatrix &m, DigitPool *digitPool = nullptr);
// Frobenius norm
BigReal        normf(    const BigRealMatrix &m, DigitPool *digitPool = nullptr);

class BigRealLUMatrix : private BigRealMatrix {
private:
  CompactArray<size_t> m_permut;
  int                  m_detsign;

  void initPermut();
  BigRealVector evald() const;
  void pivot(const BigRealVector &d, size_t k);
  void lowerUpper();
  BigRealLUMatrix(const BigRealLUMatrix &a);                 /* not defined */
  BigRealLUMatrix &operator=(const BigRealLUMatrix &a);      /* not defined */
  BigRealLUMatrix &setDimension(size_t rows, size_t cols);   /* not defined */
  BigRealLUMatrix &setDimension(size_t rows);                /* not defined */

public:
  BigRealLUMatrix(DigitPool *digitPool = nullptr);
  BigRealLUMatrix(const BigRealMatrix &a, DigitPool *digitPool = nullptr);
  BigRealLUMatrix &operator=(const BigRealMatrix &a);
  BigRealMatrix getInverse(DigitPool *digitPool = nullptr) const;
  BigRealVector solve(const BigRealVector &y, DigitPool *digitPool = nullptr) const;
  BigReal getDeterminant(DigitPool *digitPool = nullptr) const;

  friend tostream &operator<<(tostream &out, const BigRealLUMatrix &a);
};
