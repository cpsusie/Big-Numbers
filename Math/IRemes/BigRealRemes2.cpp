// BigRealRemes.cpp : Implements the second algorithm of Remes BigReal (multiprecision) arithmetic
// as defined in "A first course in numerical Analysis"
// by Anthony Ralston and Philip Rabinowitz, 2. edition, Page 315...

#include "stdafx.h"
#include <io.h>
#include <Math/Polynomial.h>
#include "BigRealRemes2.h"
#include <Console.h>
#include <fstream>

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

const TCHAR *Remes::s_stateName[] = {
  _T("INITIALIZE")
 ,_T("REMES_SEARCH_E")
 ,_T("REMES_SEARCH_EXTREMA")
 ,_T("REMES_FINALIZE_ITERATION")
 ,_T("REMES_NOCONVERGENCE")
};

static BigReal DEFAULT_MMQUOT_EPS = e(BIGREAL_1,-22);

#define DEFAULT_FLAGS ios::left | ios::showpos | ios::scientific 

class FormatBigReal : public String {
public:
  FormatBigReal(const BigReal &x, int prec = 20, int width = 30, int flags = DEFAULT_FLAGS          ) : String(::toString(x,prec,width,flags)) {}
  FormatBigReal(const Real   &x,                 int width = 18, int flags = DEFAULT_FLAGS          ) : String(::toString(x,16  ,width,flags)) {}
  FormatBigReal(int           x,                 int width =  2, int flags = ios::right | ios::fixed) : String(::toString(x,0   ,width,flags)) {}
};

static void checkRange(const TCHAR *method, const BigReal &x, const BigReal &left, const BigReal &right) {
  if((x < left) || (x > right))
    throwInvalidArgumentException(method
                                 ,_T("x=%s outside range=[%s..%s]")
                                 ,x.toString().cstr()
                                 ,left.toString().cstr()
                                 ,right.toString().cstr());
}

static BigReal signedValue(int sign, const BigReal &x) {
  return sign >= 0 ? x : -x;
}


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
  loadExtremaFromFile();

  m_hasCoefficients = false;
  m_coefVectorIndex = 0;
  m_MMQuotEps       = DEFAULT_MMQUOT_EPS;
}

Remes::Remes(const Remes &src) 
: PropertyContainer(src)
, m_targetFunction(src.m_targetFunction)
, m_left(  src.m_targetFunction.getInterval().getMin())
, m_right( src.m_targetFunction.getInterval().getMax())
, m_digits(src.m_targetFunction.getDigits() + 8)
, m_useRelativeError(src.m_useRelativeError)
{
  m_hasCoefficients = false;
  m_coefVectorIndex = 0;
  m_MMQuotEps       = src.m_MMQuotEps;
}


void Remes::setMMQuotEpsilon(const BigReal &mmQuotEps) {
  if(mmQuotEps <= 0 || mmQuotEps > 0.5) {
    throwException(_T("%s:Invalid argument=%s. Must be = ]0;0.5]")
                  ,_T(__FUNCTION__)
                  , mmQuotEps.toString());
  }
  m_MMQuotEps = mmQuotEps;
}

String Remes::getMapFileName() const {
  FileNameSplitter info(__FILE__);
  const String typeString = m_useRelativeError ? _T("RelativeError") : _T("AbsoluteError");
  const String fileName = m_targetFunction.getName() + typeString;
  return info.setFileName(fileName).setExtension(_T("dat")).getAbsolutePath();
}

void Remes::loadExtremaFromFile() {
  s_extremaMap.load(getMapFileName());
}

void Remes::saveExtremaToFile() {
  s_extremaMap.save(getMapFileName());
}

void Remes::solve(const int M, const int K) {
  const int MAXIT = 100;

  m_M = M;
  m_K = K;
  m_N = m_M + m_K;

  const int dimension = m_N + 2;
  m_coefficientVector.setDimension(dimension);
  m_extrema.setDimension(dimension);
  m_functionValue.setDimension(dimension);
  m_errorValue.setDimension(dimension);

  initSolveState(M, K);
  initCoefficientVector(dimension);

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

  setProperty(REMES_STATE, m_state, REMES_INITIALIZE);

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

      if((m_mainIteration > 1) && (m_MMQuot > m_lastMMQuot)) {
        setProperty(WARNING, m_warning, format(_T("MinMaxQuot=%s > last MinMaxQuot=%s"), FormatBigReal(m_MMQuot).cstr(), FormatBigReal(m_lastMMQuot).cstr()));
      } else {
        setProperty(WARNING, m_warning, _T(""));
      }

      m_QEpsilon   = e(BIGREAL_1,BigReal::getExpo10(m_MMQuot) - 25);
      m_lastMMQuot = m_MMQuot;

    } catch(NoConvergenceException e) {
      setProperty(WARNING, m_warning, e.what());
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
    throwException(_T("%s(%d,%d) stopped after %d iterations. No convergence")
                  ,_T(__FUNCTION__)
                  , M, K, MAXIT);
  }
}

void Remes::initCoefficientVector(int dimension) {
  BigRealVector v;
  v.setDimension(dimension);

  for(int i = 0; i <= m_N + 1; i++) v[i] = 0;
  m_E = 0;
  setProperty(COEFFICIENTVECTOR, m_coefficientVector, v);
}

void Remes::initSolveState(const int M, const int K) {
  m_solveStateInterpolationDone = false;
  m_solveStateHighPrecision     = false;
  m_solveStateDecrM             = (M > 1) && ((K == 0) || (M > K));
}

void Remes::nextSolveState() {
  if(!m_solveStateInterpolationDone) {
    m_solveStateInterpolationDone = true;
  } else if(!m_solveStateHighPrecision) {
    m_solveStateHighPrecision     = true;
  } else if(!m_solveStateDecrM && (m_M > 1)) {
    m_solveStateDecrM             = true;
  } else {
    throwException(_T("%s:No Next solveState for Remes(%d,%d):HighPrecision=true, DecrementM=true")
                  ,_T(__FUNCTION__)
                  ,m_M,m_K);
  }
}

void Remes::saveExtremaToMap(const BigReal &E, const BigReal &mmQuot) {
  const ExtremaKey key(m_M,m_K);
  Array<ExtremaVector> *a = s_extremaMap.get(key);
  if(a != NULL) {
    a->add(ExtremaVector(m_extrema, E, mmQuot));
  } else {
    Array<ExtremaVector> a;
    a.add(ExtremaVector(m_extrema, E, mmQuot));
    s_extremaMap.put(key,a);
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
    Array<ExtremaVector> *a = s_extremaMap.get(ExtremaKey(M,K));
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

  throwException(_T("%s(%d,%d):Cannot find extremaVector with MinMaxQuot < %s, though it should exist")
                ,_T(__FUNCTION__)
                , M, K, mmQuot.toString().cstr());
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
    throwInvalidArgumentException(_T(__FUNCTION__)
                                 ,_T("x=%s. x must be in the interval [0..%d]")
                                 ,toString(x).cstr(), maxX);
  }

  return getReal(m_initialExtr[getInt(x)]) + CubicSpline::operator()(x/maxX);
}

bool Remes::hasSavedExtrema(const int M, const int K) {
  return s_extremaMap.get(ExtremaKey(M,K)) != NULL;
}

const ExtremaVector &Remes::getBestSavedExtrema(const int M, const int K) {
  const Array<ExtremaVector> *a = s_extremaMap.get(ExtremaKey(M,K));
  if(a == NULL) {
    throwInvalidArgumentException(_T(__FUNCTION__), _T("ExtremaKey(%d,%d) not found")
                                 , M, K);
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
    throwInvalidArgumentException(_T(__FUNCTION__)
                                 ,_T("(M,K)=(%d,%d). No saved solution for (M,K)=(%d,%d) or (%d,%d)")
                                 ,M,K,M,K-1,M-1,K);
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
  DEFINEMETHODNAME;
  static const BigReal c1 = 0.5;

  const int MAXIT = 700;

  BigRealMatrix A(m_N+2, m_N+2, m_digits);
  for(int r = 0, s = 1; r <= m_N+1; r++, s = -s) {
    const BigReal &xr = m_extrema[r];

    checkRange(method, xr, m_left, m_right);

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

    setProperty(COEFFICIENTVECTOR, m_coefficientVector, LU.solve(m_functionValue));
    m_E                = m_coefficientVector[m_N+1];
    setProperty(SEARCHEITERATION, m_searchEIteration, 1);
    m_hasCoefficients  = true;
    m_coefVectorIndex++;
    setProperty(REMES_STATE, m_state, REMES_SEARCH_E);
  } else {
    // for m_K > 0 we have to iterate
    for(setProperty(SEARCHEITERATION, m_searchEIteration, 1); m_searchEIteration <= MAXIT; setProperty(SEARCHEITERATION, m_searchEIteration, m_searchEIteration+1)) {
      for(int r = 0, s = 1; r <= m_N+1; r++, s = -s) {
        const BigReal &xr = m_extrema[r];
        A(r,m_M+1) = xr * (signedValue(s,m_E) - m_functionValue[r]);
        for(int c = m_M + 2; c <= m_N; c++) {
          A(r,c) = xr * A(r,c-1);
        }
      }

      const BigRealLUMatrix LU(A);

      setProperty(COEFFICIENTVECTOR, m_coefficientVector, LU.solve(m_functionValue));
      m_nextE            = m_coefficientVector[m_N+1];
      m_hasCoefficients  = true;
      m_coefVectorIndex++;
      if(m_nextE.isZero()) break;

      setProperty(Q, m_Q, fabs(BIGREAL_1 - rQuot(Min(fabs(m_E),fabs(m_nextE)), Max(fabs(m_E),fabs(m_nextE)), m_digits)));
      if(m_Q < m_QEpsilon) {
        break;
      }
      m_E = rProd(c1, m_E + m_nextE, m_digits);
    }
    if(m_searchEIteration > MAXIT) {
      throwNoConvergenceException(_T(__FUNCTION__), _T("No convergence after %d iterations"), MAXIT);
    }
  }
}
/*

class ExtremaSearchJob : public Runnable {
  const Remes &m_r;
  DigitPool   *m_pool;
  BigReal      m_left, m_right;
  BigReal     &
  int          m_index;
  unsigned int run();
  ExtremaSearchJob
};

class ExtremaFinder {

};
BigRealThreadPool &getInstance()
*/

void Remes::findExtrema() {
  DEFINEMETHODNAME;
  const BigRealVector save = m_extrema;

  setProperty(REMES_STATE, m_state, REMES_SEARCH_EXTREMA);
  resetExtremumCount();
  int prevSign = setExtremum(0, m_left);
  for(int i = 1; i <= m_N; i++) {
    const BigReal l = rQuot(save[i-1] + 2.0 * save[i], 3, m_digits);
    const BigReal r = rQuot(save[i+1] + 2.0 * save[i], 3, m_digits);
    const int s = setExtremum(i, findExtremum(l, save[i], r));
    if(s == prevSign) {
      throwSameSignException(method);
    }
    prevSign = s;
  }
  if(setExtremum(m_N+1, m_right) == prevSign) {
    throwSameSignException(method);
  }
  setProperty(MAXERROR, m_maxError, fabs(m_errorValue[m_maxExtremumIndex]));
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

static BigReal sqr(const BigReal &x, UINT digits) {
  return rProd(x, x, digits);
}

static BigReal inverseInterpolate(const Point &p1, const Point &p2, const Point &p3, UINT digits) {
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

BigReal Remes::findExtremum(const BigReal &from, const BigReal &middle, const BigReal &to) {
  DEFINEMETHODNAME;
  const int STEPCOUNT = 20;
  BigReal l = from, r = to, m = middle;
  const int errorSign = sign(errorFunction(m));

  for(int depth = 0;; depth++) {
    Array<Point> plot;

    checkRange(method, m, l, r);

    if(l == r) {
      return l;
    }

    const BigReal step = rQuot(r-l,STEPCOUNT-1,m_digits);
    BigReal x = l;
    for(int count = 0;;) {
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
      const BigReal dx = e(step,-1);
      l = x - dx; m = x; r = x + dx;
    }
  }
}

void Remes::setExtrema(const BigRealVector &extrema) {
  if(extrema.getDimension() != m_N + 2) {
    throwInvalidArgumentException(_T(__FUNCTION__)
                                 ,_T("Dimension of extremaVector=%d, N=%d => Expected dimension=%d")
                                 ,extrema.getDimension(), m_N, m_N+2);
  }
  resetExtremumCount();
  for(size_t i = 0; i < extrema.getDimension(); i++) {
    setExtremum((int)i, extrema[i]);
  }
}

void Remes::setExtrema(const ExtremaVector &extrema) {
  setExtrema((const BigRealVector&)extrema);
  m_E = extrema.getE();
}

void Remes::resetExtremumCount() {
  m_minExtremumIndex = -1;
  m_maxExtremumIndex = -1;
  setProperty(EXTREMUMCOUNT, m_extremaCount, 0);
}

int Remes::setExtremum(const int index, const BigReal &x) {
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
  setProperty(EXTREMUMCOUNT, m_extremaCount, m_extremaCount+1);
  return sign(errorValue);
}

BigReal Remes::approximation(const BigReal &x) const {
  if(m_K) {
    BigReal sum1 = m_coefficientVector[m_M];
    BigReal sum2 = m_coefficientVector[m_N];
    int i;
    for(i = m_M - 1; i >= 0  ; i--) {
      sum1 = sum1 * x + m_coefficientVector[i];
    }
    for(i = m_N - 1; i >  m_M; i--) {
      sum2 = sum2 * x + m_coefficientVector[i];
    }
    return rQuot(sum1, sum2 * x + BIGREAL_1, m_digits);
  } else {
    BigReal sum = m_coefficientVector[m_M];
    for(int i = m_M - 1; i >= 0; i--) {
      sum = sum * x + m_coefficientVector[i];
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

String Remes::getMMQuotString() const {
  const BigReal mmQuot = BIGREAL_1 - fabs(rQuot(this->getMinAbsExtremumValue(),getMaxAbsExtremumValue(),m_digits)); // minExtr -> maxExtr => mmQuot -> 0
  return format(_T("MinMaxQuot = 1-|MinExtr/MaxExtr|:%s"),FormatBigReal(mmQuot).cstr());
}

StringArray Remes::getCoefficientStringArray() const {
  StringArray result;
  for(int i = 0; i <= m_M; i++) {
    result.add(format(_T("a[%2d] = %s     %s"), i, FormatBigReal(m_coefficientVector[i],25,35).cstr(), FormatBigReal(getReal(m_coefficientVector[i])).cstr()));
  }
  result.add(format(_T("b[ 0] = +1")));
  for(int i = m_M + 1; i <= m_N; i++) {
    result.add(format(_T("b[%2d] = %s     %s"), i-m_M,FormatBigReal(m_coefficientVector[i],25,35).cstr(), FormatBigReal(getReal(m_coefficientVector[i])).cstr()));
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
    Double80 d80 = getDouble80(m_coefficientVector[i]);
    double   d64 = getDouble(  m_coefficientVector[i]);
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
    const double coef = getDouble(m_coefficientVector[i]);
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
