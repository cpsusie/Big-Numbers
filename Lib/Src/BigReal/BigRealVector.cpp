#include "pch.h"
#include <Math/BigRealMatrix.h>

// Implementation of basic BigRealVector-operations

void BigRealVector::checkPrecision(unsigned int digits) {
  if(digits == 0) {
    throwBigRealException(_T("BigRealVector:Precision = 0 not allowed."));
  }
}

void BigRealVector::init(unsigned int dim, bool initialize, int digits) {
  VectorTemplate<BigReal>::init(dim,initialize);
  checkPrecision(digits);
  m_digits = digits;
}

BigRealVector::BigRealVector(unsigned int dim, unsigned int digits) {
  init(dim,true,digits);
}

BigRealVector::BigRealVector(const BigRealVector &v) {
  init(v.getDimension(),false,v.m_digits);
  for(unsigned int i = 0; i < getDimension(); i++) {
    (*this)[i] = v[i];
  }
}

BigRealVector::BigRealVector(const VectorTemplate<BigReal> &v, unsigned int digits) {
  init(v.getDimension(),false,digits);
  for(unsigned int i = 0; i < getDimension(); i++) {
    (*this)[i] = v[i];
  }
}

unsigned int BigRealVector::setPrecision(unsigned int digits) {
  checkPrecision(digits);
  unsigned int oldDigits = m_digits;
  m_digits = digits;
  return oldDigits;
}

BigRealVector operator*(const BigReal &d, const BigRealVector &rhs) {
  const int n = rhs.getDimension();
  const int digits = rhs.getPrecision();

  BigRealVector result(n,digits);
  for(int i = 0; i < n; i++) {
    result[i] = rProd(rhs[i],d,digits);
  }
  return result;
}

BigRealVector operator*(const BigRealVector &lts, const BigReal &d ) {
  return d * lts;
}

BigRealVector operator/(const BigRealVector &lts, const BigReal &d) {
  const int n      = lts.getDimension();
  const int digits = lts.getPrecision();

  BigRealVector result(n,digits);
  for(int i = 0; i < n; i++) {
    result[i] = rQuot(lts[i],d,digits);
  }
  return result;
}

BigReal operator*(const BigRealVector &lts, const BigRealVector& rhs) {
  const int n      = lts.getDimension();
  const int digits = min(lts.getPrecision(),rhs.getPrecision());

  if((unsigned int)n != rhs.getDimension()) {
    throwBigRealException(_T("operator*(BigRealVector,BigRealVector):Invalid dimension. left.dimension=%u, right.dimension=%u."), n, rhs.getDimension());
  }

  BigReal sum = 0;
  for(int i = 0; i < n; i++) {
    sum = rSum(sum,rProd(lts[i],rhs[i],digits),digits);
  }
  return sum;
}

BigRealVector operator+(const BigRealVector& lts, const BigRealVector& rhs) {
  const int n      = lts.getDimension();
  const int digits = min(lts.getPrecision(),rhs.getPrecision());

  if((unsigned int)n != rhs.getDimension()) {
    throwBigRealException(_T("operator+(BigRealVector,BigRealVector):Invalid dimension. left.dimension=%u, right.dimension=%u."), n, rhs.getDimension());
  }

  BigRealVector result(n,digits);
  for(int i = 0; i < n; i++) {
    result[i] = rSum(lts[i],rhs[i],digits);
  }
  return result;
}

BigRealVector operator-(const BigRealVector& lts, const BigRealVector& rhs) {
  const int n      = lts.getDimension();
  const int digits = min(lts.getPrecision(),rhs.getPrecision());

  if((unsigned int)n != rhs.getDimension()) {
    throwBigRealException(_T("operator-(BigRealVector,BigRealVector):Invalid dimension. left.dimension=%u, right.dimension=%u."), n, rhs.getDimension());
  }

  BigRealVector result(n,digits);
  for(int i = 0; i < n; i++) {
    result[i] = rDif(lts[i],rhs[i],digits);
  }
  return result;
}

BigRealVector &BigRealVector::operator*=(const BigReal &d) {
  const int n = getDimension();
  for(int i = 0; i < n; i++) {
    BigReal &v = (*this)[i];
    v = rProd(v,d,m_digits);
  }
  return *this;
}

BigRealVector &BigRealVector::operator/=(const BigReal &d) {
  const int n = getDimension();
  for(int i = 0; i < n; i++) {
    BigReal &v = (*this)[i];
    v = rQuot(v,d,m_digits);
  }
  return *this;
}

BigRealVector &BigRealVector::operator+=(const BigRealVector &rhs) {
  const int n      = getDimension();
  const int digits = min(m_digits,rhs.getPrecision());
  if((unsigned int)n != rhs.getDimension()) {
    throwBigRealException(_T("operator+=(BigRealVector):Invalid dimension. Dimension=%u, right.dimension=%u."), n, rhs.getDimension());
  }

  for(int i = 0; i < n; i++) {
    BigReal &v = (*this)[i];
    v = rSum(v,rhs[i],digits);
  }
  setPrecision(digits);
  return *this;
}

BigRealVector &BigRealVector::operator-=(const BigRealVector &rhs) {
  const int n = getDimension();
  const int digits = min(m_digits,rhs.getPrecision());
  if((unsigned int)n != rhs.getDimension()) {
    throwBigRealException(_T("operator-=(BigRealVector):Invalid dimension. Dimension=%u, right.dimension=%u."), n, rhs.getDimension());
  }

  for(int i = 0; i < n; i++) {
    BigReal &v = (*this)[i];
    v = rDif(v,rhs[i],digits);
  }
  setPrecision(digits);
  return *this;
}

BigReal BigRealVector::length() const {
  const int n = getDimension();
  BigReal sum = 0;
  for(int i = 0; i < n; i++) {
    const BigReal &v = (*this)[i];
    sum = rSum(sum,rProd(v,v,m_digits),m_digits);
  }
  return rSqrt(sum,m_digits);
}
