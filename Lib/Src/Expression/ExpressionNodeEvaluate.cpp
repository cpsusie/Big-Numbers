#include "pch.h"
#include <Math/Rational.h>
#include <Math/Expression/ExpressionNode.h>
#include "ExpressionRandom.h"

namespace Expr {

static Real myroot(Real rad, Real rt) {
  if(rad >= 0) {
    return ::root(rad, rt);
  } else {
    Rational rootR;
    if(Rational::isRational(rt, &rootR)) {
      if(rootR.getNumerator() != 1) {
        rad = ::root(rad, (Real)rootR.getNumerator());
      }
      if(rootR.getDenominator() != 1) {
        rad = mypow(rad, (Real)rootR.getDenominator());
      }
      return rad;
    } else {
      return ::root(rad, rt); // -1.#IND
    }
  }
}

Real ExpressionNode::evaluateReal() const {
  DEFINEMETHODNAME;

  switch(getSymbol()) {
  case PLUS     : return left()->evaluateReal() + right()->evaluateReal();
  case MINUS    : return (left()->evaluateReal() - right()->evaluateReal());
  case UNARYMINUS:return -left()->evaluateReal();
  case PROD     : return left()->evaluateReal() * right()->evaluateReal();
  case QUOT     : return left()->evaluateReal() / right()->evaluateReal();
  case MOD      : return fmod(                   left()->evaluateReal(),  right()->evaluateReal());
  case ROOT     : return myroot(                 left()->evaluateReal(),  right()->evaluateReal());
  case BINOMIAL : return binomial(               left()->evaluateReal(),  right()->evaluateReal());
  case CHI2DIST : return chiSquaredDistribution( left()->evaluateReal(),  right()->evaluateReal());
  case CHI2DENS : return chiSquaredDensity(      left()->evaluateReal(),  right()->evaluateReal());
  case LINCGAMMA: return lowerIncGamma(          left()->evaluateReal(),  right()->evaluateReal());
  case ATAN2    : return atan2(                  left()->evaluateReal(),  right()->evaluateReal(), getTrigonometricMode());
  case HYPOT    : return hypot(                  left()->evaluateReal(),  right()->evaluateReal());
  case MAX      : return dmax(                   left()->evaluateReal(),  right()->evaluateReal());
  case MIN      : return dmin(                   left()->evaluateReal(),  right()->evaluateReal());
  case RAND     : return randomReal(             left()->evaluateReal(),  right()->evaluateReal());
  case NORMRAND : return randomGaussian(         left()->evaluateReal(),  right()->evaluateReal());
  case ABS      : return fabs(                   left()->evaluateReal());
  case ACOS     : return acos(                   left()->evaluateReal(),  getTrigonometricMode());
  case ACOSH    : return acosh(                  left()->evaluateReal());
  case ACOT     : return acot(                   left()->evaluateReal(),  getTrigonometricMode());
  case ACSC     : return acsc(                   left()->evaluateReal(),  getTrigonometricMode());
  case ASEC     : return asec(                   left()->evaluateReal(),  getTrigonometricMode());
  case ASIN     : return asin(                   left()->evaluateReal(),  getTrigonometricMode());
  case ASINH    : return asinh(                  left()->evaluateReal());
  case ATAN     : return atan(                   left()->evaluateReal(),  getTrigonometricMode());
  case ATANH    : return atanh(                  left()->evaluateReal());
  case CEIL     : return ceil(                   left()->evaluateReal());
  case COS      : return cos(                    left()->evaluateReal(),  getTrigonometricMode());
  case COSH     : return ::cosh(                 left()->evaluateReal());
  case COT      : return cot(                    left()->evaluateReal(),  getTrigonometricMode());
  case CSC      : return csc(                    left()->evaluateReal(),  getTrigonometricMode());
  case ERF      : return errorFunction(          left()->evaluateReal());
  case EXP      : return ::exp(                  left()->evaluateReal());
  case EXP10    : return ::exp10(                left()->evaluateReal());
  case EXP2     : return ::exp2(                 left()->evaluateReal());
  case FAC      : return factorial(              left()->evaluateReal());
  case FLOOR    : return floor(                  left()->evaluateReal());
  case GAMMA    : return gamma(                  left()->evaluateReal());
  case GAUSS    : return ::gauss(                left()->evaluateReal());
  case INVERF   : return inverseErrorFunction(   left()->evaluateReal());
  case LN       : return log(                    left()->evaluateReal());
  case LOG10    : return ::log10(                left()->evaluateReal());
  case LOG2     : return ::log2(                 left()->evaluateReal());
  case NORM     : return norm(                   left()->evaluateReal());
  case PROBIT   : return probitFunction(         left()->evaluateReal());
  case SEC      : return sec(                    left()->evaluateReal(),  getTrigonometricMode());
  case SIGN     : return sign(                   left()->evaluateReal());
  case SIN      : return sin(                    left()->evaluateReal(),  getTrigonometricMode());
  case SINH     : return ::sinh(                 left()->evaluateReal());
  case SQR      : return ::sqr(                  left()->evaluateReal());
  case SQRT     : return ::sqrt(                 left()->evaluateReal());
  case TAN      : return tan(                    left()->evaluateReal(),  getTrigonometricMode());
  case TANH     : return ::tanh(                 left()->evaluateReal());
  case IIF      : return child(0).evaluateBool() ? child(1).evaluateReal() : child(2).evaluateReal();

  case INDEXEDSUM     :
    {            Real                 &i     = child(0).doAssignment();
                 const Real            end   = child(1).evaluateReal();
                 const ExpressionNode *expr  = child(2).node();
                 Real                  acc   = 0;
                 for(;i <= end; i++) {
                   acc += expr->evaluateReal();
                 }
                 return acc;
    }
  case INDEXEDPRODUCT :
    {            Real                 &i     = child(0).doAssignment();
                 const Real            end   = child(1).evaluateReal();
                 const ExpressionNode *expr  = child(2).node();
                 Real                  acc   = 1;
                 for(;i <= end; i++) {
                   acc *= expr->evaluateReal();
                 }
                 return acc;
    }
  default      : throwUnknownSymbolException(method);
                 return 0;
  }
}

}; // namespace Expr
