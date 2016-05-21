#pragma once

#include <MyString.h>
#include <MatrixTemplate.h>
#include "Complex.h"
#include "LUMatrixTemplate.h"
#include "AlgorithmHandler.h"

typedef VectorTemplate<Real>      Vector;
typedef MatrixTemplate<Real>      Matrix;
typedef LUMatrixTemplate<Real>    LUMatrix;

typedef VectorTemplate<Complex>   ComplexVector;

class ComplexMatrix : public MatrixTemplate<Complex> {
public:
  ComplexMatrix();
  explicit ComplexMatrix(Complex c);
  ComplexMatrix(unsigned int rows, unsigned int columns);

  ComplexMatrix(const MatrixTemplate<Complex> &src);
  explicit ComplexMatrix(const VectorTemplate<Complex> &diagonal);

  ComplexMatrix(const Matrix &src);
  ComplexMatrix &operator=(const Matrix &src);
  explicit ComplexMatrix(const Vector &diagonal);

  Matrix getRealPart() const;
  Matrix getImaginaryPart() const;

  friend String toString(const ComplexMatrix &a) {
    return a.toString();
  }
};

typedef LUMatrixTemplate<Complex> ComplexLUMatrix;

class QRMatrix;

class QRTraceElement {
public:
  const QRMatrix &m_QR;
  QRTraceElement(const QRMatrix &QR) : m_QR(QR) {
  }
};

typedef AlgorithmHandler<QRTraceElement> QRTracer;

Real normf(    const Matrix &a); // Frobenius norm
Real norm1(    const Matrix &a);
Real norm00(   const Matrix &a);
Real fabs(     const Vector &v); // v.length()
Real fabs(     const Matrix &a); // =normf(a)
Real fabs(     const ComplexVector &v);
Real normf(    const ComplexMatrix &a);
Real fabs(     const ComplexMatrix &a);

Matrix        inverse(const Matrix        &a);
ComplexMatrix inverse(const ComplexMatrix &a);
Real          det(    const Matrix        &a);
Complex       det(    const ComplexMatrix &a);
ComplexVector operator*(const Matrix  &lts, const ComplexVector &rhs);
ComplexMatrix operator*(const Complex &lts, const Matrix        &rhs);

void setToRandom(Vector        &v);
void setToRandom(Matrix        &a);
void setToRandom(ComplexVector &v);
void setToRandom(ComplexMatrix &a);

// superdiag = (c0=a(0,1),c1=a(1,2),c2=a(2,3)...,0)
// subdiag   = (0,c1=a(1,0),c2=a(2,1),c3=a(3,2)...)
Vector solve3diag(const Vector &superdiag, const Vector &diag, const Vector &subdiag, const Vector &d);

class QRMatrix : public Matrix {
private:
  int           m_deflatedSize;
  Matrix        m_U; // inverse(U) == transpose(U)
  Matrix        m_Q;
  Vector        m_pi;
  Vector        m_u0;
  ComplexVector m_eigenValues;
  ComplexMatrix m_eigenVectors;
  QRTracer     *m_tracer;

  void solve();
  void findEigenValues();
  void reduceToHessenberg();
  void doubleQRStep(int iteration);
  void singleQRStep(int iteration, const Real &shift);
  void resetToHessenberg();
  void createComplexMatrix(ComplexMatrix &M) const;
  void setDiagonalElements(ComplexMatrix &M, const Complex &lambda) const;
  void findEigenVectors();
  void getEigenValues2x2(Complex &l1, Complex &l2) const;
  Real getEigenValue1x1() const;
  Vector getUk(int k) const;
  void createUMatrix();
  void trace();

  QRMatrix(const QRMatrix &src);            // not defined
  QRMatrix &operator=(const QRMatrix &src); // not defined
  void setDimension(int rows, int columns); // not defined
public:
  QRMatrix(QRTracer *tracer = NULL);
  QRMatrix(const Matrix &a, QRTracer *tracer = NULL);
  QRMatrix &operator=(const Matrix &a);

  const Matrix &getUMatrix() const {
    return m_U;
  }

  const Matrix &getQMatrix() const {
    return m_Q;
  }
  
  int getDeflatedSize() const {
    return m_deflatedSize;
  }
  
  const ComplexVector &getEigenValues() const {
    return m_eigenValues;
  }

  const ComplexMatrix &getEigenVectors() const {
    return m_eigenVectors;
  }

  void setTracer(QRTracer *tracer) {
    m_tracer = tracer;
  }

  QRTracer *setTracer() {
    return m_tracer;
  }
};

ComplexVector findEigenValues(const Matrix &a, QRTracer *tracer=NULL);

class SVDDecomposition {                           // Singular Value Decomposition
private:
  DECLARECLASSNAME;
public:
  Matrix m_u,m_v;
  Vector m_d;                                      // Diagonal matrix is represented as a vector
  SVDDecomposition(const Matrix &a);               // Calculate unitary Matrix u[r,c], diagonal Matrix d[c,c], and unitary Matrix v[c,c]
                                                   // so a = u * d * transpose(v)
  Vector solve(const Vector &b) const;             // b must have dimension r. Returns Vector x with dimension c, so u*d*transpose(v)*x = b.
};

class ComplexSVDDecomposition {                    // Singular Value Decomposition
public:
  ComplexMatrix u,v;
  ComplexVector d;                                 // Diagonal matrix is represented as a vector
  ComplexSVDDecomposition(const ComplexMatrix &a); // Calculate unitary Matrix u[r,c], diagonal Matrix d[c,c], and unitary Matrix v[c,c]
                                                   // so a = u * d * transpose(v)
  ComplexVector solve(const ComplexVector &b);     // b must have dimension r. Returns Vector x with dimension c, so u*d*transpose(v)*x = b.
};

