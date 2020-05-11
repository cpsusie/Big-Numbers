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

template <typename F> class FunctionWithTimeTemplate : public F {
public:
  virtual void        setTime(const Real &time)   = 0;
  virtual const Real &getTime() const             = 0;
  virtual FunctionWithTimeTemplate *clone() const = 0;
};
