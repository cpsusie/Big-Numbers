#include "pch.h"
#include "ExpressionRandom.h"

namespace Expr {

  // use _standardRandomGenerator declared in Random.h
  Real randomReal() {
    return randReal();
  }

  Real randomReal(const Real &from, const Real &to) {
    return randReal(from, to);
  }

  // mean and standardDeviation. use _standardRandomGenerator
  Real randomGaussian(const Real &mean, const Real &s) {
    return randGaussianReal(mean, s);
  }

}; // namespace Expr
