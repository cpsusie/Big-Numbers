#include "pch.h"
#include <Math/BigRealMatrix.h>

// Implementation of basic matrix-operations

void BigRealMatrix::checkPrecision(unsigned int digits) {
  if(digits == 0) {
    throwBigRealException(_T("BigRealMatrix:Precision = 0 not allowed."));
  }
}

void BigRealMatrix::init(unsigned int rows, unsigned int cols, bool initialize, unsigned int digits) {
  MatrixTemplate<BigReal>::init(rows,cols,initialize);
  checkPrecision(digits);
  m_digits = digits;
}

BigRealMatrix::BigRealMatrix(unsigned int rows, unsigned int cols, unsigned int digits) {
  init(rows,cols,true,digits);
}

BigRealMatrix::BigRealMatrix(const BigRealMatrix &a) {
  init(a.getRowCount(),a.getColumnCount(),false,a.getPrecision());
  for(int r = 0; r < getRowCount(); r++) {
    for(int c = 0; c < getColumnCount(); c++) {
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

BigRealMatrix BigRealMatrix::one(unsigned int dim, unsigned int digits) {
  BigRealMatrix result(dim,dim,digits);
  for(unsigned int i = 0; i < dim; i++) {
    result(i,i) = 1;
  }
  return result;
}

BigRealMatrix BigRealMatrix::zero(unsigned int rows, unsigned int columns, unsigned int digits) {
  return BigRealMatrix(rows,columns,digits);
}

BigRealVector BigRealMatrix::getRow(unsigned int row) const {
  return BigRealVector(MatrixTemplate<BigReal>::getRow(row),getPrecision());
}

BigRealVector BigRealMatrix::getColumn(unsigned int column) const {
  return BigRealVector(MatrixTemplate<BigReal>::getColumn(column),getPrecision());
}

// Frobenius norm
BigReal normf(const BigRealMatrix &a) {
  BigReal sum = 0;
  for(int r = 0; r < a.getRowCount(); r++) {
    for(int c = 0; c < a.getColumnCount(); c++) {
      sum += a(r,c)*a(r,c);
    }
  }
  return rSqrt(sum,a.getPrecision());
}

BigRealVector operator*(const BigRealMatrix &lts, const BigRealVector& rhs) {
  const int digits = min(lts.getPrecision(),rhs.getPrecision());

  if((unsigned int)lts.getColumnCount() != rhs.getDimension()) {
    throwBigRealException(_T("operator*(BigRealMatrix,BigRealVector):Invalid dimension. Matrix.dimension=(%u,%u), Vector.dimension=%u."),lts.getRowCount(),lts.getColumnCount(),rhs.getDimension());
  }
  
  BigRealVector result(lts.getRowCount(),digits);
  for(int r = 0; r < lts.getRowCount(); r++) {
    BigReal sum = 0;
    for(int c = 0; c < lts.getColumnCount(); c++) {
      sum = rSum(sum,rProd(lts(r,c),rhs[c],digits),digits);
    }
    result[r] = sum;
  }
  return result;
}

BigRealVector operator*(const BigRealVector &lts, const BigRealMatrix& rhs) {
  const int digits = min(lts.getPrecision(),rhs.getPrecision());

  if(lts.getDimension() != (unsigned int)rhs.getRowCount()) {
    throwBigRealException(_T("operator*(BigRealVector,BigRealMatrix):Invalid dimension. Vector.dimension=%u, Matrix.dimension=(%u,%u)."), lts.getDimension(),rhs.getRowCount(), rhs.getColumnCount());
  }

  BigRealVector result(rhs.getColumnCount(),digits);
  for(int c = 0; c < rhs.getColumnCount(); c++) {
    BigReal sum = 0;
    for(int r = 0; r < rhs.getColumnCount(); r++) {
      sum = rSum(sum,rProd(lts(r),rhs(r,c),digits),digits);
    }
    result[c] = sum;
  }
  return result;
}

BigRealMatrix operator*(const BigRealMatrix& lts, const BigRealMatrix& rhs) {
  const int digits = min(lts.getPrecision(),rhs.getPrecision());

  if(lts.getColumnCount() != rhs.getRowCount()) {
    throwBigRealException(_T("operator*(BigRealMatrix,BigRealMatrix):Invalid dimension. left.dimension=(%u,%u), right.dimension=(%u,%u)"), lts.getRowCount(), lts.getColumnCount(), rhs.getRowCount(), rhs.getColumnCount());
  }

  BigRealMatrix result(lts.getRowCount(),rhs.getColumnCount(),digits);
  for(int r = 0; r < lts.getRowCount(); r++) {
    for(int c = 0; c < rhs.getColumnCount(); c++) {
      BigReal sum = 0;
      for(int k = 0; k < lts.getColumnCount(); k++) {
        sum = rSum(sum,rProd(lts(r,k),rhs(k,c),digits),digits);
      }
      result(r,c) = sum;
    }
  }
  return result;
}

BigRealMatrix operator+(const BigRealMatrix& lts, const BigRealMatrix& rhs) {
  const int rows = lts.getRowCount();
  const int cols = lts.getColumnCount();
  const int digits = min(lts.getPrecision(),rhs.getPrecision());

  if(rows != rhs.getRowCount() || cols != rhs.getColumnCount()) {
    throwBigRealException(_T("operator+(BigRealMatrix,BigRealMatrix):Invalid dimension. left.dimension=(%u,%u), right.dimension=(%u,%u)"), lts.getRowCount(), lts.getColumnCount(), rhs.getRowCount(), rhs.getColumnCount());
  }

  BigRealMatrix result(rows,cols,digits);
  for(int r = 0; r < rows; r++) {
    for(int c = 0; c < cols; c++) {
      result(r,c) = rSum(lts(r,c),rhs(r,c),digits);
    }
  }
  return result;   
}

BigRealMatrix operator-(const BigRealMatrix& lts, const BigRealMatrix& rhs) {
  const int rows = lts.getRowCount();
  const int cols = lts.getColumnCount();
  const int digits = min(lts.getPrecision(),rhs.getPrecision());

  if(rows != rhs.getRowCount() || cols != rhs.getColumnCount()) {
    throwBigRealException(_T("operator-(BigRealMatrix,BigRealMatrix):Invalid dimension. left.dimension=(%u,%u), right.dimension=(%u,%u)"), lts.getRowCount(), lts.getColumnCount(), rhs.getRowCount(), rhs.getColumnCount());
  }
  
  BigRealMatrix result(rows,cols,digits);
  for(int r = 0; r < rows; r++) {
    for(int c = 0; c < cols; c++) {
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
  const int rows = lts.getRowCount();
  const int cols = lts.getColumnCount();
  const int digits = lts.getPrecision();

  BigRealMatrix result(rows,cols,digits);
  for(int r = 0; r < rows; r++) {
    for(int c = 0; c < cols; c++) {
      result(r,c) = rProd(lts(r,c),d,digits);
    }
  }
  return result;
}

BigRealMatrix operator*(const BigReal &d, const BigRealMatrix &rhs) {
  return rhs * d;
}

BigRealMatrix operator/(const BigRealMatrix &lts, const BigReal &d) {
  const int rows    = lts.getRowCount();
  const int cols    = lts.getColumnCount();
  const int digits = lts.getPrecision();

  BigRealMatrix result(rows,cols,digits);
  for(int r = 0; r < rows; r++) {
    for(int c = 0; c < cols; c++) {
      result(r,c) = rQuot(lts(r,c),d,digits);
    }
  }
  return result;
}

BigRealMatrix inverse(const BigRealMatrix &a) {
  if(!a.isSquare()) {
    throwBigRealException(_T("inverse(BigRealMatrix):Matrix not square. Dimension = (%u,%u)"), a.getRowCount(), a.getColumnCount());
  }

  BigRealLUMatrix lu(a);
  return lu.getInverse();
}

BigRealMatrix transpose(const BigRealMatrix &a) {
  BigRealMatrix result(a.getColumnCount(),a.getRowCount(),a.getPrecision());
  for(int r = 0; r < a.getRowCount(); r++) {
    for(int c = 0; c < a.getColumnCount(); c++) {
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
