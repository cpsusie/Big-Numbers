#include "pch.h"
#include <Math/BigRealMatrix.h>

// Implementation of basic matrix-operations

DEFINECLASSNAME(BigRealMatrix);

void BigRealMatrix::checkPrecision(unsigned int digits) {
  if(digits == 0) {
    throwBigRealException(_T("%s:Precision = 0 not allowed."), s_className);
  }
}

void BigRealMatrix::init(size_t rows, size_t cols, bool initialize, unsigned int digits) {
  MatrixTemplate<BigReal>::init(rows,cols,initialize);
  checkPrecision(digits);
  m_digits = digits;
}

BigRealMatrix::BigRealMatrix(size_t rows, size_t cols, unsigned int digits) {
  init(rows,cols,true,digits);
}

BigRealMatrix::BigRealMatrix(const BigRealMatrix &a) {
  init(a.getRowCount(),a.getColumnCount(),false,a.getPrecision());
  for(size_t r = 0; r < getRowCount(); r++) {
    for(size_t c = 0; c < getColumnCount(); c++) {
      (*this)(r,c) = a(r,c);
    }
  }
}

unsigned int BigRealMatrix::setPrecision(unsigned int digits) {
  checkPrecision(digits);
  unsigned int oldDigits = m_digits;
  m_digits = digits;
  return oldDigits;
}

BigRealMatrix BigRealMatrix::one(size_t dim, unsigned int digits) {
  BigRealMatrix result(dim,dim,digits);
  for(size_t i = 0; i < dim; i++) {
    result(i,i) = 1;
  }
  return result;
}

BigRealMatrix BigRealMatrix::zero(size_t rows, size_t columns, unsigned int digits) {
  return BigRealMatrix(rows,columns,digits);
}

BigRealVector BigRealMatrix::getRow(size_t row) const {
  return BigRealVector(MatrixTemplate<BigReal>::getRow(row),getPrecision());
}

BigRealVector BigRealMatrix::getColumn(size_t column) const {
  return BigRealVector(MatrixTemplate<BigReal>::getColumn(column),getPrecision());
}

// Frobenius norm
BigReal normf(const BigRealMatrix &a) {
  BigReal sum = 0;
  for(size_t r = 0; r < a.getRowCount(); r++) {
    for(size_t c = 0; c < a.getColumnCount(); c++) {
      sum += a(r,c)*a(r,c);
    }
  }
  return rSqrt(sum,a.getPrecision());
}

BigRealVector operator*(const BigRealMatrix &lts, const BigRealVector& rhs) {
  const int digits = min(lts.getPrecision(),rhs.getPrecision());

  if(lts.getColumnCount() != rhs.getDimension()) {
    throwBigRealException(_T("operator*(BigRealMatrix,BigRealVector):Invalid dimension. Matrix.%s, Vector.dimension=%s."),lts.getDimensionString().cstr(),format1000(rhs.getDimension()).cstr());
  }
  
  BigRealVector result(lts.getRowCount(),digits);
  for(size_t r = 0; r < lts.getRowCount(); r++) {
    BigReal sum = 0;
    for(size_t c = 0; c < lts.getColumnCount(); c++) {
      sum = rSum(sum,rProd(lts(r,c),rhs[c],digits),digits);
    }
    result[r] = sum;
  }
  return result;
}

BigRealVector operator*(const BigRealVector &lts, const BigRealMatrix& rhs) {
  const int digits = min(lts.getPrecision(),rhs.getPrecision());

  if(lts.getDimension() != rhs.getRowCount()) {
    throwBigRealException(_T("operator*(BigRealVector,BigRealMatrix):Invalid dimension. Vector.dimension=%s, Matrix.%s."), format1000(lts.getDimension()).cstr(),rhs.getDimensionString().cstr());
  }

  BigRealVector result(rhs.getColumnCount(),digits);
  for(size_t c = 0; c < rhs.getColumnCount(); c++) {
    BigReal sum = 0;
    for(size_t r = 0; r < rhs.getColumnCount(); r++) {
      sum = rSum(sum,rProd(lts(r),rhs(r,c),digits),digits);
    }
    result[c] = sum;
  }
  return result;
}

BigRealMatrix operator*(const BigRealMatrix& lts, const BigRealMatrix& rhs) {
  const int digits = min(lts.getPrecision(),rhs.getPrecision());

  if(lts.getColumnCount() != rhs.getRowCount()) {
    throwBigRealException(_T("operator*(BigRealMatrix,BigRealMatrix):Invalid dimension. left:%s, right:%s"), lts.getDimensionString().cstr(), rhs.getDimensionString().cstr());
  }

  BigRealMatrix result(lts.getRowCount(),rhs.getColumnCount(),digits);
  for(size_t r = 0; r < lts.getRowCount(); r++) {
    for(size_t c = 0; c < rhs.getColumnCount(); c++) {
      BigReal sum = 0;
      for(size_t k = 0; k < lts.getColumnCount(); k++) {
        sum = rSum(sum,rProd(lts(r,k),rhs(k,c),digits),digits);
      }
      result(r,c) = sum;
    }
  }
  return result;
}

BigRealMatrix operator+(const BigRealMatrix& lts, const BigRealMatrix& rhs) {
  const size_t rows = lts.getRowCount();
  const size_t cols = lts.getColumnCount();
  const int digits = min(lts.getPrecision(),rhs.getPrecision());

  lts.checkSameDimension(_T("operator+"), rhs);
  
  BigRealMatrix result(rows,cols,digits);
  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      result(r,c) = rSum(lts(r,c),rhs(r,c),digits);
    }
  }
  return result;   
}

BigRealMatrix operator-(const BigRealMatrix& lts, const BigRealMatrix& rhs) {
  const size_t rows = lts.getRowCount();
  const size_t cols = lts.getColumnCount();
  const int digits = min(lts.getPrecision(),rhs.getPrecision());

  lts.checkSameDimension(_T("operator-"), rhs);

  BigRealMatrix result(rows,cols,digits);
  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      result(r,c) = rDif(lts(r,c),rhs(r,c),digits);
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
  const size_t rows = lts.getRowCount();
  const size_t cols = lts.getColumnCount();
  const int digits = lts.getPrecision();

  BigRealMatrix result(rows,cols,digits);
  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      result(r,c) = rProd(lts(r,c),d,digits);
    }
  }
  return result;
}

BigRealMatrix operator*(const BigReal &d, const BigRealMatrix &rhs) {
  return rhs * d;
}

BigRealMatrix operator/(const BigRealMatrix &lts, const BigReal &d) {
  const size_t rows    = lts.getRowCount();
  const size_t cols    = lts.getColumnCount();
  const int    digits  = lts.getPrecision();

  BigRealMatrix result(rows,cols,digits);
  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < cols; c++) {
      result(r,c) = rQuot(lts(r,c),d,digits);
    }
  }
  return result;
}

BigRealMatrix inverse(const BigRealMatrix &a) {
  if(!a.isSquare()) {
    throwBigRealException(_T("inverse(BigRealMatrix):Matrix not square. %s"), a.getDimensionString().cstr());
  }

  BigRealLUMatrix lu(a);
  return lu.getInverse();
}

BigRealMatrix transpose(const BigRealMatrix &a) {
  BigRealMatrix result(a.getColumnCount(),a.getRowCount(),a.getPrecision());
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
