#include "stdafx.h"
#include <io.h>
#include <Math/Polynomial.h>
#include "BigRealRemes2.h"
#include <Console.h>
#include <fstream>

using namespace std;

#define EXTRHEADERLINE    1
#define FIRSTEXTREMUMLINE ((EXTRHEADERLINE) + 2)
#define MMQUOTLINE        ((FIRSTEXTREMUMLINE) + m_N + 3)
#define ELINE             ((MMQUOTLINE) + 2    )
#define FIRSTCOEFFICIENTLINE          ((ELINE)      + 5    )
#define BOTTOMLINE        ((FIRSTCOEFFICIENTLINE) + m_N + 4)

class NoConvergenceException : public Exception {
public:
  NoConvergenceException(const TCHAR *msg) : Exception(msg) {
  }
};

static void throwNoConvergenceException(const TCHAR *form, ...) {
  va_list argptr;
  va_start(argptr,form);
  String msg = vformat(form,argptr);
  va_end(argptr);
  throw NoConvergenceException(msg.cstr());
}

static BigReal DEFAULT_MMQUOT_EPS = e(BigReal::_1,-22);

#define DEFAULT_FLAGS ios::left | ios::showpos | ios::scientific

class FormatBigReal : public String {
public:
  FormatBigReal(const BigReal &x, int prec = 20, StreamSize width = 30, FormatFlags flags = DEFAULT_FLAGS          ) : String(::toString(x,prec,width,flags)) {}
  FormatBigReal(const Real    &x,                StreamSize width = 18, FormatFlags flags = DEFAULT_FLAGS          ) : String(::toString(x,16  ,width,flags)) {}
  FormatBigReal(int            x,                StreamSize width =  2, FormatFlags flags = ios::right | ios::fixed) : String(::toString(x,0   ,width,flags)) {}
};

static void checkRange(const BigReal &x, const BigReal &left, const BigReal &right) {
  if(x < left || x > right)
    throwException(_T("Remes:checkRange:Invalid argument. x=%s outside range=[%s..%s]")
                   , toString(x    ).cstr()
                   , toString(left ).cstr()
                   , toString(right).cstr());
}

static BigReal signedValue(int sign, const BigReal &x) {
  return sign >= 0 ? x : -x;
}

Remes::Remes(RemesTargetFunction &targetFunction, const bool useRelativeError, const bool verbose)
: m_targetFunction(targetFunction)
, m_left(targetFunction.getInterval().getMin())
, m_right(targetFunction.getInterval().getMax())
, m_digits(targetFunction.getDigits() + 8)
, m_useRelativeError(useRelativeError)
, m_verbose(verbose)
{
  if(m_left > 0 || m_right < 0) {
    throwException(_T("Interval [%s;%s] does not contain 0"), toString(m_left).cstr(), toString(m_right).cstr());
  }

  m_mmQuotEps = DEFAULT_MMQUOT_EPS;
}

Remes::Remes(const Remes &src)
: m_targetFunction(src.m_targetFunction)
, m_left(src.m_targetFunction.getInterval().getMin())
, m_right(src.m_targetFunction.getInterval().getMax())
, m_digits(src.m_targetFunction.getDigits() + 8)
, m_useRelativeError(src.m_useRelativeError)
, m_verbose(src.m_verbose)
{
  m_mmQuotEps = src.m_mmQuotEps;
}

void Remes::setMMQuotEpsilon(const BigReal &mmQuotEps) {
  if((mmQuotEps <= 0) || (mmQuotEps > 0.5)) {
    throwException(_T("Remes::setMMQuotEpsilon:Invalid argument=%s. Must be = ]0;0.5]"), toString(mmQuotEps).cstr());
  }

  m_mmQuotEps = mmQuotEps;
}

String Remes::getMapFileName() const {
  String typeString = m_useRelativeError ? _T("RelativeError") : _T("AbsoluteError");
  return m_targetFunction.getName() + typeString + _T(".dat");
}

void Remes::loadExtremaFromFile() {
  extremaMap.load(getMapFileName());
}

void Remes::saveExtremaToFile() {
  extremaMap.save(getMapFileName());
}

void Remes::solve(const int M, const int K) {
  const int MAXIT = 100;

  m_M = M;
  m_K = K;
  m_N = m_M + m_K;

  const int dimension = m_N + 2;
  m_coefficient.setDimension(dimension);
  m_extrema.setDimension(dimension);
  m_functionValue.setDimension(dimension);
  m_errorValue.setDimension(dimension);

  if(m_verbose) {
    initScreen();
  }

  initSolveState(M,K);
  initCoefficients();

  if(hasSavedExtrema(M,K)) {
    setExtrema(getBestSavedExtrema(M,K));
  } else if(hasFastInterpolationOfExtrema(M,K)) {
    setExtrema(getFastInitialExtremaByInterpolation(M, K));
  } else {
    setExtrema(getDefaultInitialExtrema(M,K));
  }

  BigReal lastMmQuot = BigReal::_1;
  BigReal QEpsilon   = e(BigReal::_1,-2);
  int it;
  for(it = 1; it <= MAXIT; it++) {
    try {
      if(m_verbose) {
        verbose(0, format(_T("M:%d, K:%d. Iteration:%2d. Stop when MinMaxQuot < %s"),M,K,it, toString(m_mmQuotEps).cstr()));
      }

      findCoefficients(QEpsilon);

//    if(m_verbose) plotError(i);
      if(m_verbose) {
        printCoefficients();
      }

      if(m_E.isZero()) {
        break;
      }

      BigReal minExtr, maxExtr;
      findExtrema(minExtr, maxExtr);

      if(maxExtr.isZero()) {
        break;
      }

      const BigReal mmQuot = BigReal::_1 - fabs(rQuot(minExtr,maxExtr,m_digits)); // minExtr -> maxExtr => mmQuot -> 0

      if(m_verbose) {
        verbose(MMQUOTLINE, format(_T("MinMaxQuot = 1-|MinExtr/MaxExtr|:%s"),FormatBigReal(mmQuot).cstr()));
      }

      if(mmQuot < m_mmQuotEps) {
        saveExtremaToMap(m_E,mmQuot);
        saveExtremaToFile();
        break;
      }

      if(it > 1 && m_verbose) {
        if(mmQuot > lastMmQuot) {
          verbose(BOTTOMLINE, format(_T("WARNING:MinMaxQuot=%s > last MinMaxQuot=%s"), FormatBigReal(mmQuot).cstr(), FormatBigReal(lastMmQuot).cstr()));
        } else {
          clearLine(BOTTOMLINE);
        }
      }

      QEpsilon = e(BigReal::_1,BigReal::getExpo10(mmQuot) - 20);
      lastMmQuot = mmQuot;

    } catch(NoConvergenceException e) {
      if(m_verbose) {
        verbose(BOTTOMLINE,format(_T("WARNING:%s"),e.what()));
      }
      if(hasNextSolveState()) {
        nextSolveState();
        setExtrema(findInitialExtremaByInterpolation(M,K));
        if(m_verbose) {
          initScreen();
        }
        continue;
      } else {
        throw;
      }
    }

  }
  if(it > MAXIT) {
    throwException(_T("Remes::solve(%d,%d) stopped after %d iterations. No convergence."),M,K,MAXIT);
  }
}

void Remes::initCoefficients() {
  for(int i = 0; i <= m_N + 1; i++) {
    m_coefficient[i] = 0;
  }
  m_E = 0;
}

void Remes::initSolveState(const int M, const int K) {
  m_solveStateInterpolationDone = false;
  m_solveStateHighPrecision     = false;
  m_solveStateDecrM             = K == 0;
}

void Remes::nextSolveState() {
  if(!m_solveStateInterpolationDone) {
    m_solveStateInterpolationDone = true;
  } else if(!m_solveStateHighPrecision) {
    m_solveStateHighPrecision = true;
  } else if(!m_solveStateDecrM) {
    m_solveStateDecrM = true;
  } else {
    throwException(_T("Remes:No Next solveState for Remes(%d,%d):HighPrecision=true, DecrementM=true"),m_M,m_K);
  }
}

void Remes::saveExtremaToMap(const BigReal &E, const BigReal &mmQuot) {
  ExtremaKey key(m_M,m_K);
  Array<ExtremaVector> *a = extremaMap.get(key);
  if(a != nullptr) {
    a->add(ExtremaVector(m_extrema,E, mmQuot));
  } else {
    Array<ExtremaVector> a;
    a.add(ExtremaVector(m_extrema,E, mmQuot));
    extremaMap.put(key,a);
  }
}

bool Remes::hasNextSolveState() const {
  return (m_solveStateInterpolationDone && m_solveStateHighPrecision && m_solveStateDecrM) ? false : true;
}

// Return the N+2 extrema (incl. endpoints) of the Chebyshev polynomial of degree N+1, scaled to the x-interval [m_left;m_right], where N=M+K
BigRealVector Remes::getDefaultInitialExtrema(const int M, const int K) {
  const Real left  = (Real)m_left;
  const Real right = (Real)m_right;
  const Real a = (left + right)/2.0;
  const Real b = (left - right)/2.0;
  const int    N = M + K;

  BigRealVector result(N+2);
  for(int i = 0; i <= N+1; i++) {
    result[i] = a + b * cos(M_PI * i / (N+1));
  }
  return result;
}

BigRealVector Remes::findFinalExtrema(const int M, const int K, const bool highPrecision) {
  const BigReal mmQuot = highPrecision ? DEFAULT_MMQUOT_EPS : e(BigReal::_1,-7);

  for(bool hasSolved = false;;) {
    Array<ExtremaVector> *a = extremaMap.get(ExtremaKey(M,K));
    if(a != nullptr) {
      for(size_t i = 0; i < a->size(); i++) {
        ExtremaVector &v = (*a)[i];
        if(v.getMMQuot() <= mmQuot) {
          return v;
        }
      }
    }

    if(hasSolved) {
      break;
    }

    setMMQuotEpsilon(mmQuot);
    solve(M,K);
    hasSolved = true;
  }

  throwException(_T("Remes::getFinalExtrema(%d,%d):Cannot find extremaVector with MinMaxQuot < %s, though it should exist"), M, K, toString(mmQuot).cstr());
  return BigRealVector(1);
}

class InterpolationFunction : public CubicSpline {
private:
  const BigRealVector m_initialExtr;

  static CompactArray<DataPoint> getInterpolationPoints(const BigRealVector &initialExtr, const BigRealVector &finalExtr);
public:
  InterpolationFunction(const BigRealVector &initialExtr, const BigRealVector &initialExtr1, const BigRealVector &finalExtr1);
  Real InterpolationFunction::operator()(const Real &x);
};

InterpolationFunction::InterpolationFunction(const BigRealVector &initialExtr, const BigRealVector &initialExtr1, const BigRealVector &finalExtr1)
: CubicSpline(NATURAL_SPLINE, getInterpolationPoints(initialExtr1,finalExtr1), 0, 0)
, m_initialExtr(initialExtr)
{
}

CompactArray<DataPoint> InterpolationFunction::getInterpolationPoints(const BigRealVector &initialExtr, const BigRealVector &finalExtr) { // static
  const int    dim   = (int)initialExtr.getDimension();
  CompactArray<DataPoint> result;

  for(int i = 0; i <= dim-1; i++) {
    const Real x = (Real)i/(dim-1); // x = [0..1] Equally spaced
    const Real y = (Real)(finalExtr[i] - initialExtr[i]); // first and last y will be 0
    result.add(DataPoint(x,y));
  }
  return result;
}

Real InterpolationFunction::operator()(const Real &x) {
  const int maxX = (int)m_initialExtr.getDimension()-1;
  if(x == 0 || x == maxX) {
    return (Real)m_initialExtr[(int)x];
  } else if(x < 0 || x > maxX) {
    throwException(_T("InterpolationFunction::Invalid argument. x=%s. x must be in the interval [0..%d]"), toString(x).cstr(), maxX);
  }

  return (Real)m_initialExtr[(int)x] + CubicSpline::operator()(x/maxX);
}

bool Remes::hasSavedExtrema(const int M, const int K) {
  return extremaMap.get(ExtremaKey(M,K)) != nullptr;
}

const ExtremaVector &Remes::getBestSavedExtrema(const int M, const int K) {
  const Array<ExtremaVector> *a = extremaMap.get(ExtremaKey(M,K));
  if(a == nullptr) {
    throwException(_T("Remes:getBestSavedExtrema:ExtremaKey(%d,%d) not found"), M, K);
  }
 size_t bestIndex = 0;
  for(size_t i = 1; i < a->size(); i++) {
    if((*a)[i].getMMQuot() < (*a)[bestIndex].getMMQuot()) {
      bestIndex = i;
    }
  }
  return (*a)[bestIndex];
}

bool Remes::hasFastInterpolationOfExtrema(const int M, const int K) {
  return (K > 0 && hasSavedExtrema(M,K-1)) || (M > 1 && hasSavedExtrema(M-1,K));
}

BigRealVector Remes::getFastInitialExtremaByInterpolation(const int M, const int K) {
  int M1 = M, K1 = K;
  if(K > 0 && hasSavedExtrema(M,K-1)) {
    K1--;
  } else if(M > 1 && hasSavedExtrema(M-1,K)) {
    M1--;
  } else {
    throwException(_T("Remes::getFastInitialExtremaByInterpolation(%d,%d):No saved solution for (M,K)=(%d,%d) or (%d,%d)"),M,K,M,K-1,M-1,K);
  }
  return getInterpolatedExtrema(getDefaultInitialExtrema(M,K),getDefaultInitialExtrema(M1,K1),getBestSavedExtrema(M1,K1));
}

BigRealVector Remes::findInitialExtremaByInterpolation(const int M, const int K) {
  const int M1 = m_solveStateDecrM ? (M-1) :  M;
  const int K1 = m_solveStateDecrM ?  K    : (K-1);

  Remes subRemes(*this);
  m_solveStateInterpolationDone = true;

  return getInterpolatedExtrema(getDefaultInitialExtrema(M,K),getDefaultInitialExtrema(M1,K1),subRemes.findFinalExtrema(M1,K1, m_solveStateHighPrecision));
}

BigRealVector Remes::getInterpolatedExtrema(const BigRealVector &defaultExtrema, const BigRealVector &defaultSubExtrema, const BigRealVector &finalSubExtrema) const {
  InterpolationFunction interpolFunction(defaultExtrema, defaultSubExtrema, finalSubExtrema);

  const int dimension = (int)defaultExtrema.getDimension();
  BigRealVector result(dimension);
  for(int i = 0; i < dimension; i++) {
    result[i] = interpolFunction(i);
  }
  return result;
}

void Remes::findCoefficients(const BigReal &QEpsilon) {
  static const BigReal c1 = 0.5;

  const int MAXIT = 700;

  BigRealMatrix A(m_N+2,m_N+2,m_digits);
  for(int r = 0, s = 1; r <= m_N+1; r++, s = -s) {
    const BigReal &xr = m_extrema[r];

    checkRange(xr, m_left, m_right);

    A(r,0) = sFunction(xr);
    for(int c = 1; c <= m_M; c++) {
      A(r,c) = xr * A(r,c-1);
    }
    A(r,m_N+1) = s;

    m_functionValue[r] = targetFunction(xr);
  }

  if(m_K == 0) {
    // The coefficients a[0]..a[m] and E can be found by solving the linear system of equations
    BigRealLUMatrix LU(A);

    m_coefficient = LU.solve(m_functionValue);
    m_E = m_coefficient[m_N+1];
    if(m_verbose) {
      verbose(ELINE,format(_T("New E:%s"),FormatBigReal(m_E,25).cstr()));
    }
  } else {
    // for m_K > 0 we have to iterate
    String Qstr = toString(QEpsilon);
    int it;
    for(it = 1; it <= MAXIT; it++) {

      for(int r = 0, s = 1; r <= m_N+1; r++, s = -s) {
        const BigReal &xr = m_extrema[r];
        A(r,m_M+1) = xr * (signedValue(s,m_E) - m_functionValue[r]);
        for(int c = m_M + 2; c <= m_N; c++) {
          A(r,c) = xr * A(r,c-1);
        }
      }

      BigRealLUMatrix LU(A);

      m_coefficient = LU.solve(m_functionValue);

      BigReal newE = m_coefficient[m_N+1];

      if(m_verbose) {
        verbose(ELINE  ,format(_T("Search E. Iteration %3d. Stop when Q < %s."),it,Qstr.cstr()));
        verbose(ELINE+1,format(_T("Last E:%s"), FormatBigReal(m_E ,25).cstr()));
        verbose(ELINE+2,format(_T("New  E:%s"), FormatBigReal(newE,25).cstr()));
      }

      if(newE.isZero()) {
        break;
      }

      BigReal Q = fabs(BigReal::_1 - rQuot(dmin(fabs(m_E),fabs(newE)), dmax(fabs(m_E),fabs(newE)), m_digits));

      if(m_verbose) {
        verbose(ELINE+3,format(_T("Q=1-min(|Last E|, |New E|)/max(|Last E|, |New E|):%s"), FormatBigReal(Q).cstr()));
      }

      if(Q < QEpsilon) {
        break;
      }

      m_E = rProd(c1,m_E + newE,m_digits);
    }

    if(it > MAXIT) {
      throwNoConvergenceException(_T("Remes:findCoefficients:No convergence after %d iterations."),MAXIT);
    }
  }
}

void Remes::findExtrema(BigReal &minExtr, BigReal &maxExtr) {
  BigRealVector save = m_extrema;

  if(m_verbose) {
    verbose(EXTRHEADERLINE,spaceString(78,'_'));
    const TCHAR *errHeader = m_useRelativeError ? _T("Relative error(x)") : _T("Error(x)");
    verbose(EXTRHEADERLINE+1,format(_T("             x                              %s"),errHeader));
  }

  for(int i = 0; i <= m_N+1; i++) {
    BigReal l = i == 0       ? m_left  : rQuot(save[i-1] + 2.0 * save[i], 3, m_digits);
    BigReal r = i == m_N + 1 ? m_right : rQuot(save[i+1] + 2.0 * save[i], 3, m_digits);
    setExtremum(i, findExtremum(l, save[i], r, 0));
  }

  int prevSign = sign(m_errorValue[0]);
  for(int i = 1; i <= m_N+1; i++) {
    int s = sign(m_errorValue[i]);
    if(s == prevSign) {
      if(m_verbose) {
        for(int j = 0; j <= m_N+1; j++) {
          printExtremum(j, _T("                    "));
        }
        printExtremum(i,_T("Error. Sign doesn't change."));
      }
      throwNoConvergenceException(_T("Remes:findExtrema:Sign(errorFunction) doesn't change between the found extrema."));
    }
    prevSign = s;
  }

  int minIndex = 0;
  int maxIndex = 0;
  for(int i = 1; i <= m_N+1; i++) {
    if(BigReal::compareAbs(m_errorValue[i], m_errorValue[minIndex]) < 0) {
      minIndex   = i;
    } else if(BigReal::compareAbs(m_errorValue[i], m_errorValue[maxIndex]) > 0) {
      maxIndex   = i;
    }
  }
  minExtr = fabs(m_errorValue[minIndex]);
  maxExtr = fabs(m_errorValue[maxIndex]);
  m_maxError = maxExtr;

  if(m_verbose) {
    for(int i = 0; i <= m_N+1; i++) {
      const TCHAR *comment = (i==minIndex)? _T("<- Smallest extremum") : (i==maxIndex) ? _T("<- Biggest extremum ") : _T("                    ");
      printExtremum(i, comment);
    }
  }
}

class Point {
public:
  BigReal m_x, m_y;
  Point(const BigReal &x, const BigReal &y);
};

Point::Point(const BigReal &x, const BigReal &y) {
  m_x = x;
  m_y = y;
}

static int pointCompareY(const Point &p1, const Point &p2) {
  return BigReal::compare(p2.m_y,p1.m_y);
}

static BigReal sqr(const BigReal &x, unsigned int digits) {
  return rProd(x, x, digits);
}

static BigReal inverseInterpolate(const Point &p1, const Point &p2, const Point &p3, unsigned int digits) {
  static const BigReal c1(0.5);

  BigReal sqx1 = sqr(p1.m_x,digits);
  BigReal sqx2 = sqr(p2.m_x,digits);
  BigReal sqx3 = sqr(p3.m_x,digits);
  BigReal dx12 = p1.m_x - p2.m_x;
  BigReal dx23 = p2.m_x - p3.m_x;
  BigReal dy12 = p1.m_y - p2.m_y;
  BigReal dy23 = p2.m_y - p3.m_y;

  BigReal t = rProd(dy12,sqx2 - sqx3,digits) - rProd(dy23,sqx1 - sqx2,digits);
  BigReal d = rProd(dy12,dx23       ,digits) - rProd(dy23,dx12       ,digits);
  return c1 * rQuot(t,d,digits);
}

BigReal Remes::findExtremum(const BigReal &l, const BigReal &m, const BigReal &r, int depth) {
  const int STEPCOUNT = 20;

  Array<Point> plot;

  checkRange(m, l, r);

  if(l == r) {
    return l;
  }

  int errorSign = sign(errorFunction(m));

  const BigReal step = rQuot(r-l,STEPCOUNT-1,m_digits);
  BigReal x = l;
  for(int count = 0;;) {
    checkRange(x, l, r);
    plot.add(Point(x,signedValue(errorSign, errorFunction(x))));

    count++;
    if(count < STEPCOUNT-1) {
      x += step;
    } else if(count == STEPCOUNT-1) {
      x = r;
    } else { // count == STEPCOUNT
      break;
    }
  }
  plot.sort(pointCompareY);

  try {
    x = inverseInterpolate(plot[0], plot[1], plot[2], m_digits);
  } catch(BigRealException e) {
    if(m_verbose) {
      verbose(BOTTOMLINE,format(_T("WARNING:%s                                "),e.what()));
    }
    x = l - 1;
  }
  if(x < l || x > r) {
    x = plot[0].m_x;
  } else { // l <= x <= r
    if(signedValue(errorSign, errorFunction(x)) < plot[0].m_y) {
      x = plot[0].m_x;
    }
  }
  if(x <= m_left) {
    return m_left;
  } else if(x >= m_right) {
    return m_right;
  } else if(depth >= 9) {
    return x;
  } else {
    return findExtremum(x - e(step,-1), x, x + e(step,-1), depth+1);
  }
}

void Remes::setExtrema(const BigRealVector &extrema) {
  if(extrema.getDimension() != m_N + 2) {
    throwException(_T("Remes::setExtrema:Invalid dimension of extremaVector=%d, N=%d => expected dimension=%d"), (int)extrema.getDimension(), m_N, m_N+2);
  }

  for(size_t i = 0; i < extrema.getDimension(); i++) {
    setExtremum((int)i,extrema[i]);
  }
}

void Remes::setExtrema(const ExtremaVector &extrema) {
  setExtrema((const BigRealVector&)extrema);
  m_E = extrema.getE();
}

void Remes::setExtremum(const int index, const BigReal &x) {
  m_extrema[index]    = x;
  m_errorValue[index] = errorFunction(x);
  if(m_verbose) {
    printExtremum(index,EMPTYSTRING);
  }
}

BigReal Remes::approximation(const BigReal &x) {
  if(m_K) {
    BigReal sum1 = m_coefficient[m_M];
    BigReal sum2 = m_coefficient[m_N];
    int i;
    for(i = m_M - 1; i >= 0  ; i--) {
      sum1 = sum1 * x + m_coefficient[i];
    }
    for(i = m_N - 1; i >  m_M; i--) {
      sum2 = sum2 * x + m_coefficient[i];
    }
    return rQuot(sum1, sum2 * x + BigReal::_1, m_digits);
  } else {
    BigReal sum = m_coefficient[m_M];
    for(int i = m_M - 1; i >= 0; i--) {
      sum = sum * x + m_coefficient[i];
    }
    return sum;
  }
}

BigReal Remes::errorFunction(const BigReal &x) {
  return m_useRelativeError ? (BigReal::_1 - sFunction(x) * approximation(x))
                            : (m_targetFunction(x) - approximation(x) /* * sFunction(x)==1 */ )
                            ;
}

BigReal Remes::sFunction(const BigReal &x) {
  return m_useRelativeError ? rQuot(BigReal::_1,m_targetFunction(x),m_digits) : BigReal::_1;
}

BigReal Remes::targetFunction(const BigReal &x) {
  return m_useRelativeError ? BigReal::_1 : m_targetFunction(x);
}

void Remes::plotError(int i) {
  FILE *f = FOPEN(format(_T("rem%02d.plo"), i).cstr(),_T("w"));
  generatePlot(f);
  fclose(f);
}

void Remes::printExtremum(const int index, const TCHAR *comment) {
  verbose(FIRSTEXTREMUMLINE+index,format(_T("Extremum[%2d]:%s, %s%s"),index,FormatBigReal(m_extrema[index]).cstr(), FormatBigReal(m_errorValue[index]).cstr(),comment));
}

void Remes::printCoefficients() {
  int i;
  for(i = 0; i <= m_M; i++) {
    verbose(FIRSTCOEFFICIENTLINE+i,format(_T("a[%2d] = %s     %s"), i,    FormatBigReal(m_coefficient[i],25,35).cstr(), FormatBigReal((Real)m_coefficient[i]).cstr()));
  }
  verbose(FIRSTCOEFFICIENTLINE+i,format(_T("b[ 0] = +1")));
  for(i = m_M + 1; i <= m_N; i++) {
    verbose(FIRSTCOEFFICIENTLINE+i+1,format(_T("b[%2d] = %s     %s"), i-m_M,FormatBigReal(m_coefficient[i],25,35).cstr(), FormatBigReal((Real)m_coefficient[i]).cstr()));
  }
  verbose(FIRSTCOEFFICIENTLINE+m_N+2,format(_T("Maxerror:%s"),FormatBigReal(m_maxError).cstr()));
}


void Remes::generatePlot(FILE *f) {
  BigReal step = rQuot(m_right - m_left,2000,m_digits);
  BigReal stop = m_right + step*0.5;
  for(BigReal x = m_left; x <= stop; x += step)
    _ftprintf(f, _T("%s %s\n"), toString(x,8,16,ios::scientific).cstr(), toString(errorFunction(x),8,16,ios::scientific).cstr());
}

void Remes::generateCFunction(FILE *f, bool useDouble80) {
  generateHeader(f);

  const TCHAR *type = useDouble80 ? _T("Double80") : _T("double");
  int typeSize = useDouble80 ? 10 : sizeof(double);

  if(useDouble80) {
    _ftprintf(f, _T("#include <Double80.h>\n\n"));
  }

  _ftprintf(f, _T("static const %s coef[%d] = {\n"), type, m_N+1);
  for(int i = 0; i <= m_N; i++) {
    Double80 d80 = (Double80)m_coefficient[i];
    double   d64 = (double  )m_coefficient[i];
    unsigned char *byte = useDouble80 ? (unsigned char*)&d80 : (unsigned char*)&d64;
    _ftprintf(f, useDouble80 ? _T("  Double80::bytesToDouble80((unsigned char*)\"") : _T("  *((double*)\""));
    for(int j = 0; j < typeSize; j++) {
      _ftprintf(f, _T("\\x%02x"), *(byte++));
    }
    _ftprintf(f, _T("%s // %s\n"), (i==m_N)?_T("\") "):_T("\"),"), toString(d80,17,24,ios::scientific).cstr());
  }
  _ftprintf( f, _T("};\n\n") );

  _ftprintf(f, _T("double approximation%02d%02d(%s x) {\n"), m_M,m_K, type);

  if(m_K) {
    _ftprintf(f, _T("  %s sum1 = coef[%d];\n"), type, m_M);
    _ftprintf(f, _T("  %s sum2 = coef[%d];\n"), type, m_N);
    _ftprintf(f, _T("  int i;\n"));
    if(m_M - 1 >= 0)
      _ftprintf(f, _T("  for(i = %d; i >= 0; i--) sum1 = sum1 * x + coef[i];\n"), m_M - 1);
    if(m_N - 1 > m_M)
      _ftprintf(f, _T("  for(i = %d; i > %d; i--) sum2 = sum2 * x + coef[i];\n"), m_N - 1, m_M);
    _ftprintf(f, _T("  return %s;\n"), useDouble80 ? _T("(double)(sum1 / (sum2 * x + 1.0))") : _T("sum1 / (sum2 * x + 1.0)"));
  }
  else {
    _ftprintf(f, _T("  %s sum = coef[%d];\n"), type, m_N);
    if(m_N - 1 >= 0)
      _ftprintf(f, _T("  for(int i = %d; i >= 0; i--) sum = sum * x + coef[i];\n"), m_N - 1);
    _ftprintf(f, _T("  return %s;\n"), useDouble80 ? _T("(double)sum") : _T("sum"));
  }
  _ftprintf(f, _T("}\n"));
}

void Remes::generateJavaFunction(FILE *f) {
  generateHeader(f);

  _ftprintf(f, _T("    private static final double coef[] = {\n"));
  for(int i = 0; i <= m_N; i++) {
    const double            coef = (double)m_coefficient[i];
    const unsigned __int64 *c    = (unsigned __int64*)&coef;
    _ftprintf(f, _T("        Double.longBitsToDouble(0x%I64xL)"), *c);
    _ftprintf(f, _T("%s // %20.16le\n"), i == m_N ? _T(" ") : _T(","), coef);
  }
  _ftprintf(f, _T("    };\n\n"));

  _ftprintf(f, _T("    public static double approximation%02d%02d(double x) {\n"), m_M, m_K);
  if(m_K) {
    _ftprintf(f, _T("        double sum1 = coef[%d];\n"), m_M);
    _ftprintf(f, _T("        double sum2 = coef[%d];\n"), m_N);
    if(m_M - 1 >= 0)
      _ftprintf(f, _T("        for(int i = %d; i >= 0; i--) sum1 = sum1 * x + coef[i];\n"), m_M - 1);
    if(m_N - 1 > m_M)
      _ftprintf(f, _T("        for(int i = %d; i > %d; i--) sum2 = sum2 * x + coef[i];\n"), m_N - 1, m_M);
    _ftprintf(f, _T("        return sum1 / (sum2 * x + 1.0);\n"));
  }
  else {
    _ftprintf(f, _T("        double sum = coef[%d];\n"),m_N);
    if(m_N - 1 >= 0)
      _ftprintf(f, _T("        for(int i = %d; i >= 0; i--) sum = sum * x + coef[i];\n"), m_N - 1);
    _ftprintf(f, _T("        return sum;\n"));
  }
  _ftprintf(f, _T("    }\n"));
}

void Remes::generateHeader(FILE *f) {
  _ftprintf(f, _T("// (%d,%d)-Minimax-approximation of %s in [%le,%le] with max %serror = %21.15le\n")
   ,m_M,m_K
   ,m_targetFunction.getName().cstr()
   ,(double)m_left
   ,(double)m_right
   ,m_useRelativeError ? _T("relative ") : EMPTYSTRING
   ,(double)m_maxError);
}

// -----------------------------------------------------------------------------------

void Remes::initScreen() {
  int maxWidth,maxHeight;
  int width,height;
  Console::getLargestConsoleWindowSize(maxWidth,maxHeight);
  Console::getWindowSize(width,height);
  width  = max(width,120);
  height = max(height,BOTTOMLINE+2);
  Console::setWindowSize(width,min(maxHeight,height));
  Console::setBufferSize(width,height);

  Console::clear();
}

void Remes::verbose(const int lineBigReal, const String &msg) {
  Console::setCursorPos(0,lineBigReal);
  tcout << msg;
  tcout.flush();
}

void Remes::clearLine(const int lineBigReal) {
  Console::setCursorPos(0,lineBigReal);
  int width,height;
  Console::getBufferSize(width,height);
  tcout << spaceString(width);
  tcout.flush();
}

// -----------------------------------------------------------------------------------

static unsigned long ExtremaKeyHash(const ExtremaKey &key) {
  return key.hashCode();
}

static int ExtremaKeyCompare(const ExtremaKey &k1, const ExtremaKey &k2) {
  return k1 == k2 ? 0 : 1;
}

ExtremaHashMap::ExtremaHashMap() : HashMap<ExtremaKey,Array<ExtremaVector> >(ExtremaKeyHash,ExtremaKeyCompare) {
}

ExtremaHashMap Remes::extremaMap;

void ExtremaHashMap::save(const String &name) {
  tofstream out(name.cstr());
  save(out);
  out.close();
}

void ExtremaHashMap::load(const String &name) {
  if(ACCESS(name,0) < 0)
    return;

  tifstream in(name.cstr());
  load(in);
  in.close();
}

static int extremaVectorCmp(const ExtremaVector &v1, const ExtremaVector &v2) {
  return sign(v1.getMMQuot() - v2.getMMQuot());
}

void ExtremaHashMap::save(tostream &out) {
  auto it = getIterator();
  out << size() << endl;
  while(it.hasNext()) {
    Entry<ExtremaKey, Array<ExtremaVector> > &e = it.next();
    out << e.getKey() << endl;
    Array<ExtremaVector> &a = e.getValue();
    if(a.size() > 1) {
      a.sort(extremaVectorCmp);
      while(a.size() > 1)
        a.removeLast();
    }
    out << a.size() << endl;
    for(size_t i = 0; i < a.size(); i++) {
      out << a[i];
    }
  }
}

void ExtremaHashMap::load(tistream &in) {
  clear();
  int count;
  in >> count;
  for(int k = 0; k < count; k++) {
    ExtremaKey key;
    in >> key;
    Array<ExtremaVector> a;
    int arraySize;
    in >> arraySize;
    for(int i = 0; i < arraySize; i++) {
      ExtremaVector v;
      in >> v;
      a.add(v);
    }
    put(key,a);
  }
}

tostream &operator<<(tostream &out, const ExtremaKey &key) {
  out << key.m_M << _T(" ") << key.m_K;
  return out;
}

tistream &operator>>(tistream &in, ExtremaKey &key) {
  int m, k;
  in >> m >> k;
  key = ExtremaKey(m,k);
  return in;
}


tostream &operator<<(tostream &out, const ExtremaVector &v) {
  out << FullFormatBigReal(v.getE())      << endl;
  out << FullFormatBigReal(v.getMMQuot()) << endl;
  out << v.getDimension() << endl;
  for(size_t i = 0; i < v.getDimension(); i++) {
    out << FullFormatBigReal(v[i]) << endl;
  }
  return out;
}

tistream &operator>>(tistream &in, ExtremaVector &v) {
  BigReal E, mmQuot;
  int dimension;
  in >> E >> mmQuot;
  in >> dimension;
  BigRealVector tmp;
  tmp.setDimension(dimension);
  for(int i = 0; i < dimension; i++) {
    in >> tmp[i];
  }
  v = ExtremaVector(tmp,E,mmQuot);
  return in;
}

// -----------------------------------------------------------------------------------
