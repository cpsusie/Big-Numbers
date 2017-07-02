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

BigRealLUMatrix::BigRealLUMatrix() {
  initPermut();
}

BigRealLUMatrix::BigRealLUMatrix(const BigRealMatrix &src) : BigRealMatrix(src) {
  if(!src.isSquare()) {
    throwBigRealException(_T("%s:Matrix not square. %s"), __TFUNCTION__, src.getDimensionString().cstr());
  }
  initPermut();
  lowerUpper();
}

BigRealLUMatrix& BigRealLUMatrix::operator=(const BigRealMatrix& src) { // assignment operator
  if(this == &src) {
    return *this;
  }
  if(!src.isSquare()) {
    throwBigRealException(_T("%s:Matrix not square. %s"), __TFUNCTION__, src.getDimensionString().cstr());
  }

  BigRealMatrix::operator=(src);
  initPermut();
  lowerUpper();
  return *this;
}

/* --------------- crout algorithm --------------- */
/*  Implements Crout LU-decomposition as described in
    "A first course in numerical analysis", Ralston & Rabinowitz 2. ed. p. 421
*/

BigRealVector BigRealLUMatrix::evald() const {
  const BigRealLUMatrix &a = *this;
  const size_t           n = getRowCount();
  BigRealVector          d(n);
  for(size_t i = 0; i < n; i++) {
    BigReal max,tmp;
    for(size_t j = 0; j < n; j++) {
      if((tmp = fabs(a(i,j))) > max) {
        max = tmp;
      }
    }
    d[i] = max;
  }
  return d;
}

void BigRealLUMatrix::pivot(const BigRealVector &d, size_t k) {
  BigRealLUMatrix &a      = *this;
  const size_t     n      = getRowCount();
  const UINT       digits = getPrecision();
  BigReal          max    = 0;
  size_t           current;

  for(size_t i = k; i < n; i++) {
    BigReal tmp(rQuot(a(m_permut[i],k), d[m_permut[i]], digits));
    if(tmp.isNegative()) tmp.changeSign();
    if(tmp > max) {
      max     = tmp;
      current = i;
    }
  }
  if(max.isZero()) {
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
  const intptr_t   n      = getRowCount();
  const UINT       digits = getPrecision();
  BigRealVector    d      = evald();

  initPermut();
  for(intptr_t k = 0; k < n; k++) {
    for(intptr_t i = k; i < n; i++) {
      BigReal sum;
      for(intptr_t l = 0; l <= k-1; l++) {
        sum = rSum(sum, rProd(a(m_permut[i],l), a(m_permut[l],k), digits), digits);
      }
      a(m_permut[i],k) = rDif(a(m_permut[i],k), sum, digits);
    }
    pivot(d, k);
    for(intptr_t j = k+1; j < n; j++) {
      BigReal sum;
      for(intptr_t l = 0; l <= k-1; l++) {
        sum = rSum(sum, rProd(a(m_permut[k],l), a(m_permut[l],j), digits), digits);
      }
      a(m_permut[k], j) = rQuot(rDif(a(m_permut[k],j), sum, digits), a(m_permut[k],k), digits);
    }
  }
}

// Computes x so that A*x=y. Assumes A has been LU decomposed with lowerupper
BigRealVector BigRealLUMatrix::solve(const BigRealVector &y) const {
  const BigRealLUMatrix &a      = *this;
  const intptr_t         n      = getRowCount();
  const UINT             digits = getPrecision();

  if(y.getDimension() != n) {
    throwBigRealException(_T("%s:Invalid dimension. y.%s, LU.%s"), __TFUNCTION__, y.getDimensionString().cstr(), getDimensionString().cstr());
  }

  BigRealVector z(n);
  for(intptr_t i = 0; i < n; i++) {
    BigReal sum = 0;
    for(intptr_t j = 0; j <= i-1; j++) {
      sum = rSum(sum, rProd(a(m_permut[i],j), z[j], digits), digits);
    }
    z[i] = rQuot(rDif(y[m_permut[i]], sum, digits), a(m_permut[i],i), digits);
  }
  BigRealVector x(n);
  for(intptr_t i = n-1; i >= 0; i--) {
    BigReal sum = 0;
    for(intptr_t j = i+1; j < n; j++) {
      sum = rSum(sum, rProd(a(m_permut[i],j), x[j], digits), digits);
    }
    x[i] = rDif(z[i], sum, digits);
  }
  return x;
}

BigRealMatrix BigRealLUMatrix::getInverse() const {
  const size_t  n = getRowCount();
  BigRealMatrix result(n, n, getPrecision());
  BigRealVector e(n);
  for(size_t i = 0; i < n; i++) {
    e[i] = BIGREAL_1;
    result.setColumn(i, solve(e));
    e[i] = BIGREAL_0;
  }
  return result;
}

BigReal BigRealLUMatrix::getDeterminant() const {
  const BigRealLUMatrix &a      = *this;
  BigReal                d      = m_detsign;
  const size_t           n      = getRowCount();
  const UINT             digits = getPrecision();

  for(size_t i = 0; i < n; i++) {
    d = rProd(d, a(a.m_permut[i], i), digits);
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
