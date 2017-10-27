/*
 QRMatrix.cpp:Implements the implicit shifted QR-algorithm as described in
 "Computer Science and Applied Mathematics"
*/
#include "pch.h"
#include <Random.h>
#include <Math/Matrix.h>

using namespace std;

//#define DEBUGMODULE 1

#ifdef DEBUGMODULE
bool traceQR = false;
static StreamParameters param(9,14,ios::fixed);
#endif

// -------------------------------- Define Vector3 ---------------------------------------------------------------------

typedef Real Vector3[3];

tostream& operator<<(tostream &out, const Vector3 &v) {
  StreamParameters param(out);
  return out << "(" << param << v[0] << "," << param << v[1] << "," << param << v[2] << ")";
};

Real length(const Vector3 &v) {
  return sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
}

// -------------------------------- Define ElementaryReflector ---------------------------------------------------------------------

class ElementaryReflector {
private:
  Vector3 v,w;
  Real sigma;
  void init(const Vector &V);
  Matrix getMatrix() const;
  void check(const Vector &V) const;
public:
  ElementaryReflector() {};
  ElementaryReflector(const Matrix   &A, int row, int column);
  ElementaryReflector(const QRMatrix &A);
  void preMultiply(         Matrix   &A, int row, int column) const;
  void postMultiply(        Matrix   &A, int row, int column) const;
  Real getSigma() const { return sigma; }
  friend tostream &operator<<(tostream &out, const ElementaryReflector &r);
};

ElementaryReflector::ElementaryReflector(const QRMatrix &A) {
  const int n = A.getDeflatedSize();

  Vector V(3);
  V[0] = ((A(n-1,n-1) - A(0,0))*(A(n-2,n-2)-A(0,0)) - A(n-1,n-2)*A(n-2,n-1))/A(1,0) + A(0,1);
  V[1] = A(1,1) + A(0,0) - A(n-1,n-1) - A(n-2,n-2);
  V[2] = A(2,1);

  init(V);
}

// Algorithm 4.4 Page 377
ElementaryReflector::ElementaryReflector(const Matrix &A, int row, int column) {
  Vector V(3);
  for(int i = 0; i < 3; i++) {
    V[i] = A(row+i,column);
  }
  init(V);
}

void ElementaryReflector::preMultiply(Matrix &A, int row, int column) const {
  Real &a1 = A(row  , column);
  Real &a2 = A(row+1, column);
  Real &a3 = A(row+2, column);

  const Real tau = a1 + w[1]*a2 + w[2]*a3;
  a1 -= tau * v[0];
  a2 -= tau * v[1];
  a3 -= tau * v[2];
}

void ElementaryReflector::postMultiply(Matrix &A, int row, int column) const {
  Real &a1 = A(row, column  );
  Real &a2 = A(row, column+1);
  Real &a3 = A(row, column+2);

  const Real tau = a1 + w[1]*a2 + w[2]*a3;
  a1 -= tau * v[0];
  a2 -= tau * v[1];
  a3 -= tau * v[2];
}

static tostream &operator<<(tostream &out, const ElementaryReflector &r) {
  StreamParameters param(out);
  return out << "ElementaryReflector:(v,w,sigma):(" << param << r.v << "," << param << r.w << "," << param << r.sigma << ")" << endl;
}

static double biasedSign(const Real &x) {
  return x >= 0 ? 1 : -1;
}

void ElementaryReflector::init(const Vector &V) {
  const Real V0 = fabs(V[0]);
  const Real V1 = fabs(V[1]);
  const Real V2 = fabs(V[2]);

  Real ny = dmax(V0,dmax(V1,V2));
  if(ny == 0) {
    v[0] = v[1] = v[2] = 0;
    w[0] = w[1] = w[2] = 0;
    sigma = 0;
  } else {
    v[0] = V[0] / ny;
    v[1] = V[1] / ny;
    v[2] = V[2] / ny;
    sigma = biasedSign(v[0]) * length(v);
    v[0] += sigma;
    w[0] = 1;  // v[0] / v[0]
    w[1] = v[1] / v[0];
    w[2] = v[2] / v[0];
    v[0] /= sigma;
    v[1] /= sigma;
    v[2] /= sigma;
    sigma *= ny;
  }
}

Matrix ElementaryReflector::getMatrix() const {
  Matrix result(Matrix::one(3));
  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 3; j++) {
      result(i,j) -= v[i]*w[j];
    }
  }
  return result;
}

void ElementaryReflector::check(const Vector &V) const {
  tcout << "reflect(" << V << "):\n";
  tcout << "  v=(" << v << ")" << endl;
  tcout << "  w=(" << w << ")" << endl;

  tcout << "reflect(a)=(" << getMatrix()*V << ")";
  tcout << ". Must be (" << -sigma << ",0,0)" << endl;
  tcout.flush();
}

// -------------------------------- Define PlaneRotation ---------------------------------------------------------------------

class PlaneRotation {
private:
  Real gamma, sigma, vs;

  Matrix getMatrix() const;
  void check(const Vector &v) const;
public:
  PlaneRotation() {};
  PlaneRotation(    Matrix &A, int row, int column); // NB. Modifies A
  void preMultiply( Matrix &A, int row, int column) const;
  void postMultiply(Matrix &A, int row, int column) const;

  friend tostream &operator<<(tostream &out, const PlaneRotation &r);
};

// Algorithm 3.4 page 357
PlaneRotation::PlaneRotation(Matrix &A, int row, int column) {
  Real      &a    = A(row  , column);
  Real      &b    = A(row+1, column);
  const Real absA = fabs(a);
  const Real absB = fabs(b);
  const Real ny = dmax(absA, absB);
  if(ny == 0) {
    gamma = sigma = vs = 0;
  } else {
    const Real a1 = a / ny;
    const Real b1 = b / ny;
    const Real delta = sqrt(sqr(a1) + sqr(b1));
    gamma = a1 / delta;
    sigma = b1 / delta;
    vs    = ny * delta;
  }

  a = vs;
  b = 0;
}

void PlaneRotation ::preMultiply(Matrix &A, int row, int column) const {
  Real &m1 = A(row  , column);
  Real &m2 = A(row+1, column);

  Real r1  =  gamma*m1 + sigma*m2;
  Real r2  = -sigma*m1 + gamma*m2;
  m1       = r1;
  m2       = r2;
}

void PlaneRotation::postMultiply(Matrix &A, int row, int column) const {
  Real &m1 = A(row, column  );
  Real &m2 = A(row, column+1);

  const Real r1  =  m1*gamma + m2*sigma;
  const Real r2  = -m1*sigma + m2*gamma;
  m1       = r1;
  m2       = r2;
}

static tostream &operator<<(tostream &out, const PlaneRotation &r) {
  StreamParameters param(out);
  return out << _T("PlaneRotation:(gamma,sigma,vs):(") << param << r.gamma << "," << param << r.sigma << "," << param << r.vs << ")" << endl;
}

Matrix PlaneRotation::getMatrix() const {
  Matrix result(2, 2);
  result(0,0) = result(1, 1) = gamma;
  result(1,0) = -sigma;
  result(0,1) = sigma;
  return result;
}

void PlaneRotation::check(const Vector &v) const {
  tcout << "Rotation(" << v << ") = (" << gamma << "," << sigma << "," << vs << ")" << endl;
  tcout << "Rotation(v) = (" << getMatrix() * v << ")";
  tcout << ". Must be (" << vs << ",0)" << endl;
  tcout << ". sqr(gamma)+sqr(sigma)=" << sqr(gamma)+sqr(sigma) << ". Must be = 1" << endl;
  tcout.flush();
}

// --------------------------------End of PlaneRotation ---------------------------------------------------------------------

QRMatrix::QRMatrix(QRTracer *tracer) {
  m_tracer       = tracer;
  m_deflatedSize = (int)getRowCount();
}

QRMatrix::QRMatrix(const Matrix &src, QRTracer *tracer)
  : Matrix(src)
  , m_pi(src.getRowCount())
  , m_u0(src.getRowCount())
  , m_eigenValues(src.getRowCount())
  , m_eigenVectors(src.getRowCount(),src.getColumnCount())
 {
  if(!src.isSquare()) {
    throwMathException(_T("QRMatrix::QRMatrix:Matrix not square. Dimension = (%d,%d)."), src.getRowCount(), src.getColumnCount());
  }

  m_tracer       = tracer;
  m_deflatedSize = (int)getRowCount();
  solve();
}

QRMatrix &QRMatrix::operator=(const Matrix &src) {
  if(this == &src) {
    return *this;
  }

  if(!src.isSquare()) {
    throwMathException(_T("QRMatrix::operator=:Matrix not square. Dimension = (%d,%d)."), src.getRowCount(), src.getColumnCount());
  }

  ((Matrix&)(*this)) = src;
  const int n = (int)getRowCount();
  m_pi.setDimension(n);
  m_u0.setDimension(n);
  m_eigenValues.setDimension(n);
  m_eigenVectors.setDimension(n,n);
  m_deflatedSize = n;
  solve();
  return *this;
}

// Checks that inverse(Q) = transpose(Q)
static void checkIsUnitary(TCHAR *name, const Matrix &A) {
  Matrix Ainverse      = inverse(A);
  Matrix Atranspose    = transpose(A);
  Matrix difference    = Ainverse - Atranspose;
  String inverseName   = format(_T("inverse(%s)"),name);
  String transposeName = format(_T("transpose(%s)"),name);
  Real diff = normf(difference);
  if(diff > 1e-13) {
    tcout << name << " not unitary\n";
    tcout << "FrobeniusNorm(" << inverseName << "-" << transposeName << "):" << diff << endl;
    tcout << name          << ":\n" << A;
    tcout << inverseName   << ":\n" << Ainverse;
    tcout << transposeName << ":\n" << Atranspose;
    tcout << inverseName << "-" << transposeName << ":\n" << difference;
  }
}

void QRMatrix::solve() {
  reduceToHessenberg();
  createUMatrix();
  resetToHessenberg();
  findEigenValues();
  findEigenVectors();
}

// Algorithm 1.1 page 332
void QRMatrix::reduceToHessenberg() {
  QRMatrix &a = *this;
  const int n = (int)a.getRowCount();

  for(int k = 0; k < n-2; k++) {
                                                      // 1 determine the transformation
    Real ny = 0;                                      // 1.1
    for(int i = k+1; i < n; i++) {
      Real e = fabs(a(i,k));
      if(e > ny)
        ny = e;
    }
    if(ny == 0) {                                     // 1.2
      m_pi[k] = 0;
      trace();
      continue;
    }
    for(int i = k+1; i < n; i++) {                        // 1.3
      a(i,k) /= ny;
    }

    Real sum = 0;                                     // 1.4
    for(int i = k+1; i < n; i++) {
      sum += sqr(a(i,k));
    }
    Real sigma = sign(a(k+1,k)) * sqrt(sum);

    a(k+1,k) += sigma;                                // 1.5
    m_pi[k]   = sigma * a(k+1,k);                     // 1.6

                                                      // 2 premultiply
    for(int j = k+1; j < n; j++) {                    // 2.1
      sum = 0;
      for(int i = k+1; i < n; i++) {             // 2.1.1
        sum += a(i,k) * a(i,j);
      }
      Real ro = sum / m_pi[k];
      for(int i = k+1; i < n; i++) {                      // 2.1.2
        a(i,j) -= ro * a(i,k);
      }
    }
                                                      // 3 postmultiply
    for(int i = 0; i < n; i++) {                          // 3.1
      sum = 0;
      for(int j = k+1; j < n; j++) {             // 3.1.1
        sum += a(i,j) * a(j,k);
      }
      Real ro = sum / m_pi[k];

      for(int j = k+1; j < n; j++) {                      // 3.1.2
        a(i,j) -= ro * a(j,k);
      }
    }
                                                      // 4 Compute a(k+1,k) and save v(k+1,k)
    m_u0[k]  = a(k+1,k);                              // 4.1
    a(k+1,k) = -ny * sigma;                           // 4.2

    trace();
  }
}

// set all elements below subdiagonal to 0
void QRMatrix::resetToHessenberg() {
  QRMatrix &a = *this;
  const int n = (int)getRowCount();
  for(int j = 0; j < n; j++) {
    for(int i = j+2; i < n; i++) {
      a(i,j) = 0;
    }
  }
}

#define EPS 1e-14

void QRMatrix::findEigenValues() {
  Complex l1, l2;
  m_Q = Matrix::one(getRowCount());

  trace();
  int iteration = 0; // Number of iterations since last deflation

  while(m_deflatedSize > 2) {
    if(fabs(subDiagonal(m_deflatedSize-1)) < EPS) {
      // Found 1 (real) eigenvalues = diagonalelement(m_deflatedSize-1). Deflate Matrix by 1
      l1 = getEigenValue1x1();
      m_eigenValues[--m_deflatedSize] = l1;
      iteration = 0;
      trace();
    } else if(fabs(subDiagonal(m_deflatedSize-2)) < EPS) {
      // Found 2 eigenvalues of 2x2 submatrix(m_deflatedSize-2,m_deflatedSize-2) (conjugated if complex). Deflate Matrix by 2
      getEigenValues2x2(l1,l2);
      if(l1.im == 0) { // if not complex, do singleQRSteps until subdiagonal(deflatedSize-1) < EPS and deflate only by 1
        Real shift = fabs(l1.re) > fabs(l2.re) ? l1.re : l2.re;
        while(fabs(subDiagonal(m_deflatedSize-1)) >= EPS) {
          singleQRStep(iteration, shift);
          iteration++;
        }
        l1 = getEigenValue1x1();
        m_eigenValues[--m_deflatedSize] = l1;
        iteration = 0;
        trace();
      } else {
        m_eigenValues[--m_deflatedSize] = l1;
        m_eigenValues[--m_deflatedSize] = l2;
        iteration = 0;
        trace();
      }
    } else {
      doubleQRStep(iteration);
      iteration++;
      if(iteration > 10) { // If no convergence for doubleQRStep, Make af singleQRStep with a random shift
        singleQRStep(iteration, randReal(-10,10));
        iteration = 0;
      }
      trace();
    }
  }
  trace();
  if(m_deflatedSize == 1) {
    l1 = getEigenValue1x1();
    m_eigenValues[--m_deflatedSize] = l1;
  } else { // m_deflatedSize == 2
    if(fabs(subDiagonal(m_deflatedSize-1)) < EPS) {
      l1 = getEigenValue1x1();
      m_eigenValues[--m_deflatedSize] = l1;
      l1 = getEigenValue1x1();
      m_eigenValues[--m_deflatedSize] = l1;
    } else {
      getEigenValues2x2(l1,l2);
      m_eigenValues[--m_deflatedSize] = l1;
      m_eigenValues[--m_deflatedSize] = l2;
    }
  }
  trace();
}

static String genUProductString(int k) {
  String result;
  for(int p = 0; p <= k; p++) {
    if(p > 0) {
      result += "*";
    }
    result += format(_T("U[%d]"),p);
  }
  return result + _T("\n");
}

Vector QRMatrix::getUk(int k) const {
  const QRMatrix &a = *this;
  const int       n = (int)getRowCount();

  Vector u(n-k-1);
  u[0] = m_u0[k];
  for(int i = k+2; i < n; i++) {
    u[i-k-1] = a(i,k);
  }
  return u;
}

void QRMatrix::createUMatrix() {
  const int n = (int)getRowCount();

  m_U = Matrix::one(n);

//  cout << "QR:\n" << Precision(7) << a << endl;
  for(int k = 0; k < n-2; k++) {
    Matrix u(n-k-1,1);
    u.setColumn(0,getUk(k));
    Real pi = m_pi[k];
    if(pi == 0) {
      continue;
    }
    Matrix Rk = Matrix::one(n-k-1) - u*transpose(u)/pi;
//    cout << "R[" << k << "]:\n" << Precision(7) << Rk << endl;

    Matrix Uk = Matrix::one(n).setSubMatrix(k+1,k+1,Rk);
//    cout << "U[" << k << "]:\n" << Precision(7) << Uk << endl;

    m_U = m_U * Uk;
//    cout << genUProductString(k) << Precision(7) << U << endl;
  }
}

// Algorithm 3.5 Page 360
void QRMatrix::singleQRStep(int iteration, const Real &shift) {
  QRMatrix &a = *this;
  const int n = (int)a.getRowCount();

  checkIsUnitary(_T("Q"), m_Q);
#if DEBUGMODULE>0
if(traceQR) {
  cout << "*********************** Begin of singleQRStep(iteration=" << iteration << ", shift=" << shift << ", deflated Size=" << m_deflatedSize << ") ************************" << endl;
  cout << "QR:\n" << param << a;
}
#endif

  a(0,0) -= shift;
  PlaneRotation rotation, lastRotation;
  for(int k = 0; k < m_deflatedSize; k++) {
    if(k < m_deflatedSize-1) { // Premultiply by P(k,k+1)
      rotation = PlaneRotation(a,k,k);
      a(k+1,k+1) -= shift;
      for(int j = k+1; j < n; j++) {
        rotation.preMultiply(a,k,j);
      }
    }
    if(k > 0) { // Postmultiply by transpose(P(k-1,k))
      for(int i = 0; i < n; i++) {
        lastRotation.postMultiply(a  ,i,k-1);
        lastRotation.postMultiply(m_Q,i,k-1);
      }
      a(k-1,k-1) += shift;
    }
    lastRotation = rotation;
  }
  a(m_deflatedSize-1,m_deflatedSize-1) += shift;

#if DEBUGMODULE>0
if(traceQR) {
  cout << "QR(end, deflated Size=" << m_deflatedSize << "):" << endl;
  cout << param << a;
  cout << "*********************** End of singleQRStep(" << iteration << ") ************************" << endl;
}
#endif
  checkIsUnitary(_T("Q"), m_Q);
}

// Algorithm 4.5 Page 378
void QRMatrix ::doubleQRStep(int iteration) {
  QRMatrix &a = *this;

#if DEBUGMODULE>0
if(traceQR) {
  cout << "*********************** Begin of doubleQRStep(iteration=" << iteration << ", deflated Size=" << m_deflatedSize << ") ************************" << endl;
  cout << "QR:\n" << param << a;
}
#endif

  for(int k = -1; k < m_deflatedSize - 3; k++) {

    const ElementaryReflector reflector = (k==-1) ? ElementaryReflector(a) : ElementaryReflector(a,k+1,k);

#if DEBUGMODULE>1
if(traceQR) {
    cout << "k=" << k << " - " << reflector << endl;
}
#endif

    if(k != -1) {
      a(k+1,k) = -reflector.getSigma();
      a(k+2,k) = 0;
      a(k+3,k) = 0;
    }

    for(int j = k+1; j < (int)getColumnCount(); j++) { // 3 Premultiply
      reflector.preMultiply(a,k+1,j);

#if DEBUGMODULE>1
if(traceQR) {
      cout << "After reflector.preMultiply(j=" << j << "):\n" << param << a;
}
#endif

    }
    for(int i = 0; i < (int)getRowCount(); i++) { // 4 Postmultiply
      reflector.postMultiply(a  ,i,k+1);
      reflector.postMultiply(m_Q,i,k+1);

#if DEBUGMODULE>1
if(traceQR) {
      cout << "After reflector.postMultiply(i=" << i << "):\n" << param << a;
}
#endif
    }

#if DEBUGMODULE>0
if(traceQR) {
      cout << "QR(k=" << k << ", deflated Size=" << m_deflatedSize << "):\n" << param << a;
}
#endif

  }

  const PlaneRotation rotation(a,m_deflatedSize-2,m_deflatedSize-3);
#if DEBUGMODULE>1
if(traceQR) {
  cout << rotation << endl;
}
#endif
  for(int j = m_deflatedSize-2; j < (int)getColumnCount(); j++) {
    rotation.preMultiply(a,m_deflatedSize-2,j);
#if DEBUGMODULE>1
if(traceQR) {
    cout << "After rotation.preMultiply(j=" << j << "):\n" << param << a;
}
#endif
  }

  for(int i = 0; i < (int)getRowCount(); i++) {
    rotation.postMultiply(a  ,i,m_deflatedSize-2);
    rotation.postMultiply(m_Q,i,m_deflatedSize-2);
#if DEBUGMODULE>1
if(traceQR) {
    cout << "After rotation.postMultiply(i=" << i << "):\n" << param << a;
}
#endif
  }

#if DEBUGMODULE>0
if(traceQR) {
  cout << "QR(end, deflated Size=" << m_deflatedSize << "):" << endl;
  cout << param << a;
  cout << "*********************** End of doubleQRStep(" << iteration << ") ************************" << endl;
}
#endif

}

void QRMatrix::createComplexMatrix(ComplexMatrix &M) const {
  const QRMatrix &a = *this;
  const int       n = (int)getRowCount();
  M.setDimension(n,n);
  for(int r = 0; r < n; r++) {
    for(int c = 0; c < n; c++) {
      M(r,c) = a(r,c);
    }
  }
}

void QRMatrix::setDiagonalElements(ComplexMatrix &M, const Complex &lambda) const {
  const QRMatrix &a = *this;
  const int       n = (int)getRowCount();
  for(int i = 0; i < n; i++) {
    M(i,i) = a(i,i) - lambda;
  }
}

void QRMatrix::findEigenVectors() {
  const int n = (int)getRowCount();

//  cout << "QR:\n" << *this;
//  cout << "EigenValues:" << m_eigenValues << endl;

  ComplexMatrix M;
  createComplexMatrix(M);
  Matrix UQ = m_U * m_Q;

//  cout << "U :\n" << m_U;
//  cout << "Q :\n" << m_Q;
//  cout << "UQ:\n" << UQ;

  for(int k = 0; k < n;) {
    int blockSize = 1; // Size of diagonalBlock belonging to eigenvalue[k]. (= 1 or 2)
    if(k < n-1 && arg(M.subDiagonal(k+1)) > EPS) { // We have a 2x2 submatrix for this eigenvalue
      blockSize = 2;
    }

    for(int e = 0; e < blockSize; e++) {
      const Complex &lambda = m_eigenValues[k];
      setDiagonalElements(M,lambda); // M is quasi triangular

//  cout << "M:\n" << prec << M << "lambda(" << k << "):" << lambda << endl;

      ComplexVector y(n); // eigenvector for M associated to lambda
      y[k] = Complex(1);  // (y[k] = 1, y[k+blockSize]..y[n-1]) = 0)
      int kmin = k;
      int kmax = k;
      if(blockSize == 2) { // Calculate the neighbor component of y[k] (y[k+1] or y[k-1])
        if(e == 0) {
          y[k+1] = -M(k,k) / M(k,k+1);
          kmin = k;
        } else {
          y[k-1] = -M(k,k) / M(k,k-1);
          kmin = k-1;
        }
        kmax = kmin+1;
      }
      for(int j = kmin-1; j >= 0;) { // Calculate y[kmin-1]..y[0]
        if(j == 0 || arg(M.subDiagonal(j)) < EPS) {
          Complex sum = 0; // We have a simple 1x1  diagonal-element. Find y[j] directly.
          for(int s = j+1; s <= kmax; s++) {
            sum += M(j,s) * y[s];
          }
          if(arg(M(j,j)) != 0) {
            y[j] = -sum / M(j,j);
          } else {
            if(arg(sum) > EPS) {
              throwException(_T("Cannot find eigenvector for eigenvalue = %s"), ::toString(lambda).cstr());
            } else {
              y[j] = Complex(randDouble(-1,1));
            }
          }
          j--;
        } else {           // We have a 2x2 diagonal-block. Solve 2x2 linear equation to get y[j-1],y[j]
          j--;             // Decrement j so that the 2x2 diagonal-block has upper left corner at M[j][j]
          ComplexVector V2(2);
          for(int i = 0; i < 2; i++) {
            Complex sum = 0;
            for(int s = j+2; s <= kmax; s++) {
              sum += M(j+i,s) * y[s];
            }
            V2[i] = -sum;
          }
          ComplexLUMatrix D22 = M.getSubMatrix(j,j,2,2);
          ComplexVector tmp = D22.solve(V2);
          y[j]   = tmp[0];
          y[j+1] = tmp[1];
          j--;
        }
      } // for(j..
      y /= fabs(y);
      m_eigenVectors.setColumn(k++, UQ * y);
//      cout << "Test(k=" << (k-1) << ",lambda=" << lambda << "): length(M*y):" << (M*y).length() << endl;
    } // for(e...
  } // for(k...
}

// Assumes a.dimension = (2,2)
static void getEigenValues2x2(const Matrix &a, Complex &l1, Complex &l2) {
  const Real B = a(0,0)+a(1,1);
  const Real C = a(0,0)*a(1,1) - a(1,0)*a(0,1);
  const Real D = sqr(B) - 4*C;
  if(D >= 0) {
    l1 = (B + sqrt(D)) / 2;
    l2 = (B - sqrt(D)) / 2;
  } else {
    l1 = Complex(B/2, sqrt(-D)/2);
    l2 = conjugate(l1);
  }
}

void QRMatrix::getEigenValues2x2(Complex &l1, Complex &l2) const {
  const int offset = m_deflatedSize-2;
  ::getEigenValues2x2(getSubMatrix(offset,offset,2,2),l1,l2);

#if DEBUGMODULE>0
if(traceQR) {
  StreamParameters ss(15,0,ios::scientific);
  cout << "Found nr. " << m_deflatedSize << " and " << (m_deflatedSize-1) << ":" << ss << l1 << "," << ss << l2;
  if(offset > 0) {
    cout << " SubDiag(" << offset << ") = " << ss << subDiagonal(offset) << endl;
  } else {
    cout << endl;
  }
}
#endif
}

Real QRMatrix::getEigenValue1x1() const {
  const QRMatrix &a      = *this;
  const int       offset = m_deflatedSize-1;
  const Real      lambda = a(offset,offset);

#if DEBUGMODULE>0
if(traceQR) {
  StreamParameters ss(15,0,ios::scientific);
  cout << "Found nr. " << m_deflatedSize << ":" << ss << lambda;
  if(offset > 0) {
    cout << " SubDiag(" << offset << ") = " << ss << subDiagonal(offset) << endl;
  } else {
    cout << endl;
  }
}
#endif

  return lambda;
}

void QRMatrix::trace() {
  if(m_tracer != NULL) {
    m_tracer->handleData(QRTraceElement(*this));
  }
}
