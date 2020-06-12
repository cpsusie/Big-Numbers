// Remes2.cpp : Implements the second algorithm of Remes using double or Double80 (Real)
// as defined in "A first course in numerical Analysis"
// by Anthony Ralston and Philip Rabinowitz, 2. edition, Page 315...

#include "stdafx.h"
#include <MyAssert.h>
#include <Console.h>
#include <fstream>
#include "Remes2.h"

using namespace std;

#define EXTRLINE   6
#define MMQUOTLINE EXTRLINE + m_N + 2
#define COEFLINE   EXTRLINE + m_N + 4
#define ELINE      3
#define BOTTOMLINE COEFLINE + m_N + 4

#if defined(LONGDOUBLE)
#define DEFAULT_PRECISION 19
#else
#define DEFAULT_PRECISION 17
#endif

#define MAXIT 1000

static Real Q_EPS    = 2e-12;
static Real EXTR_EPS = 1e-13;

#define DEFAULT_FLAGS ios::left | ios::showpos | ios::scientific


class FormatNumber {
private:
  StreamParameters m_param;
public:
  Real m_x;
  FormatNumber(const Real &x, int precision = DEFAULT_PRECISION, int width = 30, int flags = DEFAULT_FLAGS);
  FormatNumber(int x, int width = 2, int flags = ios::right | ios::fixed);
  friend tostream &operator<<(tostream &s, const FormatNumber &x);
};

FormatNumber::FormatNumber(const Real &x, int precision, int width, int flags) : m_param(precision,width,flags) {
  m_x = x;
}

FormatNumber::FormatNumber(int x, int width, int flags) : m_param(0,width,flags) {
  m_x = x;
}

tostream &operator<<(tostream &out, const FormatNumber &x) {
  return out << x.m_param << x.m_x;
}

void Remes::initCoef() {
  for(int i = 0; i <= m_N + 1; i++) {
    m_coef[i] = 0.0;
  }
  m_E = 0.0;
}

void Remes::initExtrema() {
  Real ta = (m_right + m_left)/2.0;
  Real tb = (m_left - m_right)/2.0;
  for(int i = 0; i <= m_N + 1; i++ ) {
    m_extr[i] = ta + tb * cos( M_PI * i / (m_N + 1) );
  }
}


void Remes::findCoef() {
  Real   bestQ = -1, bestE;
  Vector bestCoef;

#define FINDCOEF_MAXIT 400

  int it;
  for(it = 0; it < FINDCOEF_MAXIT; it++) {
    Matrix A(m_N+2,m_N+2);

    for(int r = 0, s = 1; r <= m_N + 1; r++, s = -s) {
      Real xr = m_extr[r];
      A(r,0) = sfunc(xr);
      for(int c = 1; c <= m_M; c++) {
        A(r,c) = xr * A(r,c-1);
      }

      if(xr < m_left || xr > m_right) {
        throwException(_T("%s out off range in line %d"),toString(xr).cstr(), __LINE__);
      }

      m_funcValue[r] = targetFunction(xr);

      A(r,m_M+1) = -xr * (m_funcValue[r] - s * m_E);
      for(INT c = m_M + 2; c <= m_N; c++ ) {
        A(r,c) = xr * A(r,c-1);
      }

      A(r,m_N+1) = s;
    }

    LUMatrix LU(A);
    m_coef = LU.solve(m_funcValue);

    Real newE = m_coef[m_N+1];
    if(m_verbose) {
      Console::setCursorPos(0,ELINE);
      tcout << _T("Iteration:") << it << _T(" LastE:") << FormatNumber(m_E) << _T(" NewE:") << FormatNumber(newE);
      tcout.flush();
    }

    if(newE == 0.0) {
      break;
    }
    Real Q = fabs((newE - m_E) / newE);

    if(m_verbose) {
      Console::setCursorPos(0,ELINE+1);
      tcout << _T("Q:") << FormatNumber(Q);
      tcout.flush();
    }

    if(m_mainIteration < 2) {
      if(Q < 0.9) {
        break;
      }
    }
    if(Q < Q_EPS) {
      break;
    } if(m_K > 0) {
      m_E = (m_E+newE) / 2;
    } else {
      m_E = newE;
    }
    if((bestQ < 0) || (Q < bestQ) ) {
      bestCoef = m_coef;
      bestE    = m_E;
      bestQ    = Q;
    }
  }

  if(it == FINDCOEF_MAXIT) {
    if(m_verbose) {
      Console::setCursorPos(0,ELINE);
      tcout << _T("Warning:No Convergence finding E. Using best E:") << FormatNumber(bestE) << endl
           << _T("Q:") << FormatNumber(bestQ);
      tcout.flush();
    }
    m_coef = bestCoef;
    m_E    = bestE;
  }
}

class Point {
public:
  int  m_index;
  Real m_x, m_y;
  Point() { m_index = -1; }
  Point(int index, const Real &x, const Real &y);
};

Point::Point(int index, const Real &x, const Real &y) {
  m_index = index;
  m_x = x;
  m_y = y;
}

static int pointCompareY(const Point &p1, const Point &p2) {
  return sign(p2.m_y - p1.m_y);
}

static Real inverseInterpol( const Point &p1, const Point &p2, const Point &p3) {
  const Real t = (p1.m_y-p2.m_y)*(p2.m_x*p2.m_x-p3.m_x*p3.m_x) - (p2.m_y-p3.m_y)*(p1.m_x*p1.m_x-p2.m_x*p2.m_x);
  const Real d = (p1.m_y-p2.m_y)*(p2.m_x-p3.m_x)               - (p2.m_y-p3.m_y)*(p1.m_x-p2.m_x);
  return t / d / 2.0;
}

Real Remes::findExtremum(const Real &l, const Real &m, const Real &r, int depth) {

#define STEPCOUNT 20

  CompactArray<Point> plot;

  assert( l <= m && m <= r );

  if(l == r) {
    return l;
  }
  Real y       = errorFunction(m);
  int  maxSign = sign(y);
  Real step    = (r-l)/STEPCOUNT;
  Real x       = l;
  for(int count = 0; x < r && count < STEPCOUNT; x += step, count++) {

    assert(l <= x && x <= r );

    Point p;
    p.m_index = count;
    p.m_x = x;
    p.m_y = maxSign * errorFunction(x);
    plot.add(p);
  }
  plot.sort(pointCompareY);

  try {
    x = inverseInterpol(plot[0], plot[1], plot[2]);
  } catch(Exception e) {
    tcout << _T("Warning:") << e.what() << _T("\n"); tcout.flush();
    x = l - 1;
  }

  if(isnan(x)) {
    return plot[0].m_x;
  }
  if(x < l || x > r) {
    y = maxSign * plot[0].m_y;
    x = plot[0].m_x;
  } else {
    assert(l <= x && x <= r);
    y = errorFunction(x);
    if(maxSign * y < plot[0].m_y) {
      y = maxSign * plot[0].m_y;
      x = plot[0].m_x;
    }
  }
  if(depth >= 9) {
    return x;
  } else {
    return findExtremum(x - step/10,x,x+step/10,depth+1);
  }
}

void Remes::setExtremum(int index, const Real &x) {
  m_extr[index]       = x;
  m_errorValue[index] = errorFunction(x);
  if(m_verbose) {
    tcout << _T("Extremum[") << FormatNumber(index) << _T("]:")
         << FormatNumber(x) << _T(",") << FormatNumber(m_errorValue[index])
         << endl;
    tcout.flush();
  }
}

void Remes::findExtrema(Real &minExtr, Real &maxExtr) {
  Vector save = m_extr;

  Console::setCursorPos(0,EXTRLINE);

  setExtremum(0, m_left);
  for(int i = 1; i <= m_N; i++) {
    Real l = ( save[i-1] + 2.0 * save[i] ) / 3.0;
    Real r = ( 2.0 * save[i] + save[i+1] ) / 3.0;
    setExtremum(i,findExtremum( l, save[i], r, 0 ));
  }
  setExtremum(m_N + 1, m_right);

  minExtr = maxExtr = fabs(m_errorValue[0]);
  int minIndex = 0;
  int maxIndex = 0;
  for(int i = 1; i <= m_N+1; i++) {
    const Real absErrorVal = fabs(m_errorValue[i]);
    if(absErrorVal < minExtr) {
      minExtr    = absErrorVal;
      minIndex   = i;
    } else if(absErrorVal > maxExtr) {
      maxExtr    = absErrorVal;
      maxIndex   = i;
      m_maxError = maxExtr;
    }
  }

  if(m_verbose) {
    Console::setCursorPos(0,EXTRLINE);
    for(int i = 0; i <= m_N+1; i++) {
      tcout << _T("Extremum[") << FormatNumber(i) << _T("]:")
           << FormatNumber(m_extr[i]) << _T(",") << FormatNumber(m_errorValue[i])
           ;

      if(i == minIndex) {
        tcout << _T("<- smallest extremum");
      } else if(i == maxIndex) {
        tcout << _T("<- biggest extremum ");
      } else {
        tcout << _T("                    ");
      }
      tcout << endl;
    }
  }
}

Real Remes::approximation(const Real &x) {
  Real sum1 = m_coef[m_M];
  Real sum2 = m_K ? m_coef[m_N] : 0.0;
  int i;
  for(i = m_M - 1; i >= 0; i-- ) sum1 = sum1 * x + m_coef[i];
  for(i = m_N - 1; i >m_M; i-- ) sum2 = sum2 * x + m_coef[i];
  return sum1 / (sum2 * x + 1.0);
}

Real Remes::errorFunction(const Real &x) {
  return m_relative ? (1 - sfunc(x) * approximation(x))
                    : (m_inputFunction(x) - sfunc(x) * approximation(x))
                    ;
}

Real Remes::targetFunction(const Real &x) {
  return m_relative ? 1 : m_inputFunction(x);
}

Real Remes::sfunc(const Real &x) {
  return m_relative ? (1.0/m_inputFunction(x)) : 1;
}

void Remes::plotError(int i) {
  String fileName = format(_T("rem%d.plo"), i);

  tofstream s(fileName.cstr());
  genPlot(s);
  s.close();
}

Remes::Remes(const Real &left, const Real &right, Function &inputFunction, bool relativeError, TCHAR *name, bool verbose) : m_inputFunction(inputFunction) {
  if(left > 0 || right < 0) {
    throwException(_T("Interval [%s;%s] does ont contain 0"),toString(left).cstr(),toString(right).cstr());
  }

  m_name             = name;
  m_left             = left;
  m_right            = right;
  m_verbose          = verbose;
  m_relative         = relativeError;
}

void Remes::solve(int M, int K) {
  m_M = M;
  m_K = K;
  m_N = m_M + m_K;

  const int dimension = m_N + 2;
  m_coef.setDimension(dimension);
  m_extr.setDimension(dimension);
  m_funcValue.setDimension(dimension);
  m_errorValue.setDimension(dimension);

  initExtrema();
  initCoef();

  Real lastMMquot = 0;

  if(m_verbose) {
    Console::clear();
  }

  for(m_mainIteration = 0; m_mainIteration < MAXIT; m_mainIteration++) {
    if(m_verbose) {
      Console::setCursorPos(0,0);
      tcout << _T("Iteration ") << FormatNumber(m_mainIteration) << _T("    ") << endl;
    }

    findCoef( );
//    if(m_verbose) plotError(i);
    if(m_verbose) {
      Console::setCursorPos(0,COEFLINE);
      genOutput(tcout);
    }

    Real minExtr, maxExtr;
    findExtrema(minExtr, maxExtr);

    if(maxExtr == 0) {
      break;
    }

    Real MMquot = fabs(minExtr / maxExtr);

    if(m_verbose) {
      Console::setCursorPos(0,MMQUOTLINE);
      tcout << _T("1-minExtr/maxExtr:") << FormatNumber(1-MMquot) << _T("   ") << endl;
    }

    if(MMquot > (1.0-EXTR_EPS)) {
      break;
    }

    if(m_mainIteration > 1) {
      if(MMquot <= lastMMquot) {
        Console::setCursorPos(0,MMQUOTLINE);
        tcout << _T("Warning : 1-MMquot:") << FormatNumber(1-MMquot) << _T(" >= ") << _T("(1-LastMMquot):") << FormatNumber(1-lastMMquot) << _T("   ") << endl;
      } else {
        Console::clearLine(MMQUOTLINE);
      }
    }
    lastMMquot = MMquot;
  }
  Console::setCursorPos(0,BOTTOMLINE);
}

void Remes::genOutput(tostream &s) {
  for(int i = 0; i <= m_M; i++) {
    s << _T("a[") << FormatNumber(i) << _T("] = ") << FormatNumber(m_coef[i]) << _T("    ") << endl;
  }
  s << _T("b[ 0] = 1") << endl;
  for(int i = m_M + 1; i <= m_N; i++) {
    s << _T("b[") << FormatNumber(i - m_M) << _T("] = ") << FormatNumber(m_coef[i]) << _T("   ") << endl;
  }
  s << _T("Maxerror:") << FormatNumber(m_maxError) << _T("    ") << endl;
  s << _T("E       :") << FormatNumber(m_E)        << _T("    ") << endl;
}

void Remes::genPlot(tostream &s) {
  const Real step = (m_right - m_left) / 200;
  const Real stop = m_right + step/2;
  for(Real x = m_left; x <= stop; x += step ) {
    s << x << _T(" ") << errorFunction(x) << endl;
  }
}

void Remes::genFunction(FILE *f, Language language) {
  switch(language) {
  case CPP:
    genCppFunction(f);
    break;
  case CPP80:
    genCpp80BitFunction(f);
    break;
  case JAVA:
    genJavaFunction(f);
    break;
  default:
    throwException(_T("Illegal language (=%d)"),language);
  }
}

void Remes::genCppFunction(FILE *f) {
  genHeader(f);
  _ftprintf(f, _T("static unsigned char coefdata[] = {\n"));
  for(int i = 0; i <= m_N; i++) {
#if defined(LONGDOUBLE)
    const double coef = getDouble(m_coef[i]);
#else
    const double coef = m_coef[i];
#endif
    _ftprintf(f, _T("   "));
    BYTE *c = (BYTE*)&coef;
    for(int j = 0 ; j < sizeof(coef); j++, c++) {
      _ftprintf(f,  _T("0x%02x%s"), *c, j == sizeof(coef) - 1 ? EMPTYSTRING : _T(",") );
    }
    _ftprintf(f, _T("%s /* %20.16le */\n"), i == m_N ? _T(" ") : _T(","), coef);
  }
  _ftprintf(f, _T("};\n\n") );
  _ftprintf(f, _T("static double *coef = (double*)coefdata;\n") );

  if(m_K) {
    _ftprintf(f, _T("double approximation(double x) {\n"));
    _ftprintf(f, _T("  double sum1 = coef[%d];\n"),m_M);
    _ftprintf(f, _T("  double sum2 = coef[%d];\n"),m_N);
    _ftprintf(f, _T("  int i;\n"));
    _ftprintf(f, _T("  for( i = %d; i >= 0; i-- ) sum1 = sum1 * x + coef[i];\n"), m_M - 1);
    _ftprintf(f, _T("  for( i = %d; i > %d; i-- ) sum2 = sum2 * x + coef[i];\n"), m_N - 1, m_M);
    _ftprintf(f, _T("  return sum1 / (sum2 * x + 1.0);\n"));
    _ftprintf(f, _T("}\n"));
  } else {
    _ftprintf(f, _T("double approximation(double x) {\n"));
    _ftprintf(f, _T("  double sum = coef[%d];\n"),m_N);
    _ftprintf(f, _T("  for(int i = %d; i >= 0; i-- )\n"), m_N - 1);
    _ftprintf(f, _T("    sum = sum * x + coef[i];\n"));
    _ftprintf(f, _T("  return sum;\n"));
    _ftprintf(f, _T("}\n"));
  }
}

void Remes::genCpp80BitFunction(FILE *f) {
  genHeader(f);
  _ftprintf( f, _T("static unsigned char coefdata[] = {\n") );
  for(int i = 0; i <= m_N; i++ ) {
    Real coef = m_coef[i];
    _ftprintf( f, _T("   ") );
    BYTE *c = (BYTE*)&coef;
    for(int j = 0 ; j < sizeof(coef); j++, c++ ) {
      _ftprintf( f, _T("0x%02x%s"), *c, j == sizeof(coef) - 1 ? EMPTYSTRING : _T(",") );
    }
    _ftprintf( f, _T("%s /* %s */\n"), i == m_N ? _T(" ") : _T(","), toString(coef).cstr());
  }
  _ftprintf( f, _T("};\n\n") );
#if defined(LONGDOUBLE)
  TCHAR *realType = _T("Double80");
#else
  TCHAR *realType = _T("double");
#endif
  _ftprintf( f, _T("static %s *coef = (%s*)coefdata;\n"),realType,realType );

  if(m_K) {
    _ftprintf(f, _T("%s approximation(const %s &x) {\n"), realType,realType);
    _ftprintf(f, _T("  %s sum1 = coef[%d];\n"),realType,m_M);
    _ftprintf(f, _T("  %s sum2 = coef[%d];\n"),realType,m_N);
    _ftprintf(f, _T("  int i;\n"));
    _ftprintf(f, _T("  for( i = %d; i >= 0; i-- ) sum1 = sum1 * x + coef[i];\n"), m_M - 1);
    _ftprintf(f, _T("  for( i = %d; i > %d; i-- ) sum2 = sum2 * x + coef[i];\n"), m_N - 1, m_M);
    _ftprintf(f, _T("  return sum1 / (sum2 * x + 1.0);\n"));
    _ftprintf(f, _T("}\n"));
  } else {
    _ftprintf(f, _T("%s approximation(const %s &x) {\n"), realType,realType);
    _ftprintf(f, _T("  %s sum = coef[%d];\n"),realType,m_N);
    _ftprintf(f, _T("  for(int i = %d; i >= 0; i-- )\n"), m_N - 1);
    _ftprintf(f, _T("    sum = sum * x + coef[i];\n"));
    _ftprintf(f, _T("  return sum;\n"));
    _ftprintf(f, _T("}\n"));
  }
}

void Remes::genJavaFunction(FILE *f) {
  genHeader(f);

  _ftprintf(f, _T("    private static final double coef[] = {\n"));
  for(int i = 0; i <= m_N; i++) {
    const double coef = getDouble(m_coef[i]);
    unsigned __int64 *c = (unsigned __int64*)&coef;
    _ftprintf(f, _T("        Double.longBitsToDouble(0x%I64xL)"), *c);
    _ftprintf(f, _T("%s // %20.16le\n"), i == m_N ? _T(" ") : _T(","), coef);
  }
  _ftprintf(f, _T("    };\n\n"));

  _ftprintf(f, _T("    public static double approximation%02d%02d(double x) {\n"),m_M,m_K);
  if(m_K) {
    _ftprintf(f, _T("        double sum1 = coef[%d];\n"),m_M);
    _ftprintf(f, _T("        double sum2 = coef[%d];\n"),m_N);
    _ftprintf(f, _T("        for(int i = %d; i >= 0; i--) sum1 = sum1 * x + coef[i];\n"), m_M - 1);
    _ftprintf(f, _T("        for(int i = %d; i > %d; i--) sum2 = sum2 * x + coef[i];\n"), m_N - 1, m_M);
    _ftprintf(f, _T("        return sum1 / (sum2 * x + 1.0);\n"));
  } else {
    _ftprintf(f, _T("        double sum = coef[%d];\n"),m_N);
    _ftprintf(f, _T("        for(int i = %d; i >= 0; i-- )\n"), m_N - 1);
    _ftprintf(f, _T("            sum = sum * x + coef[i];\n"));
    _ftprintf(f, _T("        return sum;\n"));
  }
  _ftprintf(f, _T("    }\n"));
}

void Remes::genHeader(FILE *f) {
  _ftprintf(f, _T("/* (%d,%d)-Minimax-approximation of %s in [%s,%s] with max %serror = %s */\n"),
            m_M,m_K
           ,m_name
           ,toString(m_left).cstr(), toString(m_right).cstr()
           ,m_relative?_T("relative "):EMPTYSTRING,toString(m_maxError).cstr() );
}
