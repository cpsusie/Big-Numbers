// BigRealRemes.cpp : Implements the second algorithm of Remes BigReal (multiprecision) arithmetic
// as defined in "A first course in numerical Analysis"
// by Anthony Ralston and Philip Rabinowitz, 2. edition, Page 315...

#include "stdafx.h"
#include <io.h>
#include <Math/Polynomial.h>
#include "BigRealRemes2.h"
#include <Console.h>
#include <fstream>

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

static void throwNoConvergenceException(const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr, format);
  String msg = vformat(format, argptr);
  va_end(argptr);
  throw NoConvergenceException(msg.cstr());
}

static BigReal DEFAULT_MMQUOT_EPS = e(BIGREAL_1,-22);

#define DEFAULT_FLAGS ios::left | ios::showpos | ios::scientific 

class FormatBigReal : public String {
public:
  FormatBigReal(const BigReal &x, int prec = 20, int width = 30, int flags = DEFAULT_FLAGS          ) : String(::toString(x,prec,width,flags)) {}
  FormatBigReal(const Real   &x,                int width = 18, int flags = DEFAULT_FLAGS          ) : String(::toString(x,16  ,width,flags)) {}
  FormatBigReal(int           x,                int width =  2, int flags = ios::right | ios::fixed) : String(::toString(x,0   ,width,flags)) {}
};

static void checkRange(const BigReal &x, const BigReal &left, const BigReal &right) {
  if(x < left || x > right)
    throwException(_T("Remes:checkRange:Invalid argument. x=%s outside range=[%s..%s]")
                  ,x.toString().cstr()
                  ,left.toString().cstr()
                  ,right.toString().cstr());
}

static BigReal signedValue(int sign, const BigReal &x) {
  return sign >= 0 ? x : -x;
}

#define CALLHANDLER(state) { m_state = state; if(m_handler) m_handler->handleData(*this); }

Remes::Remes(RemesTargetFunction &targetFunction, const bool useRelativeError) 
: m_targetFunction(targetFunction)
, m_left(  targetFunction.getInterval().getMin())
, m_right( targetFunction.getInterval().getMax())
, m_digits(targetFunction.getDigits() + 8)
, m_useRelativeError(useRelativeError)
{
  if((m_left > 0) || (m_right < 0)) {
    throwException(_T("Interval [%s;%s] does not contain 0"), m_left.toString().cstr(), m_right.toString().cstr());
  }
  m_hasCoefficients = false;
  m_coefVectorIndex = 0;
  m_handler         = NULL;
  m_MMQuotEps       = DEFAULT_MMQUOT_EPS;
}

Remes::Remes(const Remes &src) 
: m_targetFunction(src.m_targetFunction)
, m_left(  src.m_targetFunction.getInterval().getMin())
, m_right( src.m_targetFunction.getInterval().getMax())
, m_digits(src.m_targetFunction.getDigits() + 8)
, m_useRelativeError(src.m_useRelativeError)
{
  m_hasCoefficients = false;
  m_coefVectorIndex = 0;
  m_handler         = NULL;
  m_MMQuotEps       = src.m_MMQuotEps;
}

void Remes::setMMQuotEpsilon(const BigReal &mmQuotEps) {
  if(mmQuotEps <= 0 || mmQuotEps > 0.5) {
    throwException(_T("Remes::setMMQuotEpsilon:Invalid argument=%s. Must be = ]0;0.5]"),mmQuotEps.toString());
  }
  m_MMQuotEps = mmQuotEps;
}

String Remes::getMapFileName() const {
  String typeString = m_useRelativeError ? _T("RelativeError") : _T("AbsoluteError");
  return m_targetFunction.getName() + typeString + _T(".dat");
}

void Remes::loadExtremaFromFile() {
  extremaMap.load(getMapFileName().cstr());
}

void Remes::saveExtremaToFile() {
  extremaMap.save(getMapFileName().cstr());
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

  initSolveState(M, K);
  initCoefficients();

  if(hasSavedExtrema(M, K)) {
    setExtrema(getBestSavedExtrema(M, K));
  } else if(hasFastInterpolationOfExtrema(M,K)) {
    setExtrema(getFastInitialExtremaByInterpolation(M, K));
  } else {
    setExtrema(getDefaultInitialExtrema(M, K));
  }

  m_coefVectorIndex = 0;
  m_lastMMQuot      = BIGREAL_1;
  m_QEpsilon        = e(BIGREAL_1,-2);

  CALLHANDLER(REMES_INITIALIZE);

  for(m_mainIteration = 1; m_mainIteration <= MAXIT; m_mainIteration++) {
    try {

      findCoefficients();
      
      if(m_E.isZero()) break;

      findExtrema();

      const BigReal maxExtr = fabs(m_errorValue[m_maxExtremumIndex]);
      const BigReal minExtr = fabs(m_errorValue[m_minExtremumIndex]);

      if(maxExtr.isZero()) break;

      m_MMQuot = BIGREAL_1 - rQuot(minExtr, maxExtr, m_digits); // minExtr <= maxExtr => m_MMQuot >= 0

      if(m_MMQuot < m_MMQuotEps) {
        saveExtremaToMap(m_E, m_MMQuot);
        saveExtremaToFile();
        break;
      }

      if(m_mainIteration > 1 && m_handler) {
        if(m_MMQuot > m_lastMMQuot) {
          m_warning = format(_T("MinMaxQuot=%s > last MinMaxQuot=%s"), FormatBigReal(m_MMQuot).cstr(), FormatBigReal(m_lastMMQuot).cstr());
        } else {
          m_warning = _T("");
        }
      }

      m_QEpsilon   = e(BIGREAL_1,BigReal::getExpo10(m_MMQuot) - 5);
      m_lastMMQuot = m_MMQuot;

      CALLHANDLER(REMES_FINALIZE_ITERATION);
    } catch(NoConvergenceException e) {
      if(m_handler) {
        m_warning = e.what();
        CALLHANDLER(REMES_NOCONVERGENCE);
      }
      if(hasNextSolveState()) {
        nextSolveState();
        setExtrema(findInitialExtremaByInterpolation(M,K));
        continue;
      } else {
        throw;
      }
    }
  }
  if(m_mainIteration > MAXIT) {
    throwException(_T("Remes::solve(%d,%d) stopped after %d iterations. No convergence."), M, K, MAXIT);
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
    m_solveStateHighPrecision     = true;
  } else if(!m_solveStateDecrM) {
    m_solveStateDecrM             = true;
  } else {
    throwException(_T("Remes:No Next solveState for Remes(%d,%d):HighPrecision=true, DecrementM=true"),m_M,m_K);
  }
}

void Remes::saveExtremaToMap(const BigReal &E, const BigReal &mmQuot) {
  ExtremaKey key(m_M,m_K);
  Array<ExtremaVector> *a = extremaMap.get(key);
  if(a != NULL) {
    a->add(ExtremaVector(m_extrema, E, mmQuot));
  } else {
    Array<ExtremaVector> a;
    a.add(ExtremaVector(m_extrema, E, mmQuot));
    extremaMap.put(key,a);
  }
}

bool Remes::hasNextSolveState() const {
  return (m_solveStateInterpolationDone && m_solveStateHighPrecision && m_solveStateDecrM) ? false : true;
}

// Return the N+2 extrema (incl. endpoints) of the Chebyshev polynomial of degree N+1, scaled to the x-interval [m_left;m_right], where N=M+K
BigRealVector Remes::getDefaultInitialExtrema(const int M, const int K) {
  const Real left  = getReal(m_left);
  const Real right = getReal(m_right);
  const Real a = (left + right)/2.0;
  const Real b = (left - right)/2.0;
  const int  N = M + K;

  BigRealVector result(N+2);
  for(int i = 0; i <= N+1; i++) {
    result[i] = a + b * cos(M_PI * i / (N+1));
  }
  return result;
}

BigRealVector Remes::findFinalExtrema(const int M, const int K, const bool highPrecision) {
  const BigReal mmQuot = highPrecision ? DEFAULT_MMQUOT_EPS : e(BIGREAL_1,-7);

  for(bool hasSolved = false;;) {
    Array<ExtremaVector> *a = extremaMap.get(ExtremaKey(M,K));
    if(a != NULL) {
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

  throwException(_T("Remes::getFinalExtrema(%d,%d):Cannot find extremaVector with MinMaxQuot < %s, though it should exist"), M, K, mmQuot.toString().cstr());
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
    const Real y = getReal(finalExtr[i] - initialExtr[i]); // first and last y will be 0
    result.add(DataPoint(x,y));
  }
  return result;
}

Real InterpolationFunction::operator()(const Real &x) {
  const int maxX = (int)m_initialExtr.getDimension()-1;
  if(x == 0 || x == maxX) {
    return getReal(m_initialExtr[getInt(x)]);
  } else if(x < 0 || x > maxX) {
    throwException(_T("InterpolationFunction::Invalid argument. x=%s. x must be in the interval [0..%d]"), toString(x).cstr(), maxX);
  }

  return getReal(m_initialExtr[getInt(x)]) + CubicSpline::operator()(x/maxX);
}

bool Remes::hasSavedExtrema(const int M, const int K) {
  return extremaMap.get(ExtremaKey(M,K)) != NULL;
}

const ExtremaVector &Remes::getBestSavedExtrema(const int M, const int K) {
  const Array<ExtremaVector> *a = extremaMap.get(ExtremaKey(M,K));
  if(a == NULL) {
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

void Remes::findCoefficients() {
  static const BigReal c1 = 0.5;

  const int MAXIT = 700;

  BigRealMatrix A(m_N+2, m_N+2, m_digits);
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

    m_coefficient      = LU.solve(m_functionValue);
    m_E                = m_coefficient[m_N+1];
    m_searchEIteration = 1;
    m_hasCoefficients  = true;
    m_coefVectorIndex++;
    CALLHANDLER(REMES_SEARCH_E);
  } else {
    // for m_K > 0 we have to iterate
    for(m_searchEIteration = 1; m_searchEIteration <= MAXIT; m_searchEIteration++) {
      for(int r = 0, s = 1; r <= m_N+1; r++, s = -s) {
        const BigReal &xr = m_extrema[r];
        A(r,m_M+1) = xr * (signedValue(s,m_E) - m_functionValue[r]);
        for(int c = m_M + 2; c <= m_N; c++) {
          A(r,c) = xr * A(r,c-1);
        }
      }

      const BigRealLUMatrix LU(A);

      m_coefficient      = LU.solve(m_functionValue);
      m_nextE            = m_coefficient[m_N+1];
      m_hasCoefficients  = true;
      m_coefVectorIndex++;
      if(m_nextE.isZero()) break;

      m_Q = fabs(BIGREAL_1 - rQuot(Min(fabs(m_E),fabs(m_nextE)), Max(fabs(m_E),fabs(m_nextE)), m_digits));
      if(m_Q < m_QEpsilon) break;
      CALLHANDLER(REMES_SEARCH_E);
      m_E = rProd(c1, m_E + m_nextE, m_digits);
    }
    if(m_searchEIteration > MAXIT) {
      throwNoConvergenceException(_T("Remes:findCoefficients:No convergence after %d iterations."), MAXIT);
    }
    CALLHANDLER(REMES_SEARCH_E);
  }
}

void Remes::findExtrema() {
  const BigRealVector save = m_extrema;

  m_minExtremumIndex = -1; 
  m_maxExtremumIndex = -1;
  m_extremaCount     =  0;
  int prevSign;
  for(int i = 0; i <= m_N+1; i++) {
    const BigReal l = i == 0       ? m_left  : rQuot(save[i-1] + 2.0 * save[i], 3, m_digits);
    const BigReal r = i == m_N + 1 ? m_right : rQuot(save[i+1] + 2.0 * save[i], 3, m_digits);
    setExtremum(i, findExtremum(l, save[i], r, 0));
    const BigReal &errorValue = m_errorValue[i];
    m_extremaCount++;
    if(m_extremaCount == 1) {
      m_minExtremumIndex = m_maxExtremumIndex = i;
      prevSign = sign(errorValue);
    } else {
      const int s = sign(errorValue);
      if(compareAbs(errorValue, m_errorValue[m_minExtremumIndex]) < 0) {
        m_minExtremumIndex = i;
      } else if(compareAbs(errorValue, m_errorValue[m_maxExtremumIndex]) > 0) {
        m_maxExtremumIndex = i;
      }
      if(s == prevSign) {
        throwNoConvergenceException(_T("Remes:findExtrema:Sign(errorFunction) doesn't change between the found extrema."));
      }
      prevSign = s;
    }
    m_maxError = fabs(m_errorValue[m_maxExtremumIndex]);
    CALLHANDLER(REMES_SEARCH_EXTREMA);
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
  return compare(p2.m_y,p1.m_y);
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
    m_warning = e.what();
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
    throwException(_T("Remes::setExtrema:Invalid dimension of extremaVector=%d, N=%d => expected dimension=%d"), extrema.getDimension(), m_N, m_N+2);
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
}

BigReal Remes::approximation(const BigReal &x) const {
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
    return rQuot(sum1, sum2 * x + BIGREAL_1, m_digits);
  } else {
    BigReal sum = m_coefficient[m_M];
    for(int i = m_M - 1; i >= 0; i--) {
      sum = sum * x + m_coefficient[i];
    }
    return sum;
  }
}

BigReal Remes::errorFunction(const BigReal &x) const {
  return m_useRelativeError ? (BIGREAL_1 - sFunction(x) * approximation(x))
                            : (m_targetFunction(x) - approximation(x) /* * sFunction(x)==1 */ )
                            ;
}

BigReal Remes::sFunction(const BigReal &x) const {
  return m_useRelativeError ? rQuot(BIGREAL_1,m_targetFunction(x),m_digits) : BIGREAL_1;
}

BigReal Remes::targetFunction(const BigReal &x) const {
  return m_useRelativeError ? BIGREAL_1 : m_targetFunction(x);
}

void Remes::getErrorPlot(int n, Point2DArray &pa) const {
  if(!hasErrorPlot()) return;
  pa.clear();
  const BigReal step = rQuot(m_right - m_left,n,m_digits);
  const BigReal stop = m_right + step*0.5;
  for(BigReal x = m_left; x <= stop; x += step) {
    pa.add(Point2D(getDouble(x), getDouble(errorFunction(x))));
  }
}

String Remes::getSearchEString() const {
  if(m_K == 0) {
    return format(_T("New E:%s"), FormatBigReal(m_E, 25).cstr());
  } else {
    String str;
    str  = format(_T("Search E. Iteration %3d. Stop when Q < %s.\n"), m_searchEIteration, m_QEpsilon.toString().cstr());
    str += format(_T("Last E:%s\n"), FormatBigReal(m_E ,25).cstr());
    str += format(_T("New  E:%s\n"), FormatBigReal(m_nextE, 25).cstr());
    str += format(_T("Q=1-min(|Last E|, |New E|)/max(|Last E|, |New E|)\nQ=%s"), FormatBigReal(m_Q).cstr());
    return str;
  }
}

String Remes::getExtremumString(int index) const {
  const TCHAR *comment = _T("");
  if(index == m_minExtremumIndex) {
    comment = _T("<-- Min.");
  } else if(index == m_maxExtremumIndex) {
    comment = _T("<-- Max.");
  }
  return format(_T("Extr[%2d]:%s %s%s"), index, FormatBigReal(m_extrema[index]).cstr(), FormatBigReal(m_errorValue[index]).cstr(), comment);
}

StringArray Remes::getCoefficientStringArray() const {
  StringArray result;
  for(int i = 0; i <= m_M; i++) {
    result.add(format(_T("a[%2d] = %s     %s"), i, FormatBigReal(m_coefficient[i],25,35).cstr(), FormatBigReal(getReal(m_coefficient[i])).cstr()));
  }
  result.add(format(_T("b[ 0] = +1")));
  for(int i = m_M + 1; i <= m_N; i++) {
    result.add(format(_T("b[%2d] = %s     %s"), i-m_M,FormatBigReal(m_coefficient[i],25,35).cstr(), FormatBigReal(getReal(m_coefficient[i])).cstr()));
  }
  result.add(format(_T("Max.error:%s"), FormatBigReal(m_maxError).cstr()));
  return result;
}

String Remes::getCFunctionString(bool useDouble80) const {
  const String header = getHeaderString();

  const TCHAR *typeStr = useDouble80 ? _T("Double80") : _T("double");
  int typeSize = useDouble80 ? 10 : sizeof(double);

  String str;
  if(useDouble80) {
    str = format(_T("#include <Double80.h>\n\n"));
  }

  str += format(_T("static const %s coef[%d] = {\n"), typeStr, m_N+1);
  for(int i = 0; i <= m_N; i++) {
    Double80 d80 = getDouble80(m_coefficient[i]);
    double   d64 = getDouble(  m_coefficient[i]);
    BYTE *byte = useDouble80 ? (BYTE*)&d80 : (BYTE*)&d64;
    str += format(useDouble80 ? _T("  Double80::bytesToDouble80((unsigned char*)\"") : _T("  *((double*)\""));
    for(int j = 0; j < typeSize; j++) {
      str += format(_T("\\x%02x"), *(byte++));
    }
    str += format(_T("%s // %s\n"), (i==m_N)?_T("\") "):_T("\"),"), toString(d80,17,24,ios::scientific).cstr());
  }
  str += _T("};\n\n");

  str += format(_T("double approximation%02d%02d(%s x) {\n"), m_M, m_K, typeStr);

  if(m_K) {
    str += format(_T("  %s sum1 = coef[%d];\n"), typeStr, m_M);
    str += format(_T("  %s sum2 = coef[%d];\n"), typeStr, m_N);
    str += format(_T("  int i;\n"));
    if(m_M - 1 >= 0) {
      str += format(_T("  for(i = %d; i >= 0; i--) sum1 = sum1 * x + coef[i];\n"), m_M - 1);
    }
    if(m_N - 1 > m_M) {
      str += format(_T("  for(i = %d; i > %d; i--) sum2 = sum2 * x + coef[i];\n"), m_N - 1, m_M);
    }
    str += format(_T("  return %s;\n"), useDouble80 ? _T("getDouble(sum1 / (sum2 * x + 1.0))") : _T("sum1 / (sum2 * x + 1.0)"));
  } else {
    str += format(_T("  %s sum = coef[%d];\n"), typeStr, m_N);
    if(m_N - 1 >= 0) {
      str += format(_T("  for(int i = %d; i >= 0; i--) sum = sum * x + coef[i];\n"), m_N - 1);
    }
    str += format(_T("  return %s;\n"), useDouble80 ? _T("getDouble(sum)") : _T("sum"));
  }
  str += _T("}\n");
  return header + str;
}

String Remes::getJavaFunctionString() const {
  const String header = getHeaderString();

  String str = format(_T("    private static final double coef[] = {\n"));
  for(int i = 0; i <= m_N; i++) {
    const double coef = getDouble(m_coefficient[i]);
    const unsigned __int64 *c = (unsigned __int64*)&coef;
    str += format(_T("        Double.longBitsToDouble(0x%I64xL)"), *c);
    str += format(_T("%s // %20.16le\n"), i == m_N ? _T(" ") : _T(","), coef);
  }
  str += format(_T("    };\n\n"));

  str += format(_T("    public static double approximation%02d%02d(double x) {\n"), m_M, m_K);
  if(m_K) {
    str += format(_T("        double sum1 = coef[%d];\n"), m_M);
    str += format(_T("        double sum2 = coef[%d];\n"), m_N);
    if(m_M - 1 >= 0) {
      str += format(_T("        for(int i = %d; i >= 0; i--) sum1 = sum1 * x + coef[i];\n"), m_M - 1);
    }
    if(m_N - 1 > m_M) {
      str += format(_T("        for(int i = %d; i > %d; i--) sum2 = sum2 * x + coef[i];\n"), m_N - 1, m_M);
    }
    str += format(_T("        return sum1 / (sum2 * x + 1.0);\n"));
  } else {
    str += format(_T("        double sum = coef[%d];\n"), m_N);
    if(m_N - 1 >= 0) {
      str += format(_T("        for(int i = %d; i >= 0; i--) sum = sum * x + coef[i];\n"), m_N - 1);
    }
    str += _T("        return sum;\n");
  }
  str += _T("    }\n");
  return header + str;
}

String Remes::getHeaderString() const {
  return format(_T("// (%d,%d)-Minimax-approximation of %s in [%le,%le] with max %serror = %21.15le\n")
               ,m_M,m_K
               ,m_targetFunction.getName().cstr()
               ,getDouble(m_left)
               ,getDouble(m_right)
               ,m_useRelativeError ? _T("relative ") : _T("")
               ,getDouble(m_maxError) 
               );
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
  if(ACCESS(name,0) < 0) {
    return;
  }

  tifstream in(name.cstr());
  load(in);
  in.close();
}

static int extremaVectorCmp(const ExtremaVector &v1, const ExtremaVector &v2) {
  return sign(v1.getMMQuot() - v2.getMMQuot());
}

typedef Entry<ExtremaKey, Array<ExtremaVector> > ExtremaMapEntry;

void ExtremaHashMap::save(tostream &out) {
  Iterator<ExtremaMapEntry> it = entrySet().getIterator();
  out << size() << endl;
  while(it.hasNext()) {
    ExtremaMapEntry &e = it.next();
    out << e.getKey() << endl;
    Array<ExtremaVector> &a = e.getValue();
    if(a.size() > 1) {
      a.sort(extremaVectorCmp);
      while(a.size() > 1) {
        a.removeLast();
      }
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
  out << key.m_M << _T(") ") << key.m_K;
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
