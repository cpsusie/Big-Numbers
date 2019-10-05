#pragma once

#include <Math/Real.h>

namespace Expr {

  // use _standardRandomGenerator declared in Random.h
  Real randomReal();
  Real randomReal(const Real &from, const Real &to);
  // mean and standardDeviation. use _standardRandomGenerator
  Real randomGaussian(const Real &mean, const Real &s);

}; // namespace Expr
