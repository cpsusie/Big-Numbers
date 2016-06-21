#include "pch.h"
#include <Math/Expression/Expression.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>

#ifdef __NEVER__

/* -------------------------------------------- replaceRationalPowers -----------------------------------------------------------*/

const ExpressionNode *Expression::replaceRationalPowers(const ExpressionNode *n) const {
  switch(n->getSymbol()) {
  case NAME:
  case NUMBER:
    return n;

  case SUM:
    { const AddentArray &a = n->getAddentArray();
      AddentArray result(a.size());
      for(int i = 0; i < a.size(); i++) {
        const SumElement *e = a[i];
        result.add(replaceRationalPowers(e->getNode()),e->isPositive());
      }
      return getSum(result);
    }
  case PRODUCT:
    { const FactorArray &a = n->getFactorArray();
      FactorArray result(a.size());
      for(int i = 0; i < a.size(); i++) {
        const ExpressionFactor *f = a[i];
        result.add(replaceRationalPower(f));
      }
      return getProduct(result);
    }
  case POW:
    return replaceRationalPower(n);
  default:
    { const ExpressionNodeArray &a = n->getChildArray();
      ExpressionNodeArray newChildArray(a.size());
      for(int i = 0; i < a.size(); i++) {
        newChildArray.add(replaceRationalPowers(a[i]));
      }
      return getTree(n, newChildArray);
    }
  }
}

const ExpressionNode *Expression::replaceRationalPower(const ExpressionNode *n) const {
  const ExpressionFactor     *f        = replaceRationalPower(fetchFactorNode(n->left(), n->right()));
  const ExpressionNode *exponent = f->getExponent();
  if(!exponent->isNumber()) {
    return power(f->getBase(), exponent);
  } else if(exponent->isNegative()) {
    return quotient(getOne(), power(f->getBase(), minus(exponent)));
  } else {
    return power(f->getBase(), exponent);
  }
}

const ExpressionFactor *Expression::replaceRationalPower(const ExpressionFactor *f) const {
  const ExpressionNode *exponent = f->getExponent();
  switch(exponent->getSymbol()) {
  case NUMBER:
    if(exponent->isRational()) {
      const Rational er = exponent->getRational();
      if(er.isInteger()) {
        return fetchFactorNode(replaceRationalPowers(f->getBase()),exponent);
      } else {
        return fetchFactorNode(root(replaceRationalPowers(f->getBase()),numberExpression(er.getDenominator())),numberExpression(er.getNumerator()));
      }
    } else {
      const Real ed = exponent->getReal();
      String     s1 = ::toString(ed);
      String     s2 = ::toString(1.0/ed);
      if(s1.length() <= s2.length()) {
        return fetchFactorNode(replaceRationalPowers(f->getBase()),exponent);
      } else {
        const ExpressionNode *newExponent = (ed > 0) ? getOne() : getMinusOne();
        const ExpressionNode *rootExpr    = numberExpression(1.0/fabs(ed));
        return fetchFactorNode(root(replaceRationalPowers(f->getBase()), rootExpr), newExponent);
      }
    }
  }
  return fetchFactorNode(replaceRationalPowers(f->getBase()), replaceRationalPowers(exponent));
}

/* -------------------------------------------- replaceRationalFactors -----------------------------------------------------------*/

const ExpressionNode *Expression::replaceRationalFactors(const ExpressionNode *n) const {
  DEFINEMETHODNAME;

  switch(n->getSymbol()) {
  case NAME:
  case NUMBER:
    return n;

  case SUM:
    { AddentArray result;
      const AddentArray &a = n->getAddentArray();
      for(int i = 0; i < a.size(); i++) {
        const SumElement *e = a[i];
        result.add(replaceRationalFactors(e->getNode()), e->isPositive());
      }
      return getSum(result);
    }
  case PRODUCT:
    { FactorArray result;
      const FactorArray &a = n->getFactorArray();
      for(int i = 0; i < a.size(); i++) {
        const ExpressionFactor *f = a[i];
        if(!f->isRational()) {
          result.add(f);
        } else {
          Rational r = f->getBase()->getRational();
          if(!f->getExponent()->isOne()) {
            throwMethodException(s_className, method, _T("Exponent to rational constant is not 1. (factor=%s)"),  f->toString().cstr());
          }
          const __int64 numerator   = r.getNumerator();
          const __int64 denominator = r.getDenominator();
          if(numerator != 1) {
            result.add(numberExpression(numerator));
          }
          if(denominator != 1) {
            result.add(numberExpression(denominator), getMinusOne());
          }
        }
      }
      return getProduct(result);
    }
  default:
    { const ExpressionNodeArray &a = n->getChildArray();
      ExpressionNodeArray newChildArray(a.size());
      for(int i = 0; i < a.size(); i++) {
        newChildArray.add(replaceRationalFactors(a[i]));
      }
      return getTree(n, newChildArray);
    }
  }
}

// -----------------------------------------------------------------------------------------------------------------------------

const ExpressionNode *Expression::substituteName(const ExpressionNode *n, const String &from, const String &to) const {
  switch(n->getSymbol()) {
  case NAME:
    return (n->getName() == from) ? fetchVariableNode(to) : n;

  case NUMBER:
    return n;

  case SUM:
    { const AddentArray &a = n->getAddentArray();
      AddentArray list(a.size());
      for(int i = 0; i < a.size(); i++) {
        const SumElement *e = a[i];
        list.add(substituteName(e->getNode(), from, to), e->isPositive());
      }
      return getSum(list);
    }
  case PRODUCT:
    { const FactorArray &a = n->getFactorArray();
      FactorArray list(a.size());
      for(int i = 0; i < a.size(); i++) {
        const ExpressionFactor *f = a[i];
        list.add(substituteName(f->getBase()    , from, to)
                ,substituteName(f->getExponent(), from, to)
                );
      }
      return getProduct(list);
    }
  default:
    { const ExpressionNodeArray &a = n->getChildArray();
      ExpressionNodeArray list(a.size());
      for(int i = 0; i < a.size(); i++) {
        const ExpressionNode *child = a[i];
        list.add(substituteName(child, from, to));
      }
      return getTree(n, list);
    }
  }
}

#endif
