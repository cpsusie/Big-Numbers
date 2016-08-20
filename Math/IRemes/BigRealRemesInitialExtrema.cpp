#include "stdafx.h"
#include <Math/Polynomial.h>

// Return the N+2 extrema (incl. endpoints) of the Chebyshev polynomial of degree N+1, scaled to the x-interval [m_left;m_right], where N=M+K
BigRealVector Remes::getDefaultInitialExtrema(const UINT M, const UINT K) {
  const Real left  = getReal(m_domain.getFrom());
  const Real right = getReal(m_domain.getTo());
  const Real a = (left + right)/2.0;
  const Real b = (left - right)/2.0;
  const UINT N = M + K;

  BigRealVector result(N+2);
  for(UINT i = 0; i <= N+1; i++) {
    result[i] = a + b * cos(M_PI * i / (N+1));
  }
  return result;
}

BigRealVector Remes::findFinalExtrema(const UINT M, const UINT K, const bool highPrecision) {
  const BigReal mmQuot = highPrecision ? s_defaultMMQuotEps : e(BIGREAL_1,-7);

  for(bool hasSolved = false;;) {
    Array<ExtremaVector> *a = s_extremaMap.get(ExtremaKey(M, K));
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

  throwException(_T("%s(%u,%u):Cannot find extremaVector with MinMaxQuot < %s, though it should exist")
                ,__TFUNCTION__
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
  const UINT              dim   = (UINT)initialExtr.getDimension();
  CompactArray<DataPoint> result;

  for(UINT i = 0; i <= dim-1; i++) {
    const Real x = (Real)i/(dim-1); // x = [0..1] Equally spaced
    const Real y = getReal(finalExtr[i] - initialExtr[i]); // first and last y will be 0
    result.add(DataPoint(x,y));
  }
  return result;
}

Real InterpolationFunction::operator()(const Real &x) {
  const int maxX = (int)m_initialExtr.getDimension()-1;
  if((x == 0) || (x == maxX)) {
    return getReal(m_initialExtr[getInt(x)]);
  } else if((x < 0) || (x > maxX)) {
    throwInvalidArgumentException(__TFUNCTION__
                                 ,_T("x=%s. x must be in the interval [0..%d]")
                                 ,toString(x).cstr(), maxX);
  }

  return getReal(m_initialExtr[getInt(x)]) + CubicSpline::operator()(x/maxX);
}

bool Remes::hasSavedExtrema(const UINT M, const UINT K) {
  return s_extremaMap.get(ExtremaKey(M, K)) != NULL;
}

const ExtremaVector &Remes::getBestSavedExtrema(const UINT M, const UINT K) {
  const Array<ExtremaVector> *a = s_extremaMap.get(ExtremaKey(M, K));
  if(a == NULL) {
    throwInvalidArgumentException(__TFUNCTION__, _T("ExtremaKey(%u,%u) not found"), M, K);
  }
  size_t bestIndex = 0;
  for(size_t i = 1; i < a->size(); i++) {
    if((*a)[i].getMMQuot() < (*a)[bestIndex].getMMQuot()) {
      bestIndex = i;
    }
  }
  return (*a)[bestIndex];
}

bool Remes::hasFastInterpolationOfExtrema(const UINT M, const UINT K) {
  return ((K > 0) && hasSavedExtrema(M, K-1)) || ((M > 1) && hasSavedExtrema(M-1, K));
}

BigRealVector Remes::getFastInitialExtremaByInterpolation(const UINT M, const UINT K) {
  UINT M1 = M, K1 = K;
  if(K > 0 && hasSavedExtrema(M,K-1)) {
    K1--;
  } else if(M > 1 && hasSavedExtrema(M-1,K)) {
    M1--;
  } else {
    throwInvalidArgumentException(__TFUNCTION__
                                 ,_T("(M,K)=(%u,%u). No saved solution for (M,K)=(%u,%u) or (%u,%u)")
                                 ,M,K,M,K-1,M-1,K);
  }
  return getInterpolatedExtrema(getDefaultInitialExtrema(M, K),getDefaultInitialExtrema(M1, K1),getBestSavedExtrema(M1, K1));
}

BigRealVector Remes::findInitialExtremaByInterpolation(const UINT M, const UINT K) {
  const UINT M1 = m_solveStateDecrM ? (M-1) :  M;
  const UINT K1 = m_solveStateDecrM ?  K    : (K-1);

  Remes subRemes(*this);
  m_solveStateInterpolationDone = true;

  return getInterpolatedExtrema(getDefaultInitialExtrema(M, K),getDefaultInitialExtrema(M1, K1),subRemes.findFinalExtrema(M1, K1, m_solveStateHighPrecision));
}

BigRealVector Remes::getInterpolatedExtrema(const BigRealVector &defaultExtrema, const BigRealVector &defaultSubExtrema, const BigRealVector &finalSubExtrema) const {
  InterpolationFunction interpolFunction(defaultExtrema, defaultSubExtrema, finalSubExtrema);
  const UINT dimension = (UINT)defaultExtrema.getDimension();
  notifyPropertyChanged(INTERPOLATIONSPLINE, &dimension, &(Function&)interpolFunction);
  BigRealVector result(dimension);
  for(UINT i = 0; i < dimension; i++) {
    result[i] = interpolFunction(i);
  }
  return result;
}

void Remes::saveExtremaToMap(const BigReal &E, const BigReal &mmQuot) {
  const ExtremaKey key(m_M, m_K);
  Array<ExtremaVector> *a = s_extremaMap.get(key);
  if(a != NULL) {
    a->add(ExtremaVector(m_extrema, E, mmQuot));
  } else {
    Array<ExtremaVector> a;
    a.add(ExtremaVector(m_extrema, E, mmQuot));
    s_extremaMap.put(key,a);
  }
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
