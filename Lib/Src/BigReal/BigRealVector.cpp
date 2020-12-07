#include "pch.h"
#include <Math/BigReal/BigRealMatrix.h>

// Implementation of basic BigRealVector-operations

void BigRealVector::checkPrecision(UINT digits, const TCHAR *method) const {
  if(digits == 0) {
    throwBigRealException(_T("%s:Precision = 0 not allowed"), method);
  }
}

BigRealVector::BigRealVector(DigitPool *digitPool)
  : VectorTemplate<BigReal>(_USEDEFAULTALLOCATORIFNULL())
  , m_digits(16)
{
  assert(getDigitPool());
}

BigRealVector::BigRealVector(size_t dim, UINT digits, DigitPool *digitPool)
  : VectorTemplate<BigReal>(dim, _USEDEFAULTALLOCATORIFNULL())
  , m_digits(digits)
{
  assert(getDigitPool());
  _CHECKPRECISION(digits);
}

BigRealVector::BigRealVector(const BigRealVector &v, DigitPool *digitPool)
  : VectorTemplate<BigReal>(v, _SELECTPOINTER(digitPool, v.getDigitPool()))
  , m_digits(v.getPrecision())
{
  assert(getDigitPool());
}

BigRealVector::BigRealVector(const VectorTemplate<BigReal> &v, UINT digits, DigitPool *digitPool)
  : VectorTemplate<BigReal>(v, _USEDEFAULTALLOCATORIFNULL())
  , m_digits(digits)
{
  assert(getDigitPool());
  _CHECKPRECISION(digits);
}

UINT BigRealVector::setPrecision(UINT digits) {
  _CHECKPRECISION(digits);
  const UINT oldDigits = m_digits;
  m_digits = digits;
  return oldDigits;
}

BigRealVector prod(const BigRealVector &v, const BigReal &d, DigitPool *digitPool) {
  _SELECTDIGITPOOL(v);
  const size_t n      = v.getDimension();
  const UINT   digits = v.getPrecision();

  BigRealVector result(n, digits, pool);
  for(size_t i = 0; i < n; i++) {
    result[i] = rProd(v[i], d, digits, pool);
  }
  return result;
}

BigRealVector quot(const BigRealVector &v, const BigReal &d, DigitPool *digitPool) {
  _SELECTDIGITPOOL(v);
  const size_t n      = v.getDimension();
  const UINT   digits = v.getPrecision();

  BigRealVector result(n, digits, pool);
  for(size_t i = 0; i < n; i++) {
    result[i] = rQuot(v[i], d, digits, pool);
  }
  return result;
}

BigRealVector sum(const BigRealVector &v1, const BigRealVector &v2, DigitPool *digitPool) {
  v1.checkSameDimension(__TFUNCSIG__, v2);
  _SELECTDIGITPOOL(v1);
  const size_t n      = v1.getDimension();
  const UINT   digits = min(v1.getPrecision(), v2.getPrecision());

  BigRealVector result(n, digits, pool);
  for(size_t i = 0; i < n; i++) {
    result[i] = rSum(v1[i], v2[i], digits);
  }
  return result;

}
BigRealVector dif(const BigRealVector &v1, const BigRealVector &v2, DigitPool *digitPool) {
  v1.checkSameDimension(__TFUNCSIG__, v2);
  _SELECTDIGITPOOL(v1);
  const size_t n      = v1.getDimension();
  const UINT   digits = min(v1.getPrecision(), v2.getPrecision());

  BigRealVector result(n, digits, pool);
  for(size_t i = 0; i < n; i++) {
    result[i] = rDif(v1[i], v2[i], digits);
  }
  return result;

}
BigReal       dotp(const BigRealVector &v1, const BigRealVector &v2, DigitPool *digitPool) {
  v1.checkSameDimension(__TFUNCSIG__, v2);
  _SELECTDIGITPOOL(v1);
  const size_t n      = v1.getDimension();
  const UINT   digits = min(v1.getPrecision(), v2.getPrecision());

  BigReal sum(0, pool);
  for(size_t i = 0; i < n; i++) {
    sum = rSum(sum, rProd(v1[i], v2[i], digits, pool), digits, pool);
  }
  return sum;

}

BigRealVector &BigRealVector::operator*=(const BigReal &d) {
  const size_t n = getDimension();
  for(size_t i = 0; i < n; i++) {
    BigReal &v = (*this)[i];
    v = rProd(v, d, m_digits);
  }
  return *this;
}

BigRealVector &BigRealVector::operator/=(const BigReal &d) {
  const size_t n = getDimension();
  for(size_t i = 0; i < n; i++) {
    BigReal &v = (*this)[i];
    v = rQuot(v, d, m_digits);
  }
  return *this;
}

BigRealVector &BigRealVector::operator+=(const BigRealVector &rhs) {
  checkSameDimension(__TFUNCSIG__, rhs);
  const size_t n      = getDimension();
  const UINT   digits = min(m_digits, rhs.getPrecision());

  for(size_t i = 0; i < n; i++) {
    BigReal &v = (*this)[i];
    v = rSum(v, rhs[i], digits);
  }
  setPrecision(digits);
  return *this;
}

BigRealVector &BigRealVector::operator-=(const BigRealVector &rhs) {
  checkSameDimension(__TFUNCSIG__, rhs);
  const size_t n      = getDimension();
  const UINT   digits = min(m_digits, rhs.getPrecision());
  for(size_t i = 0; i < n; i++) {
    BigReal &v = (*this)[i];
    v = rDif(v, rhs[i], digits);
  }
  setPrecision(digits);
  return *this;
}

BigReal BigRealVector::length(DigitPool *digitPool) const {
  _SELECTDIGITPOOL(*this);
  const size_t n    = getDimension();
  BigReal      sum(0, pool);
  for(size_t i = 0; i < n; i++) {
    const BigReal &v = (*this)[i];
    sum = rSum(sum, rProd(v, v, m_digits, pool), m_digits, pool);
  }
  return rSqrt(sum, m_digits, pool);
}
