#pragma once

#include "Real.h"

template <class Domain, class Range> class FunctionTemplate {
public:
  virtual Range operator()(const Domain &x) = 0;
};

typedef FunctionTemplate<Real,Real>         Function;

CompactRealArray findZeroes(  Function &f, const RealInterval &interval);
