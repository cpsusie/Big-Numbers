#pragma once

#include <Unicode.h>
#include <CompactArray.h>
#include <NumberInterval.h>
#include "TrigonometricMode.h"

//#define LONGDOUBLE // defined in configurations "Debug long double" and "Release long double"
#if defined(LONGDOUBLE)

#include "Double80.h"
#define Real Double80

#else

#include "Double64.h"
#define Real double

#endif

typedef CompactArray<Real>   CompactRealArray;
typedef NumberInterval<Real> RealInterval;

template<typename T> T csc(T x) {
  return 1.0 / sin(x);
}

template<typename T> T sec(T x) {
  return 1.0 / cos(x);
}

template<typename T> T acsc(T x) {
  return asin(1.0 / x);
}

template<typename T> T asec(T x) {
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

// Calculates x! = x*(x-1)*(x-2)*...*2*1, extended to double numbers by the gamma function x! = gamma(x+1)
template<typename T> T  factorial(T x) {
  return gamma(x + 1);
}

template<typename T> T binomial(T n, T k) {
  return  factorial(n) / (factorial(k)* factorial(n - k));
}

#if !defined(LONGDOUBLE)
#define REAL_PI          M_PI
#define randReal         randDouble
#define randGaussianReal randGaussian
#else
#define REAL_PI          DBL80_PI
#define randReal         randDouble80
#define randGaussianReal randGaussianD80
#endif

inline void setToRandom(Real &x, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
  x = randReal(rnd);
}

inline Real getRealNaN() {
  return std::numeric_limits<Real>::quiet_NaN();
}

#if !defined(LONGDOUBLE)
#define rtoa   dbltoa
#define rtow   dbltow
#define strtor strtod
#define wcstor wcstod
#else // LONGDOUBLE
#define rtoa   d80toa
#define rtow   d80tow
#define strtor strtod80
#define wcstor wcstod80
#endif // LONGDOUBLE

#if defined(_UNICODE)
#define rtot    rtow
#define _tcstor wcstor

#else
#define rtot    rtoa
#define _tcstor strtor
#endif // _UNICODE
