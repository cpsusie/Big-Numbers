#include "stdafx.h"
#include "CppUnitTest.h"
#include <Random.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace TestMatrix {		

#include <UnitTestTraits.h>

  class LOG : public tostrstream {
  public:
    ~LOG() {
      OUTPUT(_T("%s"), str().c_str());
    }
  };

  static StreamParameters param1(10, 14, ios::fixed);
  static StreamParameters param2(10, 0, ios::scientific);

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

      static Vector randomVector(int dimension) {
        Vector result(dimension);
        setToRandom(result);
        return result;
      }

      static Vector randomIntVector(int dimension) {
        Vector result(dimension);
        for (int i = 0; i < dimension; i++) {
          result[i] = rand() % 21 - 11;
        }
        return result;
      }

      static Matrix randomMatrix(int rows, int columns) {
        Matrix result(rows, columns);
        setToRandom(result);
        return result;
      }

      static Matrix randomIntMatrix(int rows, int columns) {
        Matrix result(rows, columns);
        for (int r = 0; r < rows; r++) {
          result.setRow(r, randomIntVector(columns));
        }
        return result;
      }

      static Matrix genNoConvergenceMatrix(int dim) {
        Matrix a = Matrix::zero(dim, dim);
        for (int i = 1; i < dim; i++) {
          a(i, i - 1) = 1;
        }
        a(0, dim - 1) = 1;
        return a;
      }

      static Matrix randomSymmetricMatrix(int dimension) {
        Matrix result(dimension, dimension);
        for (int r = 0; r < dimension; r++) {
          result.setSubMatrix(r, r, randomMatrix(1, dimension - r));
          if (r < dimension - 1) {
            result.setSubMatrix(r + 1, r, transpose(result.getSubMatrix(r, r + 1, 1, dimension - r - 1)));
          }
        }
        return result;
      }

      static ComplexVector randomComplexVector(int dimension) {
        ComplexVector result(dimension);
        setToRandom(result);
        return result;
      }

      static ComplexMatrix randomComplexMatrix(int rows, int columns) {
        ComplexMatrix result(rows, columns);
        setToRandom(result);
        return result;
      }

      static Matrix exp(const Matrix &A) {
        if (!A.isSquare()) {
          throwException(_T("exp(Matrix):Matrix not square. A.%s"), A.getDimensionString().cstr());
        }
        const int dim = (int)A.getRowCount();
        Matrix    p = Matrix::one(dim);
        Matrix    sum(dim, dim);
        Matrix    last(dim, dim);

        for (Real k = 1;; k++) {
          last = sum;
          sum += p;
          if (normf(sum - last) == 0) {
            break;
          }
          p = p * A;
          p /= k;
        }
        return sum;
      }

      static Matrix cos(const Matrix &A) {
        if (!A.isSquare()) {
          throwException(_T("cos(Matrix):Matrix not square. A.%s"), A.getDimensionString().cstr());
        }
        const int    dim = (int)A.getRowCount();
        const Matrix A2 = -A * A;
        Matrix       p = Matrix::one(dim);
        Matrix       sum(dim, dim);
        Matrix       last(dim, dim);

        for (Real k = 0;; k += 2) {
          last = sum;
          sum += p;
          if (normf(sum - last) == 0) {
            break;
          }
          p = p * A2;
          p /= ((k + 1)*(k + 2));
        }
        return sum;
      }

      static Matrix sin(const Matrix &A) {
        if (!A.isSquare()) {
          throwException(_T("sin(Matrix):Matrix not square. A.%s"), A.getDimensionString().cstr());
        }
        const int    dim = (int)A.getRowCount();
        const Matrix A2 = -A * A;
        Matrix       p = A;
        Matrix       sum(dim, dim);
        Matrix       last(dim, dim);

        for (Real k = 1;; k += 2) {
          last = sum;
          sum += p;
          if (normf(sum - last) == 0) {
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

        for (size_t i = 0; i < D.getDimension(); i++) {
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
      verify(C == Matrix::zero(4, 3));
      Vector v(4);
      for (int i = 0; i < 4; i++) {
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
      for (size_t i = 0; i < D.getRowCount(); i++) {
        for (size_t j = 0; j < D.getColumnCount(); j++) {
          if (i == j) {
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
      verify(C == ComplexMatrix::zero(4, 3));
      ComplexVector v(4);
      for (int i = 0; i < 4; i++) {
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
      for (size_t i = 0; i < D.getRowCount(); i++) {
        for (size_t j = 0; j < D.getColumnCount(); j++) {
          if (i == j) {
            verify(D(i, j) == (int)i);
          }
          else {
            verify(D(i, j) == 0);
          }
        }
      }
      Vector vr(4);
      for (int i = 0; i < 4; i++) {
        vr[i] = i;
      }
      Matrix Dr(vr);
      ComplexMatrix C1(Dr);
      verify(C1.getRealPart() == Dr);
      verify(C1.getImaginaryPart() == Matrix::zero(4, 4));
      ComplexMatrix C2;
      C2 = Dr;
      verify(C2.getRealPart() == Dr);
      verify(C2.getImaginaryPart() == Matrix::zero(4, 4));
      ComplexMatrix C3(vr);
      verify(C3.getRealPart() == Dr);
      verify(C3.getImaginaryPart() == Matrix::zero(4, 4));

      verify(fabs(C3) == sqrt(Real(14)));
    }

    TEST_METHOD(MatrixRealEquationsText) {
      const int dimension = 5;

      for (int i = 0; i < 100; i++) {
        const Matrix   A = randomMatrix(dimension, dimension);
        const Vector   b = randomVector(dimension);

        const LUMatrix lu(A);
        const Vector   x = lu.solve(b);
        const Vector   Ax = A * x;

        //  log << _T("A:\n") << param1 << A      << endl;;
        //  log << _T("Det(A):")              << det(A) << endl;

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
        const Real diff2 = normf(aInverse * A - Matrix::one(dimension));
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
      } catch (MathException e) {
        verify(true);
      }

      try {
        Matrix E(4, 3);
        ComplexVector v(4);
        ComplexVector r = E*v;
        verify(false);
      } catch (Exception e) {
        verify(true);
      }

      const Matrix        T = randomIntMatrix(4, 5);
      const Complex       factor(1, 1);
      const ComplexMatrix Tc = factor * T;
      for (size_t i = 0; i < Tc.getRowCount(); i++) {
        for (size_t j = 0; j < Tc.getColumnCount(); j++) {
          verify(Tc(i, j) == factor * T(i, j));
        }
      }

      for (int it = 0; it < 100; it++) {
        const ComplexMatrix A = randomComplexMatrix(dimension, dimension);
        const ComplexVector b = randomComplexVector(dimension);

        const ComplexLUMatrix lu(A);
        const ComplexVector   x = lu.solve(b);
        const ComplexVector   Ax = A * x;

        //  log << _T("A:\n") << param1 << A      << endl;;
        //  log << _T("Det(A):")                 << det(A) << endl;

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

        const Real diff2 = fabs(aInverse * A - ComplexMatrix(Matrix::one(dimension)));
        verify(diff2 < 1e-12);
        //  log << _T("normf(A*inverse(A) - I):") << diff2 << endl;
        //  log.flush();

      }
    }

    TEST_METHOD(MatrixSVDTest) {
      int dim = 4;
      int rows = dim + 2;
      int columns = dim;

      const Matrix A = randomMatrix(rows, columns);

      //  a.setRow(ROWS-1,a.getRow(0) - a.getRow(1));

      const SVDDecomposition svd(A);

      //  log << _T("A:\n") << param1 << A     << endl;
      //  log << _T("U:\n") << param1 << svd.u << endl;
      //  log << _T("D:")   << param1 << svd.d << endl;
      //  log << _T("V:\n") << param1 << svd.v << endl;

      const Matrix A1 = svd.m_u * Matrix(svd.m_d) * transpose(svd.m_v);

      //  log << _T("U*D*transpose(V):\n") << param1 << A1                     << endl;
      verify(normf(A - A1) < 1e-14);

      const Matrix UtU = transpose(svd.m_u)*svd.m_u;
      const Matrix VtV = transpose(svd.m_v)*svd.m_v;

      //  log << _T("transpose(U)*U:\n")   << param1 << UtU << endl;
      //  log << _T("transpose(V)*V:\n")   << param1 << VtV << endl;

      verify(normf(UtU - Matrix::one(dim)) < 1e-14);
      verify(normf(VtV - Matrix::one(dim)) < 1e-14);

      Vector epsVector(rows);
      epsVector[0] = 0.1;
      const Vector b = 2 * A.getColumn(0) + 3 * A.getColumn(1) + epsVector;
      const Vector x = svd.solve(b);

      DistanceFromPlan diff(A, b);
      const Vector     d = A*x - b;
      const Real       distance = diff(x);
      const Vector     gr = getGradient1(diff, x);
      LOG log;

      log << _T("b       :") << param1 << b << endl;
      log << _T("x       :") << param1 << x << endl;
      log << _T("A*x     :") << param1 << A*x << endl;
      log << _T("A*x-b   :") << param1 << d << endl;
      log << _T("Gradient:") << param1 << gr << endl;
      log << _T("Distance:") << distance << endl;
    }

    TEST_METHOD(MatrixCubicRoot) {
      for (int i = 0; i < 100; i++) {
        const Matrix        A = randomMatrix(5, 5);
        const ComplexMatrix r3 = cubicRoot(A);
        const ComplexMatrix A1 = r3 * r3 * r3;

        //  log << _T("A          :\n") << param1 << A  << endl;
        //  log << _T("root(A,3)  :\n") << param1 << r3 << endl;
        //  log << _T("root(A,3)^3:\n") << param1 << A1 << endl;

        const Real diffRe = normf(A - A1.getRealPart());
        const Real diffIm = normf(A1.getImaginaryPart());
        const Real diff   = diffRe + diffIm;
        if (diff > 1e-11) {
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

      for (int i = 0; i < n; i++) {
        const Complex v = eigenValues[i];
        if (v.im != 0) {
          continue;
        }

        const Matrix S = A - v.re*Matrix::one(n);

        const SVDDecomposition svd(S);

        bool zeroFound = false;
        for (unsigned int k = 0; k < svd.m_d.getDimension(); k++) {
          Real sv = svd.m_d[k];
          if (fabs(sv) < 1e-12) {
            zeroFound = true;
            break;
          }
        }
        if (!zeroFound) {
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
        if (diff > 1e-13) {
          LOG log;
          log << name << _T(" not unitary\n");
          log << name << _T(":\n") << param1 << A;
          log << inverseName << _T(":\n") << param1 << Ainverse;
          log << transposeName << _T(":\n") << param1 << Atranspose;
          log << inverseName << _T("-") << transposeName << endl << param1 << difference;
          log << _T("normf(") << inverseName << _T("-") << transposeName << _T("):") << diff << endl;
          throwException(_T("%s not unitary"), name.cstr());
        }
      } catch (Exception e) {
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

      const int n = (int)A.getRowCount();
      const ComplexVector &EValues = QR.getEigenValues();
      checkIsSingular(A, EValues);
      for (int i = 0; i < n; i++) {
        const Complex      &lambda     = EValues[i];
        const ComplexVector EVector    = QR.getEigenVectors().getColumn(i);
        const ComplexVector Av         = A * EVector;
        const ComplexVector lambdaV    = lambda * EVector;
        const ComplexVector difference = lambdaV - Av;
        const Real          diff       = fabs(difference);
        if (diff > 1e-12) {
          const Matrix A0 = transpose(U)* A  * U;
          const Matrix AV = transpose(Q)* A0 * Q;
          LOG log;
          log << _T("Egenvector passer ikke\n");
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

    TEST_METHOD(MatrixEigenValues) {
      OUTPUT(_T("  Begin test QRMatrix on 100 random matrices"));
      for (int i = 0; i < 100; i++) {
        testQRMatrix(randomMatrix(6, 6));
      }
      OUTPUT(_T("  End test QRMatrix on random matrices"));

      OUTPUT(_T("  Begin test QRMatrix on no convergence matrix"));
      testQRMatrix(genNoConvergenceMatrix(6));
      OUTPUT(_T("  End test QRMatrix on no convergence matrix"));

      OUTPUT(_T("  Begin test QRMatrix on Zero matrix"));
      testQRMatrix(Matrix::zero(6, 6));
      OUTPUT(_T("  End test QRMatrix on Zero matrix"));
    }

    TEST_METHOD(MatrixExp) {
      for (int i = 0; i < 100; i++) {
        const Matrix        A     = randomMatrix(6, 6); // randomMatrix(6,6);
        const QRMatrix      QR    = A;
        const ComplexMatrix P     = QR.getEigenVectors();
        const ComplexMatrix P1    = inverse(P);
        const ComplexMatrix D     = ComplexMatrix(QR.getEigenValues());

        const Matrix        exp1A = exp(A);

        const int           dim   = (int)A.getRowCount();
        ComplexMatrix       expD  = ComplexMatrix::one(dim);
        for (int k = 0; k < dim; k++) {
          expD(k, k) = ::exp(D(k, k));
        }

        const ComplexMatrix exp2A = P * expD * P1;

        const Real diff = normf(exp1A - exp2A.getRealPart()) + normf(exp2A.getImaginaryPart());
        if (diff > 1e-12) {
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
      for (int i = 0; i < 100; i++) {
        const Matrix        A     = randomMatrix(6, 6); // randomMatrix(6,6);
        const QRMatrix      QR    = A;
        const ComplexMatrix P     = QR.getEigenVectors();
        const ComplexMatrix P1    = inverse(P);
        const ComplexMatrix D     = ComplexMatrix(QR.getEigenValues());

        const Matrix        cos1A = cos(A);
        const Matrix        sin1A = sin(A);

        const int           dim   = (int)A.getRowCount();
        ComplexMatrix       cosD  = ComplexMatrix::one(dim);
        ComplexMatrix       sinD  = ComplexMatrix::one(dim);

        for (int k = 0; k < dim; k++) {
          cosD(k, k) = ::cos(D(k, k));
          sinD(k, k) = ::sin(D(k, k));
        }

        const ComplexMatrix cos2A = P * cosD * P1;
        const ComplexMatrix sin2A = P * sinD * P1;

        const Real cosDiff = normf(cos1A - cos2A.getRealPart()) + normf(cos2A.getImaginaryPart());

        if (cosDiff > 1e-12) {
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

        if (sinDiff > 1e-12) {
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
        const Real   diff    = normf(sumC2S2 - Matrix::one(dim));
        if (diff > 1e-14) {
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
      const Matrix A = randomMatrix(3, 3);
      const Matrix B = randomMatrix(4, 4);

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
