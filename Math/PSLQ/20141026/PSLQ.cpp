#include "stdafx.h"
#include <MyUtil.h>
#include <Tokenizer.h>
#include <fstream.h>
#include <Console.h>
#include <Math/BigReal.h>
#include <Math/BigRealMatrix.h>
#include <StreamParameters.h>

class RotationMatrix {
private:
  int m_row,m_column;
  BigReal m_gamma, m_sigma;
public:
  RotationMatrix(const BigRealMatrix &m, int row, int column);
  friend BigRealMatrix &operator*=(BigRealMatrix &m, const RotationMatrix &p);
};

RotationMatrix::RotationMatrix(const BigRealMatrix &m, int row, int column) {
  m_row           = row;
  m_column        = column;
  const BigReal &a = m(row,column);
  const BigReal &b = m(row,column+1);
  const BigReal d  = rSqrt(a*a+b*b,m.getPrecision());
  m_gamma = rQuot(a,d,m.getPrecision());
  m_sigma = rQuot(b,d,m.getPrecision());
}

BigRealMatrix &operator*=(BigRealMatrix &m, const RotationMatrix &p) {
  for(int r = max(p.m_row-1,0); r < m.getRowCount(); r++) {
    BigReal &m1 = m(r,p.m_column);
    BigReal &m2 = m(r,p.m_column+1);
    BigReal r1  = m1*p.m_gamma + m2*p.m_sigma;
    BigReal r2  = m2*p.m_gamma - m1*p.m_sigma;

    m1 = r1;
    m2 = r2;
  }
  m(p.m_row,p.m_column+1) = BigReal::zero;
  return m;
}

class PSLQ {
private:
  BigRealVector m_x;
  double       m_gamma;
  int          m_n;       // = m_x.getDimension()
  int          m_digits;  // = parameter or expo10(max(|m_x[i]|)) * m_n 
  BigReal       m_maxNorm; // = 10^(m_digits-10)
  BigRealMatrix m_A;       // = m_n x m_n matrix
  BigRealVector m_solution;
  bool         m_verbose;

  static const double m_minGamma;

  BigRealMatrix createH(const BigRealVector &x);
  BigRealMatrix createHermiteReducingMatrix( const BigRealMatrix &m);
  BigRealMatrix createHermiteReducingMatrix0(const BigRealMatrix &m);
  int findPivotRow(const BigRealMatrix &m) const;
  void checkOrthogonality(const BigRealMatrix &m);
  int getZeroComponent(const BigRealVector &y);
  int findPrecision(const BigRealVector &x) const;
public:
  PSLQ(const BigRealVector &x, int digits = 0, bool verbose = false, double gamma = 2.0 / sqrt(3)+0.1);
  bool solve();
  BigRealVector getSolution() const {
    return m_solution;
  }
};

const double PSLQ::m_minGamma = 2.0/sqrt(3);

PSLQ::PSLQ(const BigRealVector &x, int digits, bool verbose, double gamma) {
  if(gamma <= m_minGamma) {
    throwException(_T("PSLQ:gamma must be > %.16lf. (=%.16lf)"),m_minGamma,gamma);
  }

  m_verbose = verbose;
  m_x       = x;
  m_gamma   = gamma;
  m_n       = x.getDimension();
  m_digits  = digits ? digits : findPrecision(x);
  m_x.setPrecision(m_digits);
  m_maxNorm = e(1,m_digits-10);
}

bool PSLQ::solve() {
  BigRealMatrix H  = createH(m_x);
  m_A             = createHermiteReducingMatrix(H);
  BigRealMatrix AH = m_A * H; // N x (N-1)-matrix

  for(;;) {
    if(normf(m_A) > m_maxNorm)
      return false; // we are out of digits

    int r = findPivotRow(AH); // Step 1: Exchange. r = [N..N-2]

    if(m_verbose)
      tcout << _T("Pivotrow:") << r << endl;

    m_A.swapRows(r,r+1);
    AH.swapRows(r,r+1);

    if(r != m_n - 2) {   // Step 2: Corner
      AH *= RotationMatrix(AH,r,r);
    }

    // Step 3: Reduction
    BigRealMatrix D = createHermiteReducingMatrix(AH);
    if(m_verbose)
      tcout << _T("Hermite reducing Matrix:") << endl << StreamParameters(0,6,ios::fixed) << D;

    m_A = D * m_A;
    AH  = D * AH;

    if(m_verbose) {
      tcout << _T("AH:") << endl << dparam(8) << AH;
      tcout << _T("A:")  << endl << iparam(12) << m_A;
    }

    // Step 4: check Termination
    try {
      BigRealVector y = m_x * inverse(m_A);
      int z = getZeroComponent(y);
      if(z >= 0) {
        m_solution = inverse(m_A).getColumn(z);
        return true;
      }
    } catch(Exception e) {
      return false; // Occurs when m_A is singular (Exception comes from inverse(m_A))
    }
  }
}

int PSLQ::findPrecision(const BigRealVector &x) const {
  BigReal m = BigReal::zero;
  for(int i = 0; i < x.getDimension(); i++) {
    if(compareAbs(x[i],m) > 0) {
      m = x[i];
    }
  }
  int result = x.getDimension() * BigReal::getExpo10(m);

  if(m_verbose) {
    tcout << _T("Used precision:") << result << _T(" digits.") << endl;

  }
  return result;
}

BigRealMatrix PSLQ::createH(const BigRealVector &x) {
  const int n = x.getDimension();
  BigRealVector s(n,m_digits);
  s[n-1] = x[n-1];
  for(int k = n-2; k >= 0; k--) {
    s[k] = rSqrt(s[k+1]*s[k+1] + x[k]*x[k],m_digits);
  }

  BigRealMatrix H(n,n-1,m_digits);

  for(int column = 0; column < n-1; column++) {
    H(column,column) = rQuot(s[column+1],s[column],m_digits);
    BigReal d = -s[column]*s[column+1];
    for(int row = column+1; row < n; row++) {
      H(row,column) = rQuot(x[column]*x[row],d,m_digits);
    }
  }
  return H;
}

void PSLQ::checkOrthogonality(const BigRealMatrix &m) {
  const int columnCount = m.getColumnCount();
  for(int c1 = 0; c1 < columnCount; c1++) {
    VectorTemplate<BigReal> v1 = m.getColumn(c1);
    for(int c2 = c1+1; c2 < columnCount; c2++) {
      VectorTemplate<BigReal> v2 = m.getColumn(c2);
      tcout << _T("(") << v1 << _T(")*(") << v2 << _T(")=") << v1*v2 << endl;
    }
    tcout << _T("(") << v1 << _T(")*(") << m_x << _T(")=") << v1*m_x << endl;
  }
}

// Returns Matrix D so D*m is "as diagonal as possible"
BigRealMatrix PSLQ::createHermiteReducingMatrix(const BigRealMatrix &m) {
  BigRealMatrix D = BigRealMatrix::one(m_n,m_digits);
  for(int i = 0; i < m_n; i++) {
    for(int j = i-1; j >= 0; j--) {
      BigReal sum;
      for(int k = j+1; k <= i; k++) {
        sum += D(i,k) * m(k,j);
      }
      D(i,j) = floor(BigReal::half-rQuot(sum,m(j,j),m_digits));
    }
  }
  return D;
}

// Returns Matrix D so D*m is diagonal
BigRealMatrix PSLQ::createHermiteReducingMatrix0(const BigRealMatrix &m) { 
  BigRealMatrix D = BigRealMatrix::one(m_n,m_digits);
  for(int i = 0; i < m_n; i++) {
    for(int j = i-1; j >= 0; j--) {
      BigReal sum;
      for(int k = j+1; k <= i; k++) {
        sum += D(i,k) * m(k,j);
      }
      D(i,j) = -rQuot(sum,m(j,j),m_digits);
    }
  }
  return D;
}

int PSLQ::findPivotRow(const BigRealMatrix &m) const {
  BigReal currentMax = -1;
  int r;
  const int n = min(m.getRowCount(),m.getColumnCount());
  for(int j = 0; j < n; j++) {
    BigReal v = pow(m_gamma,j+1) * fabs(m(j,j));
    if(v > currentMax) {
      r = j;
      currentMax = v;
    }
  }
  return r;
}

int PSLQ::getZeroComponent(const BigRealVector &y) {
//  tcout << _T("y:") << nparam << y << endl;
  BigReal minimum,maximum;
  maximum = minimum = fabs(y[0]);
  int result = 0;
  for(int i = 1; i < y.getDimension(); i++) {
    BigReal tmp = fabs(y[i]);
    if(tmp < minimum) {
      minimum = tmp;
      result = i;
    } else if(tmp > maximum) {
      maximum = tmp;
    }
  }
  BigReal q = rQuot(minimum,maximum,10);
  
  if(m_verbose)
    tcout << _T("min:") << dparam(8) << minimum << _T("  |min/max|:") << dparam(8) << q << endl;

  if(q < e(BigReal::one,-14)) {
    return result;
  }
  return -1;
}

static void findIntegerPolynomial(const BigReal &r, bool verbose) {
  int maxExpo10;
  int length = r.getDecimalDigits();
  int digits = length+1;
  for(int degree = 2; degree <= 30; degree++) {
    BigRealVector x(degree+1);
    x[0] = 1;
    maxExpo10 = BigReal::getExpo10(x[0]);
    for(int i = 1; i <= degree; i++) {
      x[i] = rProd(r, x[i-1], digits);
      int d = BigReal::getExpo10(x[i]);
      maxExpo10 = max(d, maxExpo10);
    }
    x.setPrecision(digits);
    if(verbose) {
      tcout << _T("maxExpo10:") << maxExpo10 << _T(", digits:") << digits << endl;
      tcout << _T("x:") << StreamParameters(length,length+6,ios::scientific) << x << endl;
    }

    PSLQ pslq(x,digits,verbose);
    if(pslq.solve()) {
      tcout << _T("Found integer polynomial of degree:") << degree << _T(".")
           << _T(" c(0)..c(") << degree << _T("):")
           << _T(":") << iparam(1) << pslq.getSolution() << endl;

      return;
    } else {
      if(verbose)
        tcout << _T("No solution of degree ") << degree << endl;
    }
  }
  tcout << _T("No solution of degree [2..30] found.") << endl;
}

static void findIntegerRelation(const Array<BigReal> &a, int digits, bool verbose) {
  BigRealVector x(a.size());
  for(int i = 0; i < a.size(); i++)
    x[i] = a[i];
  PSLQ pslq(x,digits,verbose);
  if(pslq.solve()) {
    tcout << _T("Integer relation:") << iparam(1) << _T(":") << pslq.getSolution() << endl;
    return;
  } else {
    tcout << _T("No solution found.") << endl;
  }
}

static void testSuite(bool verbose) {
  findIntegerPolynomial(BigReal("1.414213562373095048801688724209698"), verbose);       // solution:2,0,-1.     x = root(2,2)
  findIntegerPolynomial(BigReal("1.25992104989487316476721060727822835057"), verbose);  // solution:2,0,0,-1    x = root(2,3)
  findIntegerPolynomial(BigReal("1.189207115002721066717499970560475915293"), verbose); // solution:2,0,0,0,-1  x = root(2,4)
//  findIntegerPolynomial(BigReal("4.756828460010884266869999882241903661172"), verbose);
  _tprintf(_T("Test PSLQ:Find integer polynomial for 3.6502815398728847452. Expected solution is 9 -9 -5 14 -13 -1 1\n"));
  findIntegerPolynomial(BigReal("3.6502815398728847452"),verbose);
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
  _ftprintf(stderr,_T("pslq:Usage:pslq [-v] [-pDigits] -rx|-xx1,x2,...xn|-f[file]|-t\n"
                      "     -v: Verbose  . Write temporary information to stdout\n"
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
  Command       cmd     = NO_COMMAND;
  char         *cp;
  bool          verbose = false;
  int           digits  = 0;
  BigReal        x;
  Array<BigReal> v;

  try {
    for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'v':
          verbose = true;
          continue;
        case 'p':
          if(sscanf(cp+1,"%u",&digits) != 1) {
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
            for(Tokenizer tok(cp+1,_T(","); tok.hasNext();) {
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
            
            istream *input = fileName[0] == '\0' ? (istream*)&cin : new ifstream(fileName);
            while(!input->eof() && !input->bad()) {
              BigReal x;
              (*input) >> x;
              v.add(x);
            }
            if(input->bad()) {
              tcerr << _T("Invalid input in ") << fileName << endl;
              exit(-1);
            }
            if(input != &cin) {
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
      findIntegerPolynomial(x,verbose);
      break;
    case FIND_RELATION  :
      findIntegerRelation(v,digits,verbose);
      break;
    case TEST_PSLQ:
      testSuite(verbose);
      break;
    }
  } catch(Exception e) {
    _ftprintf(stderr,_T("Exception:%s\n"),e.what());
    return -1;
  }
  return 0;

//  BaileyBorwein(200);
}
