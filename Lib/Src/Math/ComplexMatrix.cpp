#include "pch.h"
#include <Math/Matrix.h>

ComplexMatrix::ComplexMatrix() : MatrixTemplate<Complex>() {
}

ComplexMatrix::ComplexMatrix(Complex c) : MatrixTemplate<Complex>() {
  (*this)(0,0) = c;
}

ComplexMatrix::ComplexMatrix(size_t rows, size_t columns) : MatrixTemplate<Complex>(rows,columns) {
}

ComplexMatrix::ComplexMatrix(const MatrixTemplate<Complex> &src) : MatrixTemplate<Complex>(src) {
}

ComplexMatrix::ComplexMatrix(const VectorTemplate<Complex> &diagonal) : MatrixTemplate<Complex>(diagonal) {
}

ComplexMatrix::ComplexMatrix(const Matrix &src) : MatrixTemplate<Complex>(src.getRowCount(),src.getColumnCount()) {
  for(size_t r = 0; r < getRowCount(); r++) {
    for(size_t c = 0; c < getColumnCount(); c++) {
      (*this)(r,c) = src(r,c);
    }
  }
}

ComplexMatrix &ComplexMatrix::operator=(const Matrix &src) {
  setDimension(src.getRowCount(),src.getColumnCount());
  for(size_t r = 0; r < getRowCount(); r++) {
    for(size_t c = 0; c < getColumnCount(); c++) {
      (*this)(r,c) = src(r,c);
    }
  }
  return *this;
}

ComplexMatrix::ComplexMatrix(const Vector &diagonal) : MatrixTemplate<Complex>(diagonal.getDimension(),diagonal.getDimension()) {
  for(size_t i = 0; i < getRowCount(); i++) {
    (*this)(i,i) = diagonal(i);
  }
}

Matrix ComplexMatrix::getRealPart() const {
  Matrix result(getRowCount(),getColumnCount());
  for(size_t r = 0; r < getRowCount(); r++) {
    for(size_t c = 0; c < getColumnCount(); c++) {
      result(r,c) = (*this)(r,c).re;
    }
  }
  return result;
}

Matrix ComplexMatrix::getImaginaryPart() const {
  Matrix result(getRowCount(),getColumnCount());
  for(size_t r = 0; r < getRowCount(); r++) {
    for(size_t c = 0; c < getColumnCount(); c++) {
      result(r,c) = (*this)(r,c).im;
    }
  }
  return result;
}

void setToRandom(ComplexVector &v, RandomGenerator *rnd) {
  for(size_t i = 0; i < v.getDimension(); i++) {
    setToRandom(v[i], rnd);
  }
}

void setToRandom(ComplexMatrix &a, RandomGenerator *rnd) {
  for(size_t r = 0; r < a.getRowCount(); r++) {
    for(size_t c = 0; c < a.getColumnCount(); c++) {
      setToRandom(a(r,c), rnd);
    }
  }
}

Real fabs(const ComplexVector &v) {
  Real sum = 0;
  for(size_t i = 0; i < v.getDimension(); i++) {
    sum += arg2(v[i]);
  }
  return sqrt(sum);
}

Real fabs(const ComplexMatrix &a) {
  return normf(a);
}

Real normf(const ComplexMatrix &a) {
  Real sum = 0;
  for(size_t r = 0; r < a.getRowCount(); r++) {
    for(size_t c = 0; c < a.getColumnCount(); c++) {
      sum += arg2(a(r,c));
    }
  }
  return sqrt(sum);
}

ComplexMatrix inverse(const ComplexMatrix &a) {
  _VALIDATEISSQUAREMATRIX(a);
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
  const size_t rows    = lts.getRowCount();
  const size_t columns = lts.getColumnCount();

  if(columns != rhs.getDimension()) {
    throwException(_T("operator*(Matrix,ComplexVector):Invalid dimension. Matrix.%s, Vector.dimension = %s."), lts.getDimensionString().cstr(), format1000(rhs.getDimension()).cstr());
  }
  ComplexVector result(rows);
  for(size_t r = 0; r < rows; r++) {
	  Complex sum = 0;
    for(size_t c = 0; c < columns; c++) {
      sum += lts(r,c) * rhs[c];
    }
	  result[r] = sum;
  }
  return result;
}

ComplexMatrix operator*(const Complex &lts, const Matrix &rhs) {
  const size_t rows    = rhs.getRowCount();
  const size_t columns = rhs.getColumnCount();

  ComplexMatrix result(rows,columns);
  for(size_t r = 0; r < rows; r++) {
    for(size_t c = 0; c < columns; c++) {
      result(r,c) = lts * rhs(r,c);
    }
  }
  return result;
}
