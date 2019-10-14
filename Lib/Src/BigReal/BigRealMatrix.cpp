#include "pch.h"
#include <Math/BigRealMatrix.h>

// Implementation of basic matrix-operations

DEFINECLASSNAME(BigRealMatrix);

void BigRealMatrix::checkPrecision(UINT digits) {
  if(digits == 0) {
    throwBigRealException(_T("%s:Precision = 0 not allowed"), s_className);
  }
}

void BigRealMatrix::init(size_t rows, size_t cols, bool initialize, UINT digits) {
  MatrixTemplate<BigReal>::init(rows, cols, initialize);
  checkPrecision(digits);
  m_digits = digits;
}

BigRealMatrix::BigRealMatrix(size_t rows, size_t cols, UINT digits) {
  init(rows, cols, true, digits);
}

BigRealMatrix::BigRealMatrix(const BigRealMatrix &a) {
  init(a.getRowCount(), a.getColumnCount(), false, a.getPrecision());
  for(size_t r = 0; r < getRowCount(); r++) {
    for(size_t c = 0; c < getColumnCount(); c++) {
      (*this)(r,c) = a(r,c);
    }
  }
}

UINT BigRealMatrix::setPrecision(UINT digits) {
  checkPrecision(digits);
  const UINT oldDigits = m_digits;
  m_digits = digits;
  return oldDigits;
}

BigRealMatrix BigRealMatrix::_0(size_t rows, size_t columns, UINT digits) { // static
  return BigRealMatrix(rows, columns, digits);
}

BigRealMatrix BigRealMatrix::_1(size_t dim, UINT digits) { // static
  BigRealMatrix result(dim,dim,digits);
  for(size_t i = 0; i < dim; i++) {
    result(i,i) = BIGREAL_1;
  }
  return result;
}

BigRealVector BigRealMatrix::getRow(size_t row) const {
  return BigRealVector(MatrixTemplate<BigReal>::getRow(row), getPrecision());
}

BigRealVector BigRealMatrix::getColumn(size_t column) const {
  return BigRealVector(MatrixTemplate<BigReal>::getColumn(column), getPrecision());
}

// Frobenius norm
BigReal normf(const BigRealMatrix &a) {
  const UINT digits = a.getPrecision();
  BigReal    sum    = 0;
  for(size_t r = 0; r < a.getRowCount(); r++) {
    for(size_t c = 0; c < a.getColumnCount(); c++) {
      sum += rProd(a(r,c), a(r,c), digits);
    }
  }
  return rSqrt(sum,digits);
}

BigRealVector operator*(const BigRealMatrix &lts, const BigRealVector& rhs) {
  const UINT digits = min(lts.getPrecision(), rhs.getPrecision());

  if(lts.getColumnCount() != rhs.getDimension()) {
    throwBigRealException(_T("operator*(BigRealMatrix,BigRealVector):Invalid dimension. Matrix.%s, Vector.%s"),lts.getDimensionString().cstr(),rhs.getDimensionString().cstr());
  }

  BigRealVector result(lts.getRowCount(),digits);
  for(size_t r = 0; r < lts.getRowCount(); r++) {
    BigReal sum = 0;
    for(size_t c = 0; c < lts.getColumnCount(); c++) {
      sum = rSum(sum, rProd(lts(r,c), rhs[c], digits), digits);
    }
    result[r] = sum;
  }
  return result;
}

BigRealVector operator*(const BigRealVector &lts, const BigRealMatrix& rhs) {
  const UINT digits = min(lts.getPrecision(), rhs.getPrecision());

  if(lts.getDimension() != rhs.getRowCount()) {
    throwBigRealException(_T("operator*(BigRealVector,BigRealMatrix):Invalid dimension. Vector.%s, Matrix.%s"), lts.getDimensionString().cstr(), rhs.getDimensionString().cstr());
  }

  BigRealVector result(rhs.getColumnCount(), digits);
  for(size_t c = 0; c < rhs.getColumnCount(); c++) {
    BigReal sum = 0;
    for(size_t r = 0; r < rhs.getColumnCount(); r++) {
      sum = rSum(sum, rProd(lts(r), rhs(r,c), digits), digits);
    }
    result[c] = sum;
  }
  return result;
}

BigRealMatrix operator*(const BigRealMatrix& lts, const BigRealMatrix& rhs) {
  const UINT digits = min(lts.getPrecision(), rhs.getPrecision());

  if(lts.getColumnCount() != rhs.getRowCount()) {
    throwBigRealException(_T("operator*(BigRealMatrix,BigRealMatrix):Invalid dimension. Matrix.%s, Vector.%s"), lts.getDimensionString().cstr(), rhs.getDimensionString().cstr());
  }

  BigRealMatrix result(lts.getRowCount(),rhs.getColumnCount(),digits);
  for(size_t r = 0; r < lts.getRowCount(); r++) {
    for(size_t c = 0; c < rhs.getColumnCount(); c++) {
      BigReal sum = 0;
      for(size_t k = 0; k < lts.getColumnCount(); k++) {
        sum = rSum(sum, rProd(lts(r,k), rhs(k,c), digits), digits);
      }
      result(r,c) = sum;
    }
  }
  return result;
}

BigRealMatrix operator+(const BigRealMatrix& lts, const BigRealMatrix& rhs) {
  const size_t rows   = lts.getRowCount();
  const size_t cols   = lts.getColumnCount();
  const UINT   digits = min(lts.getPrecision(), rhs.getPrecision());

  lts.checkSameDimension(__TFUNCTION__, rhs);

  BigRealMatrix result(rows,cols,digits);
  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      result(r,c) = rSum(lts(r,c), rhs(r,c), digits);
    }
  }
  return result;
}

BigRealMatrix operator-(const BigRealMatrix& lts, const BigRealMatrix& rhs) {
  const size_t rows   = lts.getRowCount();
  const size_t cols   = lts.getColumnCount();
  const UINT   digits = min(lts.getPrecision(), rhs.getPrecision());

  lts.checkSameDimension(__TFUNCTION__, rhs);

  BigRealMatrix result(rows,cols,digits);
  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      result(r,c) = rDif(lts(r,c), rhs(r,c), digits);
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
BigRealMatrix operator*(const BigRealMatrix &lts, const BigReal &d) {
  const size_t rows   = lts.getRowCount();
  const size_t cols   = lts.getColumnCount();
  const UINT   digits = lts.getPrecision();

  BigRealMatrix result(rows, cols, digits);
  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      result(r,c) = rProd(lts(r,c), d, digits);
    }
  }
  return result;
}

BigRealMatrix operator*(const BigReal &d, const BigRealMatrix &rhs) {
  return rhs * d;
}

BigRealMatrix operator/(const BigRealMatrix &lts, const BigReal &d) {
  const size_t rows   = lts.getRowCount();
  const size_t cols   = lts.getColumnCount();
  const UINT   digits = lts.getPrecision();

  BigRealMatrix result(rows, cols, digits);
  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      result(r,c) = rQuot(lts(r,c), d, digits);
    }
  }
  return result;
}

BigRealMatrix operator-(const BigRealMatrix &m) {
  const size_t rows   = m.getRowCount();
  const size_t cols   = m.getColumnCount();
  const UINT   digits = m.getPrecision();

  BigRealMatrix result(rows, cols, digits);
  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      result(r,c) = -m(r,c);
    }
  }
  return result;
}

BigRealMatrix &BigRealMatrix::operator+=(const BigRealMatrix &rhs) {
  checkSameDimension(__TFUNCTION__, rhs);
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
  checkSameDimension(__TFUNCTION__, rhs);
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

BigRealMatrix inverse(const BigRealMatrix &a) {
  if(!a.isSquare()) {
    throwBigRealException(_T("inverse(BigRealMatrix):Matrix not square. %s"), a.getDimensionString().cstr());
  }

  BigRealLUMatrix lu(a);
  return lu.getInverse();
}

BigRealMatrix transpose(const BigRealMatrix &a) {
  BigRealMatrix result(a.getColumnCount(), a.getRowCount(), a.getPrecision());
  for(size_t r = 0; r < a.getRowCount(); r++) {
    for(size_t c = 0; c < a.getColumnCount(); c++) {
      result(c,r) = a(r,c);
    }
  }
  return result;
}

BigReal det(const BigRealMatrix &a) {
  try {
    BigRealLUMatrix lu(a);
    return lu.getDeterminant();
  } catch(BigRealException) {
    return 0;
  }
}
