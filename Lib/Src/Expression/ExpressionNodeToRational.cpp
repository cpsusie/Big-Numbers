#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

bool ExpressionNode::reducesToRationalConstant(Rational *r) const {
  if(!isConstant()) {
    return false;
  }
  return reducesToRational(r);
}

bool ExpressionNode::reducesToRational(Rational *r) const {
  switch(getSymbol()) {
  case NUMBER         :
    if(!isRational()) {
      return false;
    } else {
      if(r) *r = getRational();
      return true;
    }
  case NAME           :
    return isConstant() && Rational::isRational(getValueRef(),r);

  case MINUS          :
    { assert(isUnaryMinus());
      Rational tmp, *tmpp = r ? &tmp : NULL;
      if(!left()->reducesToRational(tmpp)) {
        return false;
      }
      if(r) *r = -tmp;
      return true;
    }

  case SUM            :
    { const AddentArray &a = getAddentArray();
      Rational sum = 0, tmp, *tmpp = r ? &tmp : NULL;
      for(size_t i = 0; i < a.size(); i++) {
        const SNode &e = a[i];

        if(!e.left().reducesToRational(tmpp)) {
          return false;
        }
        if(tmpp) {
          if(e.isPositive()) sum += tmp; else sum -= tmp;
        }
      }
      if(r) *r = sum;
    }
    return true;

  case PRODUCT        :
    { const FactorArray &a = getFactorArray();
      Rational product = 1, tmp, *tmpp = r ? &tmp : NULL;
      for(size_t i = 0; i < a.size(); i++) {
        if(!a[i].reducesToRational(tmpp)) {
          return false;
        }
        if(r) product *= tmp;
      }
      if(r) *r = product;
    }
    return true;

  case POW            :
    { Rational base, exponent;
      if(left()->reducesToRational(&base) && right()->reducesToRational(&exponent)) {
        if(!Rational::isRationalPow(base, exponent, r)) {
          return false;
        }
        return true;
      }
    }
    return false;

  default:
    return Rational::isRational(evaluateReal(), r);
  }
}

}; // namespace Expr
