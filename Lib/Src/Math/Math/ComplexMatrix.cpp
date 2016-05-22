#include "pch.h"
#include <Math/Matrix.h>
#include <Math/MathException.h>

ComplexMatrix::ComplexMatrix() : MatrixTemplate<Complex>() {
}

ComplexMatrix::ComplexMatrix(Complex c) : MatrixTemplate<Complex>() {
  (*this)(0,0) = c;
}

ComplexMatrix::ComplexMatrix(unsigned int rows, unsigned int columns) : MatrixTemplate<Complex>(rows,columns) {
}

ComplexMatrix::ComplexMatrix(const MatrixTemplate<Complex> &src) : MatrixTemplate<Complex>(src) {
}

ComplexMatrix::ComplexMatrix(const VectorTemplate<Complex> &diagonal) : MatrixTemplate<Complex>(diagonal) {
}

ComplexMatrix::ComplexMatrix(const Matrix &src) : MatrixTemplate<Complex>(src.getRowCount(),src.getColumnCount()) {
  for(int r = 0; r < getRowCount(); r++) {
    for(int c = 0; c < getColumnCount(); c++) {
      (*this)(r,c) = src(r,c);
    }
  }
}

ComplexMatrix &ComplexMatrix::operator=(const Matrix &src) {
  setDimension(src.getRowCount(),src.getColumnCount());
  for(int r = 0; r < getRowCount(); r++) {
    for(int c = 0; c < getColumnCount(); c++) {
      (*this)(r,c) = src(r,c);
    }
  }
  return *this;
}

ComplexMatrix::ComplexMatrix(const Vector &diagonal) : MatrixTemplate<Complex>(diagonal.getDimension(),diagonal.getDimension()) {
  for(int i = 0; i < getRowCount(); i++) {
    (*this)(i,i) = diagonal(i);
  }
}

Matrix ComplexMatrix::getRealPart() const {
  Matrix result(getRowCount(),getColumnCount());
  for(int r = 0; r < getRowCount(); r++) {
    for(int c = 0; c < getColumnCount(); c++) {
      result(r,c) = (*this)(r,c).re;
    }
  }
  return result;
}

Matrix ComplexMatrix::getImaginaryPart() const {
  Matrix result(getRowCount(),getColumnCount());
  for(int r = 0; r < getRowCount(); r++) {
    for(int c = 0; c < getColumnCount(); c++) {
      result(r,c) = (*this)(r,c).im;
    }
  }
  return result;
}

void setToRandom(ComplexVector &v) {
  for(unsigned int i = 0; i < v.getDimension(); i++) {
    setToRandom(v[i]);
  }
}

void setToRandom(ComplexMatrix &a) {
  for(int r = 0; r < a.getRowCount(); r++) {
    for(int c = 0; c < a.getColumnCount(); c++) {
      setToRandom(a(r,c));
    }
  }
}

Real fabs(const ComplexVector &v) {
  Real sum = 0;
  for(unsigned int i = 0; i < v.getDimension(); i++) {
    sum += arg2(v[i]);
  }
  return sqrt(sum);
}

Real fabs(const ComplexMatrix &a) {
  return normf(a);
}

Real normf(const ComplexMatrix &a) {
  Real sum = 0;
  for(int r = 0; r < a.getRowCount(); r++) {
    for(int c = 0; c < a.getColumnCount(); c++) {
      sum += arg2(a(r,c));
    }
  }
  return sqrt(sum);
}

ComplexMatrix inverse(const ComplexMatrix &a) {
  if(!a.isSquare()) {
    throwMathException(_T("inverse(ComplexMatrix):Matrix not square. Dimension = (%u,%u)"), a.getRowCount(), a.getColumnCount());
  }

  ComplexLUMatrix lu(a);
  return lu.getInverse();
}

Complex det(const ComplexMatrix &a) {
  try {
    ComplexLUMatrix lu(a);
    return lu.getDeterminant();
  } catch(MathException) {
    return 0;
  }
}

ComplexVector operator*(const Matrix &lts, const ComplexVector &rhs) {
  const int rows    = lts.getRowCount();
  const int columns = lts.getColumnCount();

  if((unsigned int)columns != rhs.getDimension()) {
    throwException(_T("operator*(Matrix,ComplexVector):Invalid dimension. Matrix.dimension = (%u,%u), Vector.dimension = %u."), lts.getRowCount(), lts.getColumnCount(), rhs.getDimension());
  }
  ComplexVector result(rows);
  for(int r = 0; r < rows; r++) {
	Complex sum = 0;
    for(int c = 0; c < columns; c++) {
      sum += lts(r,c) * rhs[c];
    }
	result[r] = sum;
  }
  return result;
}

ComplexMatrix operator*(const Complex &lts, const Matrix &rhs) {
  const int rows    = rhs.getRowCount();
  const int columns = rhs.getColumnCount();

  ComplexMatrix result(rows,columns);
  for(int r = 0; r < rows; r++) {
    for(int c = 0; c < columns; c++) {
      result(r,c) = lts * rhs(r,c);
    }
  }
  return result;
}
