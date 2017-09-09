#pragma once

#include "MathFunctions.h"

template <class Domain, class Range> class FunctionTemplate {
public:
  virtual Range operator()(const Domain &x) = 0;
};

typedef FunctionTemplate<Real,Real>         Function;

#define verifyEqualsInt(   expected, value)            verify(value == expected)
#define verifyAlmostEquals(expected, value,tolerance)  verify(fabs((expected)-(value)) <= (tolerance))

CompactRealArray findZeroes(Function &f, const RealInterval &i);
