#include "stdafx.h"
#include <Tokenizer.h>
#include <Console.h>
#include <Math/BigReal.h>
#include <Math/BigRealMatrix.h>
#include <StreamParameters.h>

// Implementation of PLSQ algorithm described at the following site:
// http://arminstraub.com/downloads/math/pslq.pdf

// #define TEST_ROTATION
// #define TEST_ORTHOGONALITY
// #define TEST_REDUCEEXACT

class RotationMatrix {
private:
  int m_row;
  BigReal m_gamma, m_sigma;
public:
  RotationMatrix(const BigRealMatrix &m, int row);
  friend BigRealMatrix &operator*=(BigRealMatrix &Q, const RotationMatrix &P);
};

RotationMatrix::RotationMatrix(const BigRealMatrix &m, int row) {
  m_row            = row;
  const BigReal &a = m(row,row);
  const BigReal &b = m(row,row+1);
  const BigReal d  = rSqrt(a*a+b*b,m.getPrecision());
  m_gamma = rQuot(a,d,m.getPrecision());
  m_sigma = rQuot(b,d,m.getPrecision());
}

BigRealMatrix &operator*=(BigRealMatrix &Q, const RotationMatrix &P) {
  const UINT digits = Q.getPrecision();
  for(size_t r = max(P.m_row-1,0); r < Q.getRowCount(); r++) {
    BigReal      &q1 = Q(r,P.m_row);
    BigReal      &q2 = Q(r,P.m_row+1);
    const BigReal r1 = rProd(q1,P.m_gamma,digits) + rProd(q2,P.m_sigma,digits);
    const BigReal r2 = rProd(q2,P.m_gamma,digits) - rProd(q1,P.m_sigma,digits);
    q1 = r1;
    q2 = r2;
  }
  Q(P.m_row,P.m_row+1) = BIGREAL_0;
  return Q;
}

#ifdef TEST_ROTATION
class RotationMatrix1 : public BigRealMatrix {
public:
  RotationMatrix1(const BigRealMatrix &m, int row);
};

RotationMatrix1::RotationMatrix1(const BigRealMatrix &m, int row) : BigRealMatrix(m.getColumnCount(),m.getColumnCount()) {
  const BigReal &a    = m(row,row);
  const BigReal &b    = m(row,row+1);
  const BigReal d     = rSqrt(a*a+b*b,m.getPrecision());
  const BigReal gamma = rQuot(a,d,m.getPrecision());
  const BigReal sigma = rQuot(b,d,m.getPrecision());
  const UINT    dim   = (UINT)getRowCount();
  for(UINT i = 0; i < dim; i++) {
    for(UINT j = 0; j < dim; j++) {
      if(i == j) {
        (*this)(i,j) = ((i==row)||(i==row+1)) ? gamma : BIGREAL_1;
      } else if((i == row+1) && (j == row)) {
        (*this)(i,j) = sigma;
      } else if((i == row) && (j == row+1)) {
        (*this)(i,j) = -sigma;
      } else {
        (*this)(i,j) = BIGREAL_0;
      }
    }
  }
}
#endif // TEST_ROTATION

#define VERBOSE_DATA      0x01
#define VERBOSE_MATRIX    0x02
#define VERBOSE_PIVOT     0x04
#define VERBOSE_Y         0x08
#define VERBOSE_INVA      0x10
#define VERBOSE_ALL       0x1f

class PSLQ {
private:
  BigRealVector m_x;        // = inputvector. dimension = N (= m_n)
  double        m_gamma;
  int           m_n;        // = m_x.getDimension()
  int           m_digits;   // = m_x.getPrecision()
  BigReal       m_minBound; // = 1/max(|AHQ(j,j)|)
  BigRealMatrix m_A;        // = N x N matrix
  BigRealVector m_solution;
  int           m_verbose;

  static const double m_minGamma;

  BigRealMatrix createH() const;
#ifdef TEST_REDUCEEXACT
  BigRealMatrix createHermiteReducingMatrix0(const BigRealMatrix &H) const;
#endif // TEST_REDUCEEXACT
#ifdef TEST_ORTHOGONALITY
  void          checkOrthogonality(          const BigRealMatrix &m) const;
#endif // TEST_ORTHOGONALITY
  BigRealMatrix createHermiteReducingMatrix( const BigRealMatrix &H) const;
  int           findPivotRow(                const BigRealMatrix &m) const;
  int           getZeroComponent(            const BigRealVector &y) const;
  int           findPrecision(               const BigRealVector &x) const;
  BigReal       getMaxDiagElement(           const BigRealMatrix &m) const;
public:
  PSLQ(const BigRealVector &x, int digits = 0, int verbose = 0, double gamma = 2.0 / sqrt(3)+0.1);
  // maxDigits is the maximal allowed digits in each integer in solution
  bool solve(UINT maxDigits);
  const BigRealVector &getSolution() const {
    return m_solution;
  }
  const BigReal &getMinBound() const {
    return m_minBound;
  }
};

const double PSLQ::m_minGamma = 2.0/sqrt(3);

PSLQ::PSLQ(const BigRealVector &x, int digits, int verbose, double gamma) {
  if(gamma <= m_minGamma) {
    throwInvalidArgumentException(__TFUNCTION__,_T("gamma=%.16lf. Must be > %.16lf"),gamma,m_minGamma);
  }

  m_verbose = verbose;
  m_x       = x;
  m_gamma   = gamma;
  m_n       = (int)x.getDimension();
  m_digits  = digits ? digits : findPrecision(x);
  m_x.setPrecision(m_digits);
}

int PSLQ::findPrecision(const BigRealVector &x) const {
  BRExpoType maxExpo = BigReal::getExpo10(x[0]);
  BRExpoType minExpo = BigReal::getExpo10(x[0]) - x[0].getDecimalDigits();
  for(size_t i = 1; i < x.getDimension(); i++) {
    const BRExpoType expo1 = BigReal::getExpo10(x[i]);
    const BRExpoType expo2 = BigReal::getExpo10(x[i]) - x[i].getDecimalDigits();
    if(expo1 > maxExpo) maxExpo = expo1;
    if(expo2 < minExpo) minExpo = expo2;
  }
  const int result = (int)(maxExpo - minExpo);

  if(m_verbose&VERBOSE_DATA) {
    tcout << _T("Used precision:") << result << _T(" digits.") << endl;
  }
  return result;
}

BigReal PSLQ::getMaxDiagElement(const BigRealMatrix &m) const {
  BigReal   currentMax = -1;
  const int n          = min((int)m.getRowCount(), (int)m.getColumnCount());
  for(int j = 0; j < n; j++) {
    const BigReal v = fabs(m(j,j));
    if(v > currentMax) {
      currentMax = v;
    }
  }
  return currentMax;
}

// Return Nx(N-1) matrix of partiel sums of vector x
BigRealMatrix PSLQ::createH() const {
  const int           n = (int)m_x.getDimension();
  const BigRealVector x = m_x / m_x.length();
  BigRealVector s2(n, m_digits);
  s2[n-1] = x[n-1]*x[n-1];
  for(int k = n-1; k--;) {
    s2[k] = s2[k+1] + x[k]*x[k];
  }

  BigRealMatrix H(n,n-1,m_digits);

  for(int column = 0; column < n-1; column++) {
    H(column,column) = rSqrt(rQuot(s2[column+1],s2[column],m_digits),m_digits);
    const BigReal d = rSqrt(s2[column]*s2[column+1],m_digits);
    for(int row = column+1; row < n; row++) {
      H(row,column) = -rQuot(x[column]*x[row],d,m_digits);
    }
  }
#ifdef TEST_ORTHOGONALITY
  checkOrthogonality(H);
#endif // TEST_ORTHOGONALITY
  return H;
}

#ifdef TEST_ORTHOGONALITY
void PSLQ::checkOrthogonality(const BigRealMatrix &m) const {
  tcout << _T("Checking ortogonality") << endl;
  const int columnCount = (int)m.getColumnCount();
  for(int c1 = 0; c1 < columnCount; c1++) {
    const VectorTemplate<BigReal> v1 = m.getColumn(c1);
    for(int c2 = c1+1; c2 < columnCount; c2++) {
      const VectorTemplate<BigReal> v2 = m.getColumn(c2);
      tcout << _T("H[") << c1 << _T("]*H[(") << c2 << _T("]=") << v1*v2 << endl;
    }
    tcout << _T("H[") << c1 << _T("]*X=") << v1*m_x << endl;
  }
  tcout << _T("________________________________________") << endl;
}
#endif // TEST_ORTHOGONALITY

#ifdef TEST_REDUCEEXACT
// Returns NxN matrix D0 so D*H is diagonal
BigRealMatrix PSLQ::createHermiteReducingMatrix0(const BigRealMatrix &H) const {
  BigRealMatrix D0 = BigRealMatrix::one(m_n,m_digits);
  for(int i = 0; i < m_n; i++) {
    for(int j = i-1; j >= 0; j--) {
      BigReal sum;
      for(int k = j+1; k <= i; k++) {
        sum += D0(i,k) * H(k,j);
      }
      D0(i,j) = -quot(sum,H(j,j),e(BIGREAL_1,-10));
    }
  }
  return D0;
}
#endif // TEST_REDUCEEXACT

// Returns NxN matrix D so D*H is "as diagonal as possible while preserving the diagonal"
BigRealMatrix PSLQ::createHermiteReducingMatrix(const BigRealMatrix &H) const {
  BigRealMatrix D = BigRealMatrix::one(m_n,m_digits);
  for(int i = 0; i < m_n; i++) {
    for(int j = i-1; j >= 0; j--) {
      BigReal sum;
      for(int k = j+1; k <= i; k++) {
        sum -= D(i,k) * H(k,j);
      }
      D(i,j) = floor(quot(sum,H(j,j),e(BIGREAL_1,-10)) + BIGREAL_HALF);
    }
  }
  return D;
}

int PSLQ::findPivotRow(const BigRealMatrix &m) const {
  BigReal   currentMax = -1;
  int       r          = 0;
  const int n          = min((int)m.getRowCount(), (int)m.getColumnCount());
  double    gpowj      = m_gamma; 
  for(int j = 0; j < n; j++, gpowj *= m_gamma) {
    const BigReal v = gpowj * fabs(m(j,j));
    if(v > currentMax) {
      r          = j;
      currentMax = v;
    }
  }
  return r;
}

int PSLQ::getZeroComponent(const BigRealVector &y) const {
  if(m_verbose&VERBOSE_Y) {
    tcout << _T("y:") << dparam(8) << y << endl;
  }
  BigReal minimum, maximum;
  maximum = minimum = fabs(y[0]);
  int result = 0;
  for(UINT i = 1; i < y.getDimension(); i++) {
    const BigReal tmp = fabs(y[i]);
    if(tmp < minimum) {
      minimum = tmp;
      result = i;
    } else if(tmp > maximum) {
      maximum = tmp;
    }
  }
  const BigReal q = rQuot(minimum,maximum,10);

  if(m_verbose&VERBOSE_DATA) {
    tcout << _T("min:")         << dparam(8) << minimum 
          << _T("  |min/max|:") << dparam(8) << q
          << _T("  Min Bound:") << dparam(8) << getMinBound()
          << endl;
  }

  if(q < 1e-7) {
    return result;
  }
  return -1;
}

inline BigReal sqr(const BigReal &x) {
  return x*x;
}

BigRealMatrix round(const BigRealMatrix &a) {
  BigRealMatrix result = a;
  const size_t  n = result.getRowCount();
  const size_t  m = result.getColumnCount();
  for(size_t i = 0; i < n; i++) {
    for(size_t j = 0; j < m; j++) {
      BigReal &r = result(i,j);
      r = round(r);
    }
  }
  return result;
}

bool PSLQ::solve(UINT maxDigits) {
  const BigReal       maxNorm = rSqrt(sqr(e(1,maxDigits+1)-1)*m_x.getDimension(),10);
  const BigRealMatrix H       = createH();
  m_A                         = createHermiteReducingMatrix(H);
  BigRealMatrix       AHQ     = m_A * H; // N x (N-1)-matrix

  tcout << _T("Digits in calculation           :") << iparam(3) << m_digits  << endl
        << _T("Max number of digits in solution:") << iparam(3) << maxDigits << endl
        << _T("Max bound                       :") << dparam(5) << maxNorm   << endl;

  for(;;) {
    m_minBound = rQuot(BIGREAL_1, getMaxDiagElement(AHQ), 10);
    if(m_minBound > maxNorm) {
      return false; // we are out of digits
    }

    const int r = findPivotRow(AHQ); // Step 1: Exchange. r = [0..N-2]

    if(m_verbose&VERBOSE_PIVOT) {
      tcout << _T("Pivotrow:") << r << endl;
    }

    m_A.swapRows(r, r+1);
    AHQ.swapRows(r, r+1);

    if(r != m_n - 2) {   // Step 2: Corner
#ifdef TEST_ROTATION
      BigRealMatrix AHQTest = AHQ;
      AHQTest *= RotationMatrix1(AHQTest, r);
#endif // TEST_ROTATION
      AHQ *= RotationMatrix(AHQ, r);
    }

    // Step 3: Reduction
#ifdef TEST_REDUCEEXACT
    if(m_verbose&VERBOSE_MATRIX) {
      const BigRealMatrix D0    = createHermiteReducingMatrix0(AHQ);
      const BigRealMatrix D0AHQ = D0 * AHQ;
      tcout << _T("Hermite reducing Matrix D0:") << endl << dparam(8) << D0;
      tcout << _T("D0*AHQ:")                     << endl << dparam(8) << D0AHQ;
    }
#endif

    const BigRealMatrix D = createHermiteReducingMatrix(AHQ);
    if(m_verbose&VERBOSE_MATRIX) {
      tcout << _T("Hermite reducing Matrix D:") << endl << iparam(6) << D;
    }

    m_A = D * m_A;
    AHQ = D * AHQ;

    if(m_verbose&VERBOSE_MATRIX) {
      tcout << _T("AHQ:") << endl << dparam(8)  << AHQ;
      tcout << _T("A:"  ) << endl << iparam(16) << m_A;
    }

    // Step 4: check Termination
    try {
      const BigRealMatrix Ainv = round(inverse(m_A));
      const BigRealVector y    = m_x * Ainv;
      const int           z    = getZeroComponent(y);
      if(z >= 0) {
        if(m_verbose&VERBOSE_INVA) {
          tcout << _T("inv(A):") << endl << iparam(12) << Ainv;
        }
        if(m_verbose&VERBOSE_DATA) {
          tcout << _T("column:") << z << endl;
        }
        m_solution = Ainv.getColumn(z);
        return true;
      }
    } catch(Exception e) {
      return false; // Occurs when m_A is singular (Exception comes from inverse(m_A))
    }
  }
}

static void findIntegerPolynomial(const BigReal &r, int digits, int verbose) {
  const int xdigits = (int)r.getDecimalDigits()+1;
  bool solutionFound = false;
  for(int degree = 2; !solutionFound && (degree <= 30); degree++) {
    BigRealVector x(degree+1);
    x[0] = BIGREAL_1;
    for(int i = 1; i <= degree; i++) {
      x[i] = rProd(r,x[i-1],xdigits);
    }

    if(verbose&VERBOSE_DATA) {
      tcout << _T("Trying degree ") << iparam(2)       << degree << endl;
      tcout << _T("x:")             << dparam(xdigits) << x      << endl;
    }

    PSLQ pslq(x,digits,verbose);

    if(pslq.solve(6)) {
      tcout << _T("Found integer polynomial of degree ") << iparam(2) << degree << _T(".")
            << _T(" c(0)..c(") << degree << _T("):")
            << iparam(1) << pslq.getSolution() << endl;

      solutionFound = true;
    } else {
      if(verbose) {
        tcout << _T("No solution of degree ") << iparam(1) << degree << endl;
      }
    }
  }
  if(!solutionFound)  {
    tcout << _T("No solution of degree [2..30] found.") << endl;
  }
}

static void findIntegerRelation(const Array<BigReal> &a, int digits, int verbose) {
  BigRealVector x(a.size());
  for(size_t i = 0; i < a.size(); i++) {
    x[i] = a[i];
  }
  PSLQ pslq(x,digits,verbose);
  if(pslq.solve(6)) {
    tcout << _T("Integer relation:") << iparam(1) << pslq.getSolution() << endl;
    return;
  } else {
    tcout << _T("No solution found.") << endl;
  }
}

static void testSuite(int digits, int verbose) {
  _tprintf(_T("Test PSLQ:Find integer polynomial for 3.6502815398728847452."
              "Expected solution is 9 0 -14 0 1\n"));
  findIntegerPolynomial(BigReal("3.6502815398728847452"),digits,verbose);
}

void BaileyBorwein(int digits) {
  BigReal sum;
  BigReal p = 1;
  for(int n = 0;n < digits/1.2; n++) {
    sum += rQuot(rQuot(4,8*n+1,digits)-rQuot(2,8*n+4,digits)-rQuot(1,8*n+5,digits)-rQuot(1,8*n+6,digits),p,digits);
    tcout.precision(digits);
    tcout << sum << endl;
    p *= 16;
  }
}

static void usage() {
  _ftprintf(stderr,_T("pslq:Usage:pslq [-v[dmpyi]] [-pDigits] -rx|-xx1,x2,...xn|-f[file]|-t\n"
                      "     -v[dmpyi]    : Verbose. Write temporary information to stdout.\n"
                      "           d:Loop data\n"
                      "           m:Matrices\n"
                      "           p:Pivot row\n"
                      "           y:x*inv(A)\n"
                      "           i:inv(A)\n"
                      "     -pDigits     : Specify the number of digits used in the calculation\n"
                      "     -rx          : Try to find the integer polynomial with x as root\n"
                      "     -xx1,x2,...xn: Try to find an integer relation between the specified x1,x2,...xn\n"
                      "     -f[file]     : Same as -x, but x1..xn are read from file. If file is omitted, stdin is used.\n"
                      "     -t           : Test. Find integer polynomial for number 3.6502815398728847452 which is (9,-9,-5,14,-13,-1,1)\n")
           );
  exit(-1);
}

typedef enum {
  NO_COMMAND
 ,FIND_RELATION
 ,FIND_POLYNOMIAL
 ,TEST_PSLQ
} Command;

int main(int argc, char **argv) {
  Command        cmd     = NO_COMMAND;
  char          *cp;
  int            verbose = 0;
  int            digits  = 0;
  BigReal        x;
  Array<BigReal> v;

  try {
    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'v':
          for(cp++;*cp;cp++) {
            switch(*cp) {
            case 'd': verbose |= VERBOSE_DATA   ; continue;
            case 'm': verbose |= VERBOSE_MATRIX ; continue;
            case 'p': verbose |= VERBOSE_PIVOT  ; continue;
            case 'y': verbose |= VERBOSE_Y      ; continue;
            case 'i': verbose |= VERBOSE_INVA   ; continue;
            default : verbose  = VERBOSE_ALL    ; break;
            }
            break;
          }
          if(*cp) continue; else break;
        case 'p':
          if(sscanf(cp+1, "%u", &digits) != 1) {
            usage();
          }
          break;
        case 'r':
          if(cmd != NO_COMMAND) {
            usage();
          }
          cmd = FIND_POLYNOMIAL;
          x = BigReal(cp+1);
          break;
        case 'x':
          { if(cmd != NO_COMMAND) {
              usage();
            }
            cmd = FIND_RELATION;
            const String tmp(cp+1);
            for(Tokenizer tok(tmp,_T(",")); tok.hasNext();) {
              v.add(BigReal(tok.next()));
            }
          }
          break;
        case 'f':
          { if(cmd != NO_COMMAND) {
              usage();
            }
            cmd = FIND_RELATION;
            char *fileName = cp+1;

            tistream *input = (fileName[0] == '\0') ? (tistream*)&tcin : new tifstream(fileName);
            while(!input->eof() && !input->bad()) {
              BigReal x;
              (*input) >> x;
              v.add(x);
            }
            if(input->bad()) {
              tcerr << _T("Invalid input in ") << fileName << endl;
              exit(-1);
            }
            if(input != &tcin) {
              delete input;
            }
          }
          break;
        case 't':
          if(cmd != NO_COMMAND) {
            usage();
          }
          cmd = TEST_PSLQ;
          continue;

        default :
          usage();
        }
        break;
      }
    }

    if(cmd == NO_COMMAND) {
      usage();
    }

    switch(cmd) {
    case FIND_POLYNOMIAL:
      findIntegerPolynomial(x,digits,verbose);
      break;
    case FIND_RELATION  :
      findIntegerRelation(v,digits,verbose);
      break;
    case TEST_PSLQ:
      testSuite(digits,verbose);
      break;
    }
  } catch(Exception e) {
    _ftprintf(stderr,_T("Exception:%s\n"), e.what());
    return -1;
  }
  return 0;

//  BaileyBorwein(200);
}
