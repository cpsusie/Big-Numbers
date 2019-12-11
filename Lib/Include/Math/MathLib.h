#pragma once

#include "Real.h"

template <typename Domain, typename Range> class FunctionTemplate {
public:
  virtual Range operator()(const Domain &x) = 0;
  virtual ~FunctionTemplate() {
  }
};

typedef FunctionTemplate<Real,Real>         Function;

CompactRealArray findZeroes(  Function &f, const RealInterval &interval);
