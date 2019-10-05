#pragma once

#include <Unicode.h>
#include <CompactArray.h>
#include <NumberInterval.h>

//#define LONGDOUBLE // defined in configurations "Debug long double" and "Release long double"
#ifdef LONGDOUBLE

#include "Double80.h"
#define Real Double80

#else

#include "Double64.h"
#define Real double

#endif

typedef CompactArray<Real>   CompactRealArray;
typedef NumberInterval<Real> RealInterval;

typedef enum {
  RADIANS
 ,DEGREES
 ,GRADS
} TrigonometricMode;

String            toString(TrigonometricMode mode);
TrigonometricMode getTrigonometricModeFromString(const String &str);

template<class T> T csc(T x) {
  return 1.0 / sin(x);
}

template<class T> T sec(T x) {
  return 1.0 / cos(x);
}

template<class T> T acsc(T x) {
  return asin(1.0 / x);
}

template<class T> T asec(T x) {
  return acos(1.0 / x);
}

Real sinDegrees(          const Real &x);
Real cosDegrees(          const Real &x);
Real tanDegrees(          const Real &x);
Real cotDegrees(          const Real &x);
Real asinDegrees(         const Real &x);
Real acosDegrees(         const Real &x);
Real atanDegrees(         const Real &x);
Real atan2Degrees(        const Real &y, const Real &x);
Real acotDegrees(         const Real &x);

Real cscDegrees(          const Real &x);
Real secDegrees(          const Real &x);
Real acscDegrees(         const Real &x);
Real asecDegrees(         const Real &x);

Real sinGrads(            const Real &x);
Real cosGrads(            const Real &x);
Real tanGrads(            const Real &x);
Real cotGrads(            const Real &x);
Real asinGrads(           const Real &x);
Real acosGrads(           const Real &x);
Real atanGrads(           const Real &x);
Real atan2Grads(          const Real &y, const Real &x);
Real acotGrads(           const Real &x);

Real cscGrads(            const Real &x);
Real secGrads(            const Real &x);
Real acscGrads(           const Real &x);
Real asecGrads(           const Real &x);

Real sin(                 const Real &x, TrigonometricMode mode);
Real cos(                 const Real &x, TrigonometricMode mode);
Real tan(                 const Real &x, TrigonometricMode mode);
Real cot(                 const Real &x, TrigonometricMode mode);
Real asin(                const Real &x, TrigonometricMode mode);
Real acos(                const Real &x, TrigonometricMode mode);
Real atan(                const Real &x, TrigonometricMode mode);
Real atan2(               const Real &y, const Real &x, TrigonometricMode mode);
Real acot(                const Real &x, TrigonometricMode mode);
Real csc(                 const Real &x, TrigonometricMode mode);
Real sec(                 const Real &x, TrigonometricMode mode);
Real acsc(                const Real &x, TrigonometricMode mode);
Real asec(                const Real &x, TrigonometricMode mode);

Real gammaStirling(       const Real &x);
Real lnGammaStirling(     const Real &x);
Real gauss(               const Real &x);
Real norm(                const Real &x);
Real probitFunction(      const Real &x);
Real errorFunction(       const Real &x);
Real inverseErrorFunction(const Real &x);

// Assume x >= 0
Real lowerIncGamma(          const Real &a,  const Real &x);
Real chiSquaredDensity(      const Real &df, const Real &x);
Real chiSquaredDistribution( const Real &df, const Real &x);

template<class T> T binomial(T n, T k) {
  return fac(n) / (fac(k)*fac(n - k));
}

#ifndef LONGDOUBLE
#define randReal         randDouble
#define randGaussianReal randGaussian
#else
#define randReal         randDouble80
#define randGaussianReal randGaussianD80
#endif

inline void setToRandom(Real &x, RandomGenerator *rnd = _standardRandomGenerator) {
  x = randReal(rnd);
}

inline Real getRealNaN() {
  return std::numeric_limits<Real>::quiet_NaN();
}

#ifndef LONGDOUBLE
#define strtor strtod
#define wcstor wcstod
#else // LONGDOUBLE
#define strtor strtod80
#define wcstor wcstod80
#endif // LONGDOUBLE

#ifdef _UNICODE
#define _tcstor wcstor
#else
#define _tcstor strtor
#endif // _UNICODE
