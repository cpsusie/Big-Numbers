#include "pch.h"
#include <Math/BigRealMatrix.h>

// Implementation of BigRealLUMatrix

void BigRealLUMatrix::initPermut() {
  const size_t n = getRowCount();
  m_permut.clear(n);
  for(size_t r = 0; r < n; r++) {
    m_permut.add(r);
  }
  m_detsign = 1;
}

BigRealLUMatrix::BigRealLUMatrix(DigitPool *digitPool) : BigRealMatrix(digitPool) {
  initPermut();
}

BigRealLUMatrix::BigRealLUMatrix(const BigRealMatrix &src, DigitPool *digitPool) : BigRealMatrix(src, digitPool) {
  _VALIDATEISSQUAREMATRIX(src);
  initPermut();
  lowerUpper();
}

BigRealLUMatrix& BigRealLUMatrix::operator=(const BigRealMatrix& src) { // assignment operator
  if(this == &src) {
    return *this;
  }
  _VALIDATEISSQUAREMATRIX(src);
  __super::operator=(src);
  initPermut();
  lowerUpper();
  return *this;
}

/* --------------- crout algorithm --------------- */
/*  Implements Crout LU-decomposition as described in
    "A first course in numerical analysis", Ralston & Rabinowitz 2. ed. p. 421
*/

BigRealVector BigRealLUMatrix::evald() const {
  DigitPool             *pool = getDigitPool();
  const BigRealLUMatrix &a    = *this;
  const size_t           n    = getRowCount();
  BigRealVector          d(n, getPrecision(), pool);
  for(size_t i = 0; i < n; i++) {
    const BigReal *rmax = &a(i, 0);
    for(size_t j = 1; j < n; j++) {
      if(BigReal::compareAbs(a(i,j), *rmax) > 0) {
        rmax = &a(i,j);
      }
    }
    d[i] = *rmax;
  }
  return d;
}

void BigRealLUMatrix::pivot(const BigRealVector &d, size_t k) {
  DigitPool       *pool   = getDigitPool();
  BigRealLUMatrix &a      = *this;
  const size_t     n      = getRowCount();
  const UINT       digits = getPrecision();
  BigReal          pmax(0, pool);
  size_t           current;

  for(size_t i = k; i < n; i++) {
    const BigReal tmp(rQuot(a(m_permut[i],k), d[m_permut[i]], digits));
    if(BigReal::compareAbs(tmp, pmax) > 0) {
      pmax    = tmp;
      current = i;
    }
  }
  if(pmax.isZero()) {
    throwBigRealException(_T("The BigRealMatrix is singular"));
  }

  if(current != k) {
    size_t itmp       = m_permut[current];
    m_permut[current] = m_permut[k];
    m_permut[k]       = itmp;
    m_detsign         = -m_detsign; // remember permutations % 2.
                                    // final determinant = prod(diag) * m_detSign
  }
}

void BigRealLUMatrix::lowerUpper() {
  BigRealLUMatrix &a      = *this;
  DigitPool       *pool   = getDigitPool();
  const intptr_t   n      = getRowCount();
  const UINT       digits = getPrecision();
  BigRealVector    d      = evald();

  initPermut();
  BigReal sum(pool);
  for(intptr_t k = 0; k < n; k++) {
    for(intptr_t i = k; i < n; i++) {
      sum.setToZero();
      for(intptr_t l = 0; l <= k-1; l++) {
        sum = rSum(sum, rProd(a(m_permut[i],l), a(m_permut[l],k), digits), digits);
      }
      a(m_permut[i],k) = rDif(a(m_permut[i],k), sum, digits);
    }
    pivot(d, k);
    for(intptr_t j = k+1; j < n; j++) {
      sum.setToZero();
      for(intptr_t l = 0; l <= k-1; l++) {
        sum = rSum(sum, rProd(a(m_permut[k],l), a(m_permut[l],j), digits), digits);
      }
      a(m_permut[k], j) = rQuot(rDif(a(m_permut[k],j), sum, digits), a(m_permut[k],k), digits);
    }
  }
}

// Computes x so that A*x=y. Assumes A has been LU decomposed with lowerupper
BigRealVector BigRealLUMatrix::solve(const BigRealVector &y, DigitPool *digitPool) const {
  const BigRealLUMatrix &a      = *this;
  const intptr_t         n      = getRowCount();
  const UINT             digits = __min(getPrecision(), y.getPrecision());

  if(y.getDimension() != n) {
    throwBigRealException(_T("%s:Invalid dimension. y.%s, LU.%s"), __TFUNCSIG__, y.getDimensionString().cstr(), getDimensionString().cstr());
  }
  _SELECTDIGITPOOL(a);
  BigRealVector z(n, digits, pool);
  BigReal sum(0, pool);
  for(intptr_t i = 0; i < n; i++) {
    sum.setToZero();
    for(intptr_t j = 0; j <= i-1; j++) {
      sum = rSum(sum, rProd(a(m_permut[i],j), z[j], digits, pool), digits);
    }
    z[i] = rQuot(rDif(y[m_permut[i]], sum, digits), a(m_permut[i],i), digits);
  }
  BigRealVector x(n, digits, pool);
  for(intptr_t i = n-1; i >= 0; i--) {
    sum.setToZero();
    for(intptr_t j = i+1; j < n; j++) {
      sum = rSum(sum, rProd(a(m_permut[i],j), x[j], digits, pool), digits);
    }
    x[i] = rDif(z[i], sum, digits, pool);
  }
  return x;
}

BigRealMatrix BigRealLUMatrix::getInverse(DigitPool *digitPool) const {
  _SELECTDIGITPOOL(*this);
  const size_t  n      = getRowCount();
  const UINT    digits = getPrecision();
  BigRealMatrix result(getDimension(), digits, pool);
  BigRealVector e(n, digits, pool);
  for(size_t i = 0; i < n; i++) {
    e[i] = BigReal::_1;
    result.setColumn(i, solve(e, pool));
    e[i] = BigReal::_0;
  }
  return result;
}

BigReal BigRealLUMatrix::getDeterminant(DigitPool *digitPool) const {
  _SELECTDIGITPOOL(*this);
  const BigRealLUMatrix &a      = *this;
  BigReal                d(m_detsign, pool);
  const size_t           n      = getRowCount();
  const UINT             digits = getPrecision();

  for(size_t i = 0; i < n; i++) {
    d = rProd(d, a(a.m_permut[i], i), digits, pool);
  }
  return d;
}

tostream& operator<<(tostream& out, const BigRealLUMatrix &a) {
  out << BigRealMatrix(a);
  out << _T("Permut:(");
  for(size_t r = 0; r < a.getRowCount(); r++) {
    out << a.m_permut[r] << _T(" ");
  }
  out << _T("). determinant sign:") << a.m_detsign << _T('\n');
  return out;
}
