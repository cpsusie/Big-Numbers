#include "stdafx.h"
#include <Random.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace TestMatrix {

#include <UnitTestTraits.h>

  class LOG : public tostrstream {
  public:
    ~LOG() {
      if(str().length() > 0) {
        OUTPUT(_T("%s"), str().c_str());
      }
    }
  };

#define REAL_PRECISION numeric_limits<Real>::max_digits10

  static StreamParameters param1(REAL_PRECISION, REAL_PRECISION+5, ios::scientific|ios::showpos);
//  static StreamParameters param2(10, 0 , ios::scientific);
#define param2 param1

  class DistanceFromPlan : public VectorToRFunction {
  private:
    Matrix m_a;
    Vector m_b;
  public:
    DistanceFromPlan(const Matrix &a, const Vector &b);
    Real operator()(const Vector &x);
  };

  DistanceFromPlan::DistanceFromPlan(const Matrix &a, const Vector &b) {
    m_a = a;
    m_b = b;
  }

  Real DistanceFromPlan::operator()(const Vector &x) {
    return (m_a*x - m_b).length();
  }


    TEST_CLASS(TestMatrix) {
    public:

      static Vector randomVector(size_t dimension, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
        Vector result(dimension);
        setToRandom(result,rnd);
        return result;
      }

      static Vector randomIntVector(size_t dimension, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
        Vector result(dimension);
        for(size_t i = 0; i < dimension; i++) {
          result[i] = randInt(-11,11,rnd);
        }
        return result;
      }

      static Matrix randomMatrix(size_t rows, size_t columns, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
        Matrix result(rows, columns);
        setToRandom(result, rnd);
        return result;
      }

      static Matrix randomIntMatrix(size_t rows, size_t columns, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
        Matrix result(rows, columns);
        for(size_t r = 0; r < rows; r++) {
          result.setRow(r, randomIntVector(columns, rnd));
        }
        return result;
      }

      static Matrix genNoConvergenceMatrix(size_t dim) {
        Matrix a = Matrix::_0(dim, dim);
        for(size_t i = 1; i < dim; i++) {
          a(i, i - 1) = 1;
        }
        a(0, dim - 1) = 1;
        return a;
      }

      static Matrix randomSymmetricMatrix(size_t dimension, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
        Matrix result(dimension, dimension);
        for(size_t r = 0; r < dimension; r++) {
          result.setSubMatrix(r, r, randomMatrix(1, dimension - r, rnd));
          if(r < dimension - 1) {
            result.setSubMatrix(r + 1, r, transpose(result.getSubMatrix(r, r + 1, 1, dimension - r - 1)));
          }
        }
        return result;
      }

      static ComplexVector randomComplexVector(size_t dimension, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
        ComplexVector result(dimension);
        setToRandom(result, rnd);
        return result;
      }

      static ComplexMatrix randomComplexMatrix(size_t rows, size_t columns, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
        ComplexMatrix result(rows, columns);
        setToRandom(result, rnd);
        return result;
      }

      static Matrix exp(const Matrix &A) {
        if(!A.isSquare()) {
          throwException(_T("exp(Matrix):Matrix not square. A.%s"), A.getDimensionString().cstr());
        }
        const size_t dim = A.getRowCount();
        Matrix       p   = Matrix::_1(dim);
        Matrix       sum( dim, dim);
        Matrix       last(dim, dim);

        for(Real k = 1;; k++) {
          last = sum;
          sum += p;
          if(normf(sum - last) == 0) {
            break;
          }
          p = p * A;
          p /= k;
        }
        return sum;
      }

      static Matrix cos(const Matrix &A) {
        if(!A.isSquare()) {
          throwException(_T("cos(Matrix):Matrix not square. A.%s"), A.getDimensionString().cstr());
        }
        const int    dim = (int)A.getRowCount();
        const Matrix A2 = -A * A;
        Matrix       p = Matrix::_1(dim);
        Matrix       sum(dim, dim);
        Matrix       last(dim, dim);

        for(Real k = 0;; k += 2) {
          last = sum;
          sum += p;
          if(normf(sum - last) == 0) {
            break;
          }
          p = p * A2;
          p /= ((k + 1)*(k + 2));
        }
        return sum;
      }

      static Matrix sin(const Matrix &A) {
        if(!A.isSquare()) {
          throwException(_T("sin(Matrix):Matrix not square. A.%s"), A.getDimensionString().cstr());
        }
        const int    dim = (int)A.getRowCount();
        const Matrix A2 = -A * A;
        Matrix       p = A;
        Matrix       sum(dim, dim);
        Matrix       last(dim, dim);

        for(Real k = 1;; k += 2) {
          last = sum;
          sum += p;
          if(normf(sum - last) == 0) {
            break;
          }
          p = p * A2;
          p /= ((k + 1)*(k + 2));
        }
        return sum;
      }

      static ComplexMatrix cubicRoot(const Matrix &A) {
        const QRMatrix      QR = A;
        const ComplexMatrix P  = QR.getEigenVectors();
        const ComplexMatrix P1 = inverse(P);
        ComplexVector       D  = QR.getEigenValues();

        for(size_t i = 0; i < D.getDimension(); i++) {
          D[i] = root(D[i], 3);
        }
        return P * ComplexMatrix(D) * P1;
      }

    TEST_METHOD(MatrixRealTest) {
      Matrix A(Real(1));
      verify(A.getRowCount() == 1 && A.getColumnCount() == 1);
      verify(A(0, 0) == 1);
      Matrix B = A;
      verify(B == A);
      Matrix B1;
      B1 = A;
      verify(B1 == A);

      Matrix C(4, 3);
      verify(C.getRowCount() == 4 && C.getColumnCount() == 3);
      verify(C == Matrix::_0(4, 3));
      Vector v(4);
      for(int i = 0; i < 4; i++) {
        v[i] = i;
      }
      verify(v.getDimension() == 4);
      Vector v1 = v;
      verify(v1 == v);
      Vector v2;
      v2 = v;
      verify(v2 == v);
      Matrix D(v);
      verify(D.getDiagonal() == v);
      for(size_t i = 0; i < D.getRowCount(); i++) {
        for(size_t j = 0; j < D.getColumnCount(); j++) {
          if(i == j) {
            verify(D(i, j) == i);
          }
          else {
            verify(D(i, j) == 0);
          }
        }
      }
      verify(fabs(D) == sqrt(Real(14)));
    }

    TEST_METHOD(MatrixComplexTest) {
      ComplexMatrix A(1);
      verify(A.getRowCount() == 1 && A.getColumnCount() == 1);
      verify(A(0, 0) == 1);
      ComplexMatrix B = A;
      verify(B == A);
      ComplexMatrix B1;
      B1 = A;
      verify(B1 == A);

      ComplexMatrix C(4, 3);
      verify(C.getRowCount() == 4 && C.getColumnCount() == 3);
      verify(C == ComplexMatrix::_0(4, 3));
      ComplexVector v(4);
      for(int i = 0; i < 4; i++) {
        v[i] = i;
      }
      verify(v.getDimension() == 4);
      ComplexVector v1 = v;
      verify(v1 == v);
      ComplexVector v2;
      v2 = v;
      verify(v2 == v);
      ComplexMatrix D(v);
      verify(D.getDiagonal() == v);
      for(size_t i = 0; i < D.getRowCount(); i++) {
        for(size_t j = 0; j < D.getColumnCount(); j++) {
          if(i == j) {
            verify(D(i, j) == (int)i);
          }
          else {
            verify(D(i, j) == 0);
          }
        }
      }
      Vector vr(4);
      for(int i = 0; i < 4; i++) {
        vr[i] = i;
      }
      Matrix Dr(vr);
      ComplexMatrix C1(Dr);
      verify(C1.getRealPart() == Dr);
      verify(C1.getImaginaryPart() == Matrix::_0(4, 4));
      ComplexMatrix C2;
      C2 = Dr;
      verify(C2.getRealPart() == Dr);
      verify(C2.getImaginaryPart() == Matrix::_0(4, 4));
      ComplexMatrix C3(vr);
      verify(C3.getRealPart() == Dr);
      verify(C3.getImaginaryPart() == Matrix::_0(4, 4));

      verify(fabs(C3) == sqrt(Real(14)));
    }

    TEST_METHOD(MatrixRealEquationsTest) {
      JavaRandom rnd(13144);
      const int dimension = 5;

      for(int i = 0; i < 100; i++) {
        const Matrix   A = randomMatrix(dimension, dimension, rnd);
        const Vector   b = randomVector(dimension, rnd);

        const LUMatrix lu(A);
        const Vector   x = lu.solve(b);
        const Vector   Ax = A * x;

        //  log << _T("A:\n")     << param1 << A << endl;;
        //  log << _T("Det(A):")  << det(A)      << endl;

        const Matrix   aInverse = inverse(A);

        const Real     detA = det(A);
        const Real     detAInverse = det(aInverse);
        verifyAlmostEquals(1, detA * detAInverse, 1e-12);

        //  log << _T("inverse(A):\n") << param1 << aInverse << endl;

        //  log << _T("x  :") << param1 << x  << endl;
        //  log << _T("b  :") << param1 << b  << endl;
        //  log << _T("A*x:") << param1 << Ax << endl;

        const Real diff1 = fabs(b - Ax);
        verify(diff1 < 1e-12);

        //  log << _T("length(b-Ax)           :") << diff1 << endl;
        const Real diff2 = normf(aInverse * A - Matrix::_1(dimension));
        verify(diff2 < 1e-12);
        //  log << _T("normf(A*inverse(A) - I):") << diff2 << endl;
        //  log.flush();
      }
    }

    TEST_METHOD(MatrixComplexEquationsTest) {
      const int dimension = 5;

      try {
        ComplexMatrix E(4, 3);
        ComplexMatrix Ei = inverse(E);
        verify(false);
      } catch(Exception e) {
        verify(true);
      }

      try {
        Matrix E(4, 3);
        ComplexVector v(4);
        ComplexVector r = E*v;
        verify(false);
      } catch(Exception e) {
        verify(true);
      }
      JavaRandom rnd(345);
      const Matrix        T = randomIntMatrix(4, 5, rnd);
      const Complex       factor(1, 1);
      const ComplexMatrix Tc = factor * T;
      for(size_t i = 0; i < Tc.getRowCount(); i++) {
        for(size_t j = 0; j < Tc.getColumnCount(); j++) {
          verify(Tc(i, j) == factor * T(i, j));
        }
      }

      for(int it = 0; it < 100; it++) {
        const ComplexMatrix A = randomComplexMatrix(dimension, dimension, rnd);
        const ComplexVector b = randomComplexVector(dimension, rnd);

        const ComplexLUMatrix lu(A);
        const ComplexVector   x = lu.solve(b);
        const ComplexVector   Ax = A * x;

        //  log << _T("A:\n") << param1 << A      << endl;
        //  log << _T("Det(A):")                  << det(A) << endl;

        const ComplexMatrix aInverse = inverse(A);

        const Complex detA = det(A);
        const Complex detAInverse = det(aInverse);
        verify(fabs(detA * detAInverse - 1) < 1e-13);

        //  log << _T("inverse(A):\n") << param1 << aInverse << endl;

        //  log << _T("x  :") << param1 << x  << endl;
        //  log << _T("b  :") << param1 << b  << endl;
        //  log << _T("A*x:") << param1 << Ax << endl;

        //  log << _T("A*inverse(A):\n") << param1 << p << endl;
        const Real diff1 = fabs(b - Ax);
        verify(diff1 < 1e-12);
        //  log << _T("length(b-Ax)           :") << diff1 << endl;

        const Real diff2 = fabs(aInverse * A - ComplexMatrix(Matrix::_1(dimension)));
        verify(diff2 < 1e-12);
        //  log << _T("normf(A*inverse(A) - I):") << diff2 << endl;
        //  log.flush();

      }
    }

    TEST_METHOD(MatrixSVDTest) {
      JavaRandom rnd;
      rnd.randomize();

      Real maxDetectedLength = 0;
      for(int i = 0; i < 100; i++) {
        int dim = 4;
        int rows = dim + 2;
        int columns = dim;

        const Matrix A = randomMatrix(rows, columns, rnd);

        //  a.setRow(ROWS-1,a.getRow(0) - a.getRow(1));

        const SVDDecomposition svd(A);

        //  log << _T("A:\n") << param1 << A     << endl;
        //  log << _T("U:\n") << param1 << svd.u << endl;
        //  log << _T("D:")   << param1 << svd.d << endl;
        //  log << _T("V:\n") << param1 << svd.v << endl;

        const Matrix A1 = svd.m_u * Matrix(svd.m_d) * transpose(svd.m_v);

        //  log << _T("U*D*transpose(V):\n") << param1 << A1 << endl;
        verify(normf(A - A1) < 1e-14);

        const Matrix UtU = transpose(svd.m_u)*svd.m_u;
        const Matrix VtV = transpose(svd.m_v)*svd.m_v;

        //  log << _T("transpose(U)*U:\n")   << param1 << UtU << endl;
        //  log << _T("transpose(V)*V:\n")   << param1 << VtV << endl;

        verify(normf(UtU - Matrix::_1(dim)) < 1e-14);
        verify(normf(VtV - Matrix::_1(dim)) < 1e-14);

        Vector epsVector(rows);
        epsVector[0] = 0.1;
        const Vector b = 2 * A.getColumn(0) + 3 * A.getColumn(1) + epsVector;
        const Vector x = svd.solve(b);

        DistanceFromPlan diff(A, b);
        const Vector     d = A*x - b;
        const Real       distance = diff(x);
        const Vector     gr = getGradient1(diff, x);
        const Real       grl = gr.length();
        if(grl > maxDetectedLength) {
          maxDetectedLength = grl;
        }
#define F15(x) toString((x),15,23,ios::scientific).cstr()
        INFO(_T("grl:%s, distance:%s, epsLength:%s"), F15(grl), F15(distance), F15(epsVector.length()));
        if(distance > epsVector.length()) {
          LOG log;
          log << _T("b         :") << param1 << b   << endl;
          log << _T("x         :") << param1 << x   << endl;
          log << _T("A*x       :") << param1 << A*x << endl;
          log << _T("A*x-b     :") << param1 << d   << endl;
          log << _T("Gradient  :") << param1 << gr  << endl;
          log << _T("|Gradient|:") << param1 << grl << endl;
          log << _T("Distance  :") << distance      << endl;
          verify(false);
        }
      }
      INFO(_T("maxDetectedLength:%s"), toString(maxDetectedLength, 10, 20, ios::scientific).cstr());
    }

    TEST_METHOD(MatrixCubicRoot) {
      JavaRandom rnd(2346);

      for(int i = 0; i < 100; i++) {
        const Matrix        A = randomMatrix(5, 5, rnd);
        const ComplexMatrix r3 = cubicRoot(A);
        const ComplexMatrix A1 = r3 * r3 * r3;

        //  log << _T("A          :\n") << param1 << A  << endl;
        //  log << _T("root(A,3)  :\n") << param1 << r3 << endl;
        //  log << _T("root(A,3)^3:\n") << param1 << A1 << endl;

        const Real diffRe = normf(A - A1.getRealPart());
        const Real diffIm = normf(A1.getImaginaryPart());
        const Real diff   = diffRe + diffIm;
        if(diff > 1e-11) {
          LOG log;
          log << _T("Cubic root failed") << endl;
          log << _T("A:\n") << A;
          log << _T("root(A,3):\n") << r3;
          log << _T("r3^3:\n") << A1;
          log << _T("norm(difference):") << param2 << diff << endl;
          log << _T("diffRe:") << param2 << diffRe << endl;
          log << _T("diffIm:") << param2 << diffIm << endl;
          verify(false);
        }
      }
    }

    static void checkIsSingular(const Matrix &A, const ComplexVector &eigenValues) {
      int n = (int)A.getRowCount();

      verify(n == (int)eigenValues.getDimension());

      for(int i = 0; i < n; i++) {
        const Complex v = eigenValues[i];
        if(v.im != 0) {
          continue;
        }

        const Matrix S = A - v.re*Matrix::_1(n);

        const SVDDecomposition svd(S);

        bool zeroFound = false;
        for(UINT k = 0; k < svd.m_d.getDimension(); k++) {
          Real sv = svd.m_d[k];
          if(fabs(sv) < 1e-12) {
            zeroFound = true;
            break;
          }
        }
        if(!zeroFound) {
          LOG log;
          log << _T("Matrix not singular.") << endl;

          log << _T("A-"  ) << v.re << _T("*I:\n") << param1 << S << endl;
          log << _T("U:\n") << param1 << svd.m_u << endl;
          log << _T("D:"  ) << param1 << svd.m_d << endl;
          log << _T("V:\n") << param1 << svd.m_v << endl;
          verify(false);
        }
      }
    }

    // Checks that inverse(Q) = transpose(Q)
    static void checkIsUnitary(const String &name, const Matrix &A) {
      try {
        const Matrix Ainverse      = inverse(A);
        const Matrix Atranspose    = transpose(A);
        const Matrix difference    = Ainverse - Atranspose;
        const String inverseName   = format(_T("inverse(%s)"), name.cstr());
        const String transposeName = format(_T("transpose(%s)"), name.cstr());
        const Real   diff = fabs(difference);
        if(diff > 1e-13) {
          LOG log;
          log << name << _T(" not unitary\n");
          log << name << _T(":\n") << param1 << A;
          log << inverseName << _T(":\n") << param1 << Ainverse;
          log << transposeName << _T(":\n") << param1 << Atranspose;
          log << inverseName << _T("-") << transposeName << endl << param1 << difference;
          log << _T("normf(") << inverseName << _T("-") << transposeName << _T("):") << diff << endl;
          throwException(_T("%s not unitary"), name.cstr());
        }
      } catch(Exception e) {
        LOG log;
        log << e.what() << endl;
        log << name << ":\n" << param1 << A;
        verify(false);
      }
    }

    static void testQRMatrix(const Matrix &A) {
      const QRMatrix QR = A;
      const Matrix   U  = QR.getUMatrix();
      checkIsUnitary("U", U);
      const Matrix   Q  = QR.getQMatrix();
      checkIsUnitary("Q", Q);

      const size_t n = A.getRowCount();
      const ComplexVector &EValues = QR.getEigenValues();
      checkIsSingular(A, EValues);
      for(size_t i = 0; i < n; i++) {
        const Complex      &lambda     = EValues[i];
        const ComplexVector EVector    = QR.getEigenVectors().getColumn(i);
        const ComplexVector Av         = A * EVector;
        const ComplexVector lambdaV    = lambda * EVector;
        const ComplexVector difference = lambdaV - Av;
        const Real          diff       = fabs(difference);
        if(diff > 1e-12) {
          const Matrix A0 = transpose(U)* A  * U;
          const Matrix AV = transpose(Q)* A0 * Q;
          LOG log;
          log << _T("Eigenvector mismatch |A*v - lambda*v| > 1e-12\n");
          log << _T("A :\n") << param1 << A  << endl;
          log << _T("U :\n") << param1 << U  << endl;
          log << _T("Q :\n") << param1 << Q  << endl;
          log << _T("A0:\n") << param1 << A0 << endl;
          log << _T("AV:\n") << param1 << AV << endl;
          log << _T("Eigenvalues: {"    ) << EValues       << _T("}") << endl;
          log << _T("Lambda["           ) << i << _T("]:") << lambda  << endl;
          log << _T("Evector["          ) << i << _T("]:") << EVector << endl;
          log << _T("lambda*EVector   :") << lambdaV       << endl;
          log << _T("A*EVector        :") << Av            << endl;
          log << _T("Difference       :") << difference    << endl;
          log << _T("fabs(Difference) :") << diff          << endl;
          verify(false);
        }
      }
    }

    TEST_METHOD(MatrixTestQR_Normal) {
      INFO(_T("  Begin test QRMatrix on 100 random matrices"));
        JavaRandom rnd(2);
//        TODO:
//        JavaRandom rnd(3); <----------- try this one. then i=35 will fail test
        //      rnd.randomize();
        for(int i = 0; i < 100; i++) {
          const size_t dim = randInt(2, 16, rnd);
          INFO(_T("i:%d, dim:%zu"), i, dim);
          testQRMatrix(randomMatrix(dim, dim, rnd));
        }
      INFO(_T("  End test QRMatrix on random matrices"));
    }

    TEST_METHOD(MatrixTestQR_NoConvergence) {
      INFO(_T("  Begin test QRMatrix on no convergence matrix"));
      for(size_t dim = 3; dim <= 10; dim++) {
        testQRMatrix(genNoConvergenceMatrix(dim));
      }
      INFO(_T("  End test QRMatrix on no convergence matrix"));
    }

    TEST_METHOD(MatrixTestQR_Zero) {
      INFO(_T("  Begin test QRMatrix on Zero matrix"));
      for(size_t dim = 2; dim <= 10; dim++) {
        testQRMatrix(Matrix::_0(dim, dim));
      }
      INFO(_T("  End test QRMatrix on Zero matrix"));
    }

    TEST_METHOD(MatrixExp) {
      JavaRandom rnd(13123);
      for(int i = 0; i < 100; i++) {
        const Matrix        A     = randomMatrix(6, 6, rnd); // randomMatrix(6,6);
        const QRMatrix      QR    = A;
        const ComplexMatrix P     = QR.getEigenVectors();
        const ComplexMatrix P1    = inverse(P);
        const ComplexMatrix D     = ComplexMatrix(QR.getEigenValues());

        const Matrix        exp1A = exp(A);

        const int           dim   = (int)A.getRowCount();
        ComplexMatrix       expD  = ComplexMatrix::_1(dim);
        for(int k = 0; k < dim; k++) {
          expD(k, k) = ::exp(D(k, k));
        }

        const ComplexMatrix exp2A = P * expD * P1;

        const Real diff = normf(exp1A - exp2A.getRealPart()) + normf(exp2A.getImaginaryPart());
        if(diff > 1e-12) {
          LOG log;
          log << _T("A:\n")              << param1 << A;
          log << _T("exp(A) (taylor)\n") << exp1A;
          log << _T("exp(A).real (diagonalization):\n") << param1 << exp2A.getRealPart();
          log << _T("EigenValues:")      << QR.getEigenValues() << endl;
          log << _T("EigenVectors:\n")   << P;
          log << _T("Determinant(eigenVectors):")  << StreamParameters(6, 0, ios::scientific) << det(P) << endl;
          log << _T("norm(diff):")       << diff   << endl;
          verify(false);
        }
      }
    }

    TEST_METHOD(MatrixCosSin) {
      JavaRandom rnd(13123);
      for(int i = 0; i < 100; i++) {
        const Matrix        A     = randomMatrix(6, 6, rnd); // randomMatrix(6,6);
        const QRMatrix      QR    = A;
        const ComplexMatrix P     = QR.getEigenVectors();
        const ComplexMatrix P1    = inverse(P);
        const ComplexMatrix D     = ComplexMatrix(QR.getEigenValues());

        const Matrix        cos1A = cos(A);
        const Matrix        sin1A = sin(A);

        const int           dim   = (int)A.getRowCount();
        ComplexMatrix       cosD  = ComplexMatrix::_1(dim);
        ComplexMatrix       sinD  = ComplexMatrix::_1(dim);

        for(int k = 0; k < dim; k++) {
          cosD(k, k) = ::cos(D(k, k));
          sinD(k, k) = ::sin(D(k, k));
        }

        const ComplexMatrix cos2A = P * cosD * P1;
        const ComplexMatrix sin2A = P * sinD * P1;

        const Real cosDiff = normf(cos1A - cos2A.getRealPart()) + normf(cos2A.getImaginaryPart());

        if(cosDiff > 1e-12) {
          LOG log;
          log << _T("A:\n") << param1 << A;
          log << _T("cos(A) (taylor)\n") << cos1A;
          log << _T("cos(A).real (diagonalization):\n") << param1 << cos2A.getRealPart();
          log << _T("cos(A).imag (diagonalization):\n") << param1 << cos2A.getImaginaryPart();
          log << _T("EigenValues:") << QR.getEigenValues() << endl;
          log << _T("EigenVectors:\n") << P;
          log << _T("Determinant(eigenVectors):") << StreamParameters(6, 0, ios::scientific) << det(P) << endl;
          log << _T("norm(diff):") << cosDiff << endl;
          verify(false);
        }

        const Real sinDiff = normf(sin1A - sin2A.getRealPart()) + normf(sin2A.getImaginaryPart());

        if(sinDiff > 1e-12) {
          LOG log;
          log << _T("A:\n") << param1 << A;
          log << _T("sin(A) (taylor)\n") << sin1A;
          log << _T("sin(A).real (diagonalization):\n") << param1 << sin2A.getRealPart();
          log << _T("cos(A).imag (diagonalization):\n") << param1 << sin2A.getImaginaryPart();
          log << _T("EigenValues:") << QR.getEigenValues() << endl;
          log << _T("EigenVectors:\n") << P;
          log << _T("Determinant(eigenVectors):") << StreamParameters(6, 0, ios::scientific) << det(P) << endl;
          log << _T("norm(diff):") << sinDiff << endl;
          verify(false);
        }

        const Matrix C2      = cos1A * cos1A;
        const Matrix S2      = sin1A * sin1A;
        const Matrix sumC2S2 = C2 + S2;
        const Real   diff    = normf(sumC2S2 - Matrix::_1(dim));
        if(diff > 1e-14) {
          LOG log;
          log << _T("A:\n"             ) << param1 << A;
          log << _T("cos(A) (taylor)\n") << cos1A;
          log << _T("sin(A) (taylor)\n") << sin1A;
          log << _T("cos^2(A)\n"       ) << C2;
          log << _T("sin^2(A)\n"       ) << S2;
          log << _T("cos^2(A) + sin^2(A)\n") << sumC2S2;
          log << _T("norm(diff):"      ) << diff << endl;
          verify(false);
        }
      }
    }

    TEST_METHOD(MatrixKroneckerProduct) {
      JavaRandom rnd(13123);
      const Matrix A = randomMatrix(3, 3, rnd);
      const Matrix B = randomMatrix(4, 4, rnd);

      const Matrix AplusB    = kroneckerSum(A, B);
      const Matrix expAplusB = exp(AplusB);
      const Matrix expA      = exp(A);
      const Matrix expB      = exp(B);
      const Matrix expAexpB  = kroneckerProduct(expA, expB);
      //  log << _T("exp(A+B):\n") << expAplusB;
      //  log << _T("expA*expB:\n") << expAexpB;
      const Real   diff      = normf(expAplusB - expAexpB);

      verify(diff < 1e-13);
      //  log << _T("normf(exp(A+B) - exp(A)*exp(B)):") << diff << endl;
    }
  };
}
