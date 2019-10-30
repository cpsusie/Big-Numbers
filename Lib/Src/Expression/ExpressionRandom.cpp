#include "pch.h"
#include "ExpressionRandom.h"

namespace Expr {

  // use RandomGenerator::s_stdGenerator declared in Random.h
  Real randomReal() {
    return randReal();
  }

  Real randomReal(const Real &from, const Real &to) {
    return randReal(from, to);
  }

  // mean and standardDeviation. use RandomGenerator::s_stdGenerator
  Real randomGaussian(const Real &mean, const Real &s) {
    return randGaussianReal(mean, s);
  }

}; // namespace Expr
