// BigRealRemes.cpp : Implements the second algorithm of Remes BigReal (multiprecision) arithmetic
// as defined in "A first course in numerical Analysis"
// by Anthony Ralston and Philip Rabinowitz, 2. edition, Page 315...

#include "stdafx.h"
#include <io.h>
#include <fstream>
#include "BigRealRemes2.h"
#include "MultiExtremaFinder.h"

class NoConvergenceException : public Exception {
public:
  NoConvergenceException(const TCHAR *method, const TCHAR *msg) : Exception(format(_T("%s:%s"), method, msg)) {
  }
};

static void throwNoConvergenceException(const TCHAR *method, const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format, argptr);
  va_end(argptr);
  throw NoConvergenceException(method, msg.cstr());
}

static void throwSameSignException(const TCHAR *method) {
  throwNoConvergenceException(method, _T("Sign(errorFunction) doesn't change between the found extrema."));
}

static void checkRange(const TCHAR *method, const BigReal &x, const BigReal &left, const BigReal &right) {
  if((x < left) || (x > right))
    throwInvalidArgumentException(method
                                 ,_T("x=%s outside range=[%s..%s]")
                                 ,x.toString().cstr()
                                 ,left.toString().cstr()
                                 ,right.toString().cstr());
}

static BigReal signedValue(int sign, const BigReal &x) {
  return (sign >= 0) ? x : -x;
}

const ConstBigReal Remes::s_defaultMMQuotEps = e(BIGREAL_1,-22);
#define DEFAULT_SEARCHEMAXIT 700

Remes::Remes(RemesTargetFunction &targetFunction, const bool useRelativeError) 
: m_targetFunction(targetFunction)
, m_domain(targetFunction.getDomain())
, m_digits(targetFunction.getDigits() + 8)
, m_useRelativeError(useRelativeError)
{
  checkInterval();
  loadExtremaFromFile();

  m_reduceToInterpolate  = false;
  m_hasCoefficients      = false;
  m_coefVectorIndex      = 0;
  m_searchEMaxIterations = DEFAULT_SEARCHEMAXIT;
  m_MMQuotEps            = s_defaultMMQuotEps;
  m_state                = REMES_INITIALIZED;
}

Remes::Remes(const Remes &src) 
: PropertyContainer(src)
, m_targetFunction(      src.m_targetFunction)
, m_domain(              src.m_domain)
, m_digits(              src.m_digits)
, m_useRelativeError(    src.m_useRelativeError)
, m_searchEMaxIterations(src.m_searchEMaxIterations)
{
  checkInterval();
  m_reduceToInterpolate = false;
  m_hasCoefficients     = false;
  m_coefVectorIndex     = 0;
  m_MMQuotEps           = s_defaultMMQuotEps;
  m_state                = REMES_INITIALIZED;
}

void Remes::checkInterval() {
  if(!m_domain.contains(BIGREAL_0)) {
    throwException(_T("Interval [%s;%s] does not contain 0"), m_domain.getFrom().toString().cstr(), m_domain.getTo().toString().cstr());
  }
}

void Remes::setMMQuotEpsilon(const BigReal &mmQuotEps) {
  if(mmQuotEps <= 0 || mmQuotEps > 0.5) {
    throwException(_T("%s:Invalid argument=%s. Must be = ]0;0.5]")
                  ,__TFUNCTION__
                  , mmQuotEps.toString().cstr());
  }
  m_MMQuotEps = mmQuotEps;
}

void Remes::solve(const UINT M, const UINT K) {
  DEFINEMETHODNAME;

  const int MAXIT = 100;

  m_M = M;
  m_K = K;
  m_N = m_M + m_K;

  setProperty(WARNING, m_warning, _T(" "));

  const UINT dimension = m_N + 2;
  m_coefficientVector.setDimAndPrecision(dimension, m_digits);
  m_extrema.setDimAndPrecision(          dimension, m_digits);
  m_functionValue.setDimAndPrecision(    dimension, m_digits);
  m_errorValue.setDimAndPrecision(       dimension, m_digits);
  m_extremaStringArray.clear();
  for(UINT i = 0; i < dimension; i++) {
    m_extremaStringArray.add(_T(""));
  }
  initSolveState();
  initCoefficientVector(dimension);

  setProperty(REMES_STATE, m_state, REMES_SOLVE_STARTED);

  if(hasSavedExtrema(M, K)) {
    setExtrema(getBestSavedExtrema(M, K));
  } else if(hasFastInterpolationOfExtrema(M,K)) {
    setExtrema(getFastInitialExtremaByInterpolation(M, K));
  } else {
    setExtrema(getDefaultInitialExtrema(M, K));
  }

RestartMainIteration:
  m_coefVectorIndex = 0;
  m_lastMMQuot      = BIGREAL_1;
  m_QEpsilon        = e(BIGREAL_1,-2);

  for(setProperty(MAINITERATION, m_mainIteration, 1); m_mainIteration <= MAXIT; setProperty(MAINITERATION, m_mainIteration, m_mainIteration+1)) {
    try {
      findCoefficients();
      
      if(m_E.isZero()) {
        break;
      }

      findExtrema();

      const BigReal maxExtr = getMaxAbsExtremumValue();
      const BigReal minExtr = getMinAbsExtremumValue();

      if(maxExtr.isZero()) {
        break;
      }

      setProperty(MMQUOT, m_MMQuot, BIGREAL_1 - rQuot(minExtr, maxExtr, m_digits)); // minExtr <= maxExtr => m_MMQuot >= 0

      if(m_MMQuot < m_MMQuotEps) {
        saveExtremaToMap(m_E, m_MMQuot);
        saveExtremaToFile();
        break;
      }

      if(m_MMQuot <= m_lastMMQuot) {
        setProperty(WARNING, m_warning, _T(""));
      } else {
        const String msg = format(_T("MinMaxQuot=%s > last MinMaxQuot=%s"), FormatBigReal(m_MMQuot).cstr(), FormatBigReal(m_lastMMQuot).cstr());
        if(m_mainIteration < 2) {
          setProperty(WARNING, m_warning, msg);
        } else {
          throwNoConvergenceException(method, _T("%s"), msg.cstr());
        }
      }

      m_QEpsilon   = e(BIGREAL_1,BigReal::getExpo10(m_MMQuot) - 25);
      m_lastMMQuot = m_MMQuot;

    } catch(NoConvergenceException e) {
      setProperty(WARNING, m_warning, e.what());
      if(hasNextSolveState()) {
        nextSolveState();
        setExtrema(findInitialExtremaByInterpolation(M,K));
        setProperty(E, m_E, (BigReal&)BIGREAL_0);
        goto RestartMainIteration;
      } else {
        throw;
      }
    }
  }
  if(m_mainIteration > MAXIT) {
    throwException(_T("%s(%d,%d) stopped after %d iterations. No convergence")
                  ,__TFUNCTION__
                  , M, K, MAXIT);
  }
  setProperty(REMES_STATE, m_state, REMES_SUCCEEDED);
}

void Remes::initCoefficientVector(size_t dimension) {
  BigRealVector v;
  v.setDimAndPrecision(dimension, m_digits);

  for(UINT i = 0; i <= m_N + 1; i++) v[i] = 0;
  setProperty(E                , m_E                , (BigReal&)BIGREAL_0);
  setProperty(COEFFICIENTVECTOR, m_coefficientVector, v);
}

void Remes::initSolveState() {
  m_solveStateInterpolationDone = false;
  m_solveStateHighPrecision     = false;
  m_solveStateDecrM             = (m_M > 1) && ((m_K == 0) || (m_M > m_K));
}

void Remes::nextSolveState() {
  if(m_reduceToInterpolate) {
    m_solveStateHighPrecision     = true;
  }
  if(!m_solveStateInterpolationDone) {
    m_solveStateInterpolationDone = true;
  } else if(!m_solveStateHighPrecision) {
    m_solveStateHighPrecision     = true;
  } else if(!m_solveStateDecrM && (m_M > 1)) {
    m_solveStateDecrM             = true;
  } else {
    throwException(_T("%s:No Next solveState for Remes(%u,%u):HighPrecision=true, DecrementM=true")
                  ,__TFUNCTION__
                  ,m_M,m_K);
  }
}

bool Remes::hasNextSolveState() const {
  if(m_reduceToInterpolate) {
    return true;
  }
  return (m_solveStateInterpolationDone && m_solveStateHighPrecision && m_solveStateDecrM) ? false : true;
}


void Remes::findCoefficients() {
  DEFINEMETHODNAME;
  static const BigReal c1 = BIGREAL_HALF;

  setProperty(REMES_STATE, m_state, REMES_SEARCH_COEFFICIENTS);

  BigRealMatrix A(m_N+2, m_N+2, m_digits);
  int s = 1;
  for(UINT r = 0; r <= m_N+1; r++, s = -s) {
    const BigReal &xr = m_extrema[r];

    checkRange(method, xr, m_domain.getFrom(), m_domain.getTo());

    A(r,0) = sFunction(xr);
    for(UINT c = 1; c <= m_M; c++) {
      A(r,c) = rProd(xr, A(r,c-1), m_digits);
    }
    A(r,m_N+1) = s;

    m_functionValue[r] = targetFunction(xr);
  }

  if(m_K == 0) { 
    // The coefficients a[0]..a[m] and E can be found by solving the linear system of equations
    BigRealLUMatrix LU(A);

    setProperty(COEFFICIENTVECTOR, m_coefficientVector, LU.solve(m_functionValue));
    setProperty(E, m_E           , m_coefficientVector[m_N+1]);
    m_hasCoefficients  = true;
    m_coefVectorIndex++;
  } else {
    // for m_K > 0 we have to iterate
    for(setProperty(SEARCHEITERATION, m_searchEIteration, 1); m_searchEIteration <= m_searchEMaxIterations; setProperty(SEARCHEITERATION, m_searchEIteration, m_searchEIteration+1)) {
      if(m_reduceToInterpolate) {
        throwNoConvergenceException(__TFUNCTION__, _T("Reduce M,K to interpolate new initial extrame"));
      }

      s = 1;
      for(UINT r = 0; r <= m_N+1; r++, s = -s) {
        const BigReal &xr = m_extrema[r];
        A(r,m_M+1) = rProd(xr, (signedValue(s,m_E) - m_functionValue[r]), m_digits);
        for(UINT c = m_M + 2; c <= m_N; c++) {
          A(r,c) = rProd(xr, A(r,c-1), m_digits);
        }
      }

      const BigRealLUMatrix LU(A);

      setProperty(COEFFICIENTVECTOR, m_coefficientVector, LU.solve(m_functionValue));
      m_nextE            = m_coefficientVector[m_N+1];
      m_hasCoefficients  = true;
      m_coefVectorIndex++;
      if(m_nextE.isZero()) break;

      setProperty(Q, m_Q, fabs(BIGREAL_1 - rQuot(Min(fabs(m_E), fabs(m_nextE)), Max(fabs(m_E), fabs(m_nextE)), m_digits)));
      if(m_Q < m_QEpsilon) {
        break;
      }
      setProperty(E, m_E, rProd(c1, m_E + m_nextE, m_digits));
    }
    if(m_searchEIteration > m_searchEMaxIterations) {
      throwNoConvergenceException(__TFUNCTION__, _T("No convergence after %d iterations"), m_searchEMaxIterations);
    }
  }
}

void Remes::findExtrema() {
  DEFINEMETHODNAME;
  const BigRealVector save = m_extrema;
  setProperty(REMES_STATE, m_state, REMES_SEARCH_EXTREMA);
  resetExtremaCount();

#ifndef MULTITHREADEDEXTREMAFINDER
  int prevSign = setExtremum(0, m_left);
  DigitPool *pool = m_left.getDigitPool();
  for(int i = 1; i <= m_N; i++) {
    const BigReal l = rQuot(save[i-1] + 2.0 * save[i], 3, m_digits);
    const BigReal r = rQuot(save[i+1] + 2.0 * save[i], 3, m_digits);
    const int s = setExtremum(i, findExtremum(l, save[i], r, pool));
    if(s == prevSign) {
      throwSameSignException(method);
    }
    prevSign = s;
  }
  if(setExtremum(m_N+1, m_right) == prevSign) {
    throwSameSignException(method);
  }
#else // MULTITHREADEDEXTREMAFINDER
  MultiExtremaFinder multiExtremaFinder(this);
  for(UINT i = 1; i <= m_N; i++) {
    const BigReal l = rQuot(save[i-1] + 2.0 * save[i], 3, m_digits);
    const BigReal r = rQuot(save[i+1] + 2.0 * save[i], 3, m_digits);
    multiExtremaFinder.insertJob(i, l, r, save[i]);
  }
  setExtremum(0, m_domain.getFrom());
  multiExtremaFinder.execute();
  setExtremum(m_N+1, m_domain.getTo());
  checkExtremaSigns(method);
#endif // MULTITHREADEDEXTREMAFINDER

  setProperty(MAXERROR, m_maxError, fabs(m_errorValue[m_maxExtremumIndex]));
}

class Point {
public:
  BigReal m_x, m_y;
  inline Point(const BigReal &x, const BigReal &y) : m_x(x), m_y(y) {
  }
};

static int pointCompareY(const Point &p1, const Point &p2) {
  return compare(p2.m_y,p1.m_y);
}

static BigReal sqr(const BigReal &x, UINT digits) {
  return rProd(x, x, digits);
}

static BigReal inverseInterpolate(const Point &p1, const Point &p2, const Point &p3, UINT digits) {
  DigitPool *pool = p1.m_x.getDigitPool();
  const BigReal &c1 = pool->getHalf();

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

BigReal Remes::findExtremum(const BigReal &from, const BigReal &middle, const BigReal &to, DigitPool *pool) {
  DEFINEMETHODNAME;
  const UINT STEPCOUNT = 20;
  BigReal l(pool), r(pool), m(pool), brStepCount(pool);
  l = from; r = to; m = middle;
  brStepCount = STEPCOUNT - 1;
  const int errorSign = sign(errorFunction(m));

  for(UINT depth = 0;; depth++) {
    Array<Point> plot;

    checkRange(method, m, l, r);

    if(l == r) {
      return l;
    }

    const BigReal step = rQuot(r-l,brStepCount,m_digits);
    BigReal x = l;
    for(UINT count = 0;;) {
      checkRange(method, x, l, r);
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
      setProperty(WARNING, m_warning, e.what());
      x = l - 1;
    }
    if(x < l || x > r) {
      x = plot[0].m_x;
    } else { // l <= x <= r
      if(signedValue(errorSign, errorFunction(x)) < plot[0].m_y) {
        x = plot[0].m_x;
      }
    }
    if(x <= m_domain.getFrom()) {
      x = m_domain.getFrom();
      return x;
    } else if(x >= m_domain.getTo()) {
      x = m_domain.getTo();
      return x;
    } else if(depth >= 9) {
      return x;
    } else {
      const BigReal dx = e(step,-1);
      l = x - dx; m = x; r = x + dx;
    }
  }
}

void Remes::setExtrema(const BigRealVector &extrema) {
  const size_t dim = extrema.getDimension();
  if(dim != m_N + 2) {
    throwInvalidArgumentException(__TFUNCTION__
                                 ,_T("Dimension of extremaVector=%s, N=%u => Expected dimension=%u")
                                 ,format1000(extrema.getDimension()).cstr(), m_N, m_N+2);
  }
  resetExtremaCount();
  m_notifyExtremaCountChanged = false;
  for(size_t i = 0; i < dim; i++) {
    if(i == dim-1) {
      m_notifyExtremaCountChanged = true;
    }
    setExtremum((UINT)i, extrema[i]);
  }
}

void Remes::setExtrema(const ExtremaVector &extrema) {
  setExtrema((const BigRealVector&)extrema);
  setProperty(E, m_E, extrema.getE());
}

void Remes::resetExtremaCount() {
  m_minExtremumIndex = -1;
  m_maxExtremumIndex = -1;
  for(size_t i = 0; i < m_extremaStringArray.size(); i++) {
    m_extremaStringArray[i] = _T("");
  }
  setProperty(EXTREMACOUNT, m_extremaCount, 0);
}

int Remes::setExtremum(const UINT index, const BigReal &x) {
  m_extrema[index] = x;
  const BigReal &errorValue = m_errorValue[index] = errorFunction(x);
  if(m_extremaCount == 0) {
    m_minExtremumIndex = m_maxExtremumIndex = index;
  } else {
    if(compareAbs(errorValue, m_errorValue[m_minExtremumIndex]) < 0) {
      m_minExtremumIndex = index;
    } else if(compareAbs(errorValue, m_errorValue[m_maxExtremumIndex]) > 0) {
      m_maxExtremumIndex = index;
    }
  }
  m_extremaStringArray[index] = getExtremumString(index);
  if(m_notifyExtremaCountChanged) {
    setProperty(EXTREMACOUNT, m_extremaCount, m_extremaCount+1);
  } else {
    m_extremaCount++;
  }
  return sign(errorValue);
}

void Remes::checkExtremaSigns(const TCHAR *method) const {
  assert(m_extremaCount == m_extrema.getDimension());
  int s = sign(m_errorValue[0]);
  for (int i = 1; i < m_extremaCount; i++) {
    const int s1 = sign(m_errorValue[i]);
    if (s1 != -s) {
      throwSameSignException(method);
    }
    s = s1;
  }
}

BigReal Remes::approximation(const BigReal &x) const {
  DigitPool *pool = x.getDigitPool();
  if(m_K) {
    BigReal sum1(pool), sum2(pool);
    sum1 = m_coefficientVector[m_M];
    sum2 = m_coefficientVector[m_N];
    for(int i = m_M; --i >= 0;) {
      sum1 = sum1 * x + m_coefficientVector[i];
    }
    for(UINT j = m_N; --j > m_M;) {
      sum2 = sum2 * x + m_coefficientVector[j];
    }
    return rQuot(sum1, sum2 * x + pool->get1(), m_digits, pool);
  } else {
    BigReal sum(pool);
    sum = m_coefficientVector[m_M];
    for(int i = m_M; --i >= 0;) {
      sum = sum * x + m_coefficientVector[i];
    }
    return sum;
  }
}

BigReal Remes::errorFunction(const BigReal &x) const {
  DigitPool *pool = x.getDigitPool();
  return m_useRelativeError ? (pool->get1() - sFunction(x) * approximation(x))
                            : (m_targetFunction(x) - approximation(x) /* * sFunction(x)==1 */ )
                            ;
}

BigReal Remes::sFunction(const BigReal &x) const {
  DigitPool *pool = x.getDigitPool();
  return m_useRelativeError ? rQuot(pool->get1(), m_targetFunction(x), m_digits) : pool->get1();
}

BigReal Remes::targetFunction(const BigReal &x) const {
  return m_useRelativeError ? x.getDigitPool()->get1() : m_targetFunction(x);
}

void Remes::getErrorPlot(UINT n, Point2DArray &pa) const {
  if(!hasErrorPlot()) return;
  pa.clear();
  const BigReal step = rQuot(m_domain.getLength(), n, m_digits);
  const BigReal stop = m_domain.getTo() + step*0.5;
  for(BigReal x = m_domain.getFrom(); x <= stop; x += step) {
    pa.add(Point2D(getDouble(x), getDouble(errorFunction(x))));
  }
}
