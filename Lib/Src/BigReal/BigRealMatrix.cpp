#include "pch.h"
#include <Math/BigReal/BigRealMatrix.h>

// Implementation of basic matrix-operations

DEFINECLASSNAME(BigRealMatrix);

void BigRealMatrix::checkPrecision(UINT digits) {
  if(digits == 0) {
    throwBigRealException(_T("%s:Precision = 0 not allowed"), s_className);
  }
}

UINT BigRealMatrix::setPrecision(UINT digits) {
  checkPrecision(digits);
  const UINT oldDigits = m_digits;
  m_digits = digits;
  return oldDigits;
}

BigRealMatrix BigRealMatrix::_0(size_t rows, size_t columns, UINT digits, DigitPool *digitPool) { // static
  return BigRealMatrix(rows, columns, digits, digitPool);
}

BigRealMatrix BigRealMatrix::_1(size_t dim, UINT digits, DigitPool *digitPool) { // static
  BigRealMatrix result(dim,dim,digits, digitPool);
  for(size_t i = 0; i < dim; i++) {
    result(i,i) = BigReal::_1;
  }
  return result;
}

BigRealVector BigRealMatrix::getRow(size_t row, DigitPool *digitPool) const {
  _SELECTDIGITPOOL(*this);
  if(pool == getDigitPool()) {
    return BigRealVector(__super::getRow(row), getPrecision(), pool);
  } else {
    checkRowIndex(__TFUNCSIG__, row);
    const size_t cn = getColumnCount();
    BigRealVector result(cn, getPrecision(), pool);
    for(size_t c = 0; c < cn; c++) {
      result[c] = (*this)(row, c);
    }
    return result;
  }
}

BigRealVector BigRealMatrix::getColumn(size_t column, DigitPool *digitPool) const {
  _SELECTDIGITPOOL(*this);
  if (pool == getDigitPool()) {
    return BigRealVector(__super::getColumn(column), getPrecision());
  } else {
    checkColumnIndex(__TFUNCSIG__, column);
    const size_t rn = getRowCount();
    BigRealVector result(rn, getPrecision(), pool);
    for(size_t r = 0; r < rn; r++) {
      result[r] = (*this)(r, column);
    }
    return result;
  }
}

// Frobenius norm
BigReal normf(const BigRealMatrix &a, DigitPool *digitPool) {
  _SELECTDIGITPOOL(a);
  const UINT digits = a.getPrecision();
  BigReal    sum(0, pool);
  for(size_t r = 0; r < a.getRowCount(); r++) {
    for(size_t c = 0; c < a.getColumnCount(); c++) {
      sum += rProd(a(r,c), a(r,c), digits, pool);
    }
  }
  return rSqrt(sum,digits);
}

BigRealMatrix  prod(const BigRealMatrix &lts, const BigReal       &d, DigitPool *digitPool) {
  _SELECTDIGITPOOL(lts);
  const size_t rows   = lts.getRowCount();
  const size_t cols   = lts.getColumnCount();
  const UINT   digits = lts.getPrecision();

  BigRealMatrix result(rows, cols, digits, pool);
  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      result(r, c) = rProd(lts(r, c), d, digits, pool);
    }
  }
  return result;
}

BigRealMatrix  quot(const BigRealMatrix &lts, const BigReal       &d, DigitPool *digitPool) {
  _SELECTDIGITPOOL(lts);
  const size_t rows   = lts.getRowCount();
  const size_t cols   = lts.getColumnCount();
  const UINT   digits = lts.getPrecision();

  BigRealMatrix result(rows, cols, digits, pool);
  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      result(r, c) = rQuot(lts(r, c), d, digits, pool);
    }
  }
  return result;
}

BigRealVector  prod(const BigRealVector &lts, const BigRealMatrix &rhs, DigitPool *digitPool) {
  if(lts.getDimension() != rhs.getRowCount()) {
    throwBigRealException(_T("%s:Invalid dimension. Vector.%s, Matrix.%s"), __TFUNCSIG__, lts.getDimensionString().cstr(), rhs.getDimensionString().cstr());
  }
  _SELECTDIGITPOOL(lts);
  const UINT digits = min(lts.getPrecision(), rhs.getPrecision());

  BigRealVector result(rhs.getColumnCount(), digits, pool);
  BigReal sum(0, pool);
  for(size_t c = 0; c < rhs.getColumnCount(); c++) {
    sum.setToZero();
    for(size_t r = 0; r < rhs.getRowCount(); r++) {
      sum = rSum(sum, rProd(lts(r), rhs(r,c), digits,pool), digits,pool);
    }
    result[c] = sum;
  }
  return result;
}

BigRealVector  prod(const BigRealMatrix &lts, const BigRealVector &rhs, DigitPool *digitPool) {
  if(lts.getColumnCount() != rhs.getDimension()) {
    throwBigRealException(_T("%s:Invalid dimension. Matrix.%s, Vector.%s"), __TFUNCSIG__, lts.getDimensionString().cstr(),rhs.getDimensionString().cstr());
  }
  _SELECTDIGITPOOL(lts);
  const UINT digits = min(lts.getPrecision(), rhs.getPrecision());

  BigRealVector result(lts.getRowCount(),digits, pool);
  BigReal sum(0, pool);
  for(size_t r = 0; r < lts.getRowCount(); r++) {
    sum.setToZero();
    for(size_t c = 0; c < lts.getColumnCount(); c++) {
      sum = rSum(sum, rProd(lts(r,c), rhs[c], digits,pool), digits,pool);
    }
    result[r] = sum;
  }
  return result;

}
BigRealMatrix  sum(const BigRealMatrix &lts, const BigRealMatrix &rhs, DigitPool *digitPool) {
  lts.checkSameDimension(__TFUNCSIG__, rhs);
  const size_t rows   = lts.getRowCount();
  const size_t cols   = lts.getColumnCount();
  const UINT   digits = min(lts.getPrecision(), rhs.getPrecision());
  _SELECTDIGITPOOL(lts);

  BigRealMatrix result(lts.getDimension(),digits,pool);
  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      result(r,c) = rSum(lts(r,c), rhs(r,c), digits,pool);
    }
  }
  return result;
}

BigRealMatrix  dif(const BigRealMatrix &lts, const BigRealMatrix &rhs, DigitPool *digitPool) {
  lts.checkSameDimension(__TFUNCSIG__, rhs);
  const size_t rows   = lts.getRowCount();
  const size_t cols   = lts.getColumnCount();
  const UINT   digits = min(lts.getPrecision(), rhs.getPrecision());
  _SELECTDIGITPOOL(lts);

  BigRealMatrix result(lts.getDimension(),digits,pool);
  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      result(r,c) = rDif(lts(r,c), rhs(r,c), digits,pool);
    }
  }
  return result;
}

BigRealMatrix  prod(const BigRealMatrix &lts, const BigRealMatrix &rhs, DigitPool *digitPool) {
  const UINT digits = min(lts.getPrecision(), rhs.getPrecision());

  if (lts.getColumnCount() != rhs.getRowCount()) {
    throwBigRealException(_T("operator*(BigRealMatrix,BigRealMatrix):Invalid dimension. Matrix.%s, Vector.%s"), lts.getDimensionString().cstr(), rhs.getDimensionString().cstr());
  }
  _SELECTDIGITPOOL(lts);

  BigRealMatrix result(lts.getRowCount(), rhs.getColumnCount(), digits, pool);
  BigReal sum(0, pool);
  for(size_t r = 0; r < lts.getRowCount(); r++) {
    for(size_t c = 0; c < rhs.getColumnCount(); c++) {
      sum.setToZero();
      for(size_t k = 0; k < lts.getColumnCount(); k++) {
        sum = rSum(sum, rProd(lts(r, k), rhs(k, c), digits,pool), digits, pool);
      }
      result(r, c) = sum;
    }
  }
  return result;
}

/*
static inline double sqr(double x) { return x*x; }

double normf(const BigRealMatrix& a) {
  double sum = 0;
  for(int r = 0; r < a.m_rows; r++)
    for(int c = 0; c < a.m_cols; c++)
      sum += sqr(a.m_a[r][c]);
  return sqrt(sum);
}

double norm1(const BigRealMatrix& a) {
  double m = 0;
  for(int c = 0; c < a.m_cols; c++) {
    double sum = 0;
    for(int r = 0; r < a.m_rows; r++)
      sum += fabs(a.m_a[r][c]);
    if(sum > m) m = sum;
  }
  return m;
}

double norm00( const BigRealMatrix& a) {
  double m = 0;
  for(int r = 0; r < a.m_rows; r++) {
    double sum = 0;
    for(int c = 0; c < a.m_cols; c++)
      sum += fabs(a.m_a[r][c]);
    if(sum > m) m = sum;
  }
  return m;
}
*/

BigRealMatrix operator-(const BigRealMatrix &m) {
  const size_t rows   = m.getRowCount();
  const size_t cols   = m.getColumnCount();
  const UINT   digits = m.getPrecision();

  BigRealMatrix result(m.getDimension(), digits, m.getDigitPool());
  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      result(r,c) = -m(r,c);
    }
  }
  return result;
}

BigRealMatrix &BigRealMatrix::operator+=(const BigRealMatrix &rhs) {
  checkSameDimension(__TFUNCSIG__, rhs);
  const size_t rows   = getRowCount();
  const size_t cols   = getColumnCount();
  const UINT   digits = min(getPrecision(), rhs.getPrecision());

  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      (*this)(r,c) = rSum((*this)(r,c), rhs(r,c), digits);
    }
  }
  setPrecision(digits);
  return *this;
}

BigRealMatrix &BigRealMatrix::operator-=(const BigRealMatrix &rhs) {
  checkSameDimension(__TFUNCSIG__, rhs);
  const size_t rows   = getRowCount();
  const size_t cols   = getColumnCount();
  const UINT   digits = min(getPrecision(), rhs.getPrecision());

  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      (*this)(r,c) = rDif((*this)(r,c), rhs(r,c), digits);
    }
  }
  setPrecision(digits);
  return *this;
}

BigRealMatrix &BigRealMatrix::operator*=(const BigReal &d) {
  const size_t rows   = getRowCount();
  const size_t cols   = getColumnCount();
  const UINT   digits = getPrecision();

  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      (*this)(r,c) = rProd((*this)(r,c), d, digits);
    }
  }
  return *this;
}

BigRealMatrix &BigRealMatrix::operator/=(const BigReal &d) {
  const size_t rows   = getRowCount();
  const size_t cols   = getColumnCount();
  const UINT   digits = getPrecision();

  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      (*this)(r,c) = rQuot((*this)(r,c), d, digits);
    }
  }
  return *this;
}

BigRealMatrix inverse(const BigRealMatrix &a, DigitPool *digitPool) {
  if(!a.isSquare()) {
    throwBigRealException(_T("%s:Matrix not square. %s"), __TFUNCSIG__, a.getDimensionString().cstr());
  }

  BigRealLUMatrix lu(a);
  return lu.getInverse();
}

BigRealMatrix transpose(const BigRealMatrix &m, DigitPool *digitPool) {
  _SELECTDIGITPOOL(m);
  BigRealMatrix result(m.getColumnCount(), m.getRowCount(), m.getPrecision(), pool);
  for(size_t r = 0; r < m.getRowCount(); r++) {
    for(size_t c = 0; c < m.getColumnCount(); c++) {
      result(c,r) = m(r,c);
    }
  }
  return result;
}

BigReal det(const BigRealMatrix &m, DigitPool *digitPool) {
  try {
    _SELECTDIGITPOOL(m);
    BigRealLUMatrix lu(m);
    return lu.getDeterminant();
  } catch(BigRealException) {
    return 0;
  }
}
