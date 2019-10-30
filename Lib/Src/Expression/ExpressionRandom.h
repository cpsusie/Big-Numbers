#pragma once

#include <Math/Real.h>

namespace Expr {

  // use RandomGenerator::s_stdGenerator declared in Random.h
  Real randomReal();
  Real randomReal(const Real &from, const Real &to);
  // mean and standardDeviation. use RandomGenerator::s_stdGenerator
  Real randomGaussian(const Real &mean, const Real &s);

}; // namespace Expr
