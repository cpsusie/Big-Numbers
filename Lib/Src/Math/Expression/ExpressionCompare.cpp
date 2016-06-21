#include "pch.h"
#include <Math/Expression/Expression.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>

// Should only be called in Canonical treeform
bool Expression::treesEqual(const ExpressionNode *n1, const ExpressionNode *n2) const {
  DEFINEMETHODNAME;

  if(n1 == n2) {
    return true;
  } else if((n1 == NULL) || (n2 == NULL) || (n1->getSymbol() != n2->getSymbol())) {
    return false;
  }
  switch(n1->getSymbol()) {
  case NUMBER:
    return n1->getNumber() == n2->getNumber();
  case NAME  :
    return n1->getName() == n2->getName();
  case SUM:
    { const AddentArray &a1 = n1->getAddentArray(); // they have been sorted
      const AddentArray &a2 = n2->getAddentArray();
      if(a1.size() != a2.size()) return false;
      for(size_t i = 0; i < a1.size(); i++) {
        const SumElement *e1 = a1[i];
        const SumElement *e2 = a2[i];
        if((e1->isPositive() != e2->isPositive()) || !treesEqual(e1->getNode(), e2->getNode())) return false;
      }
    }
    return true;

  case MINUS :
    assert(n1->isUnaryMinus());
    assert(n2->isUnaryMinus());
    return treesEqual(n1->left(), n2->left());

  case PRODUCT  :
    { const FactorArray &a1 = n1->getFactorArray(); // they have been sorted
      const FactorArray &a2 = n2->getFactorArray();
      if(a1.size() != a2.size()) return false;
      for(size_t i = 0; i < a1.size(); i++) {
        if(!treesEqual(a1[i], a2[i])) return false;
      }
    }
    return true;

  case MOD   :
    if(!treesEqual(n1->left(), n2->left())) return false;
    return treesEqual(n1->right(), n2->right()) || treesEqualMinus(n1->right(), n2->right());

  case POLY  :
    { const ExpressionNodeArray &coef1 = n1->getCoefficientArray();
      const ExpressionNodeArray &coef2 = n2->getCoefficientArray();
      const ExpressionNode      *arg1  = n1->getArgument();
      const ExpressionNode      *arg2  = n2->getArgument();
      if(coef1.size() != coef2.size()) return false;
      for(size_t i = 0; i < coef1.size(); i++) {
        if(!treesEqual(coef1[i], coef2[i])) return false;
      }
      return treesEqual(arg1, arg2);
    }

  case PLUS  :
  case PROD  :
  case QUOT  :
  case SQR   :
  case ROOT  :
  case SQRT  :
  case EXP   :
  case SEC   :
  case CSC   :
  case COT   :
    throwMethodInvalidArgumentException(s_className, method, _T("Invalid symbol in canonical form:%s"), n1->getSymbolName().cstr());

  default:
    { const ExpressionNodeArray &a1 = n1->getChildArray();
      const ExpressionNodeArray &a2 = n2->getChildArray();
      if(a1.size() != a2.size()) return false;
      for(size_t i = 0; i < a1.size(); i++) {
        if(!treesEqual(a1[i], a2[i])) return false;
      }
    }
    return true;
  }
}

// Should only be called in Canonical treeform
bool Expression::treesEqualMinus(const SNode n1, const SNode n2) const {
  DEFINEMETHODNAME;

  if(n1.isSameNode(n2) || n1.isEmpty() || n2.isEmpty()) {
    return false;
  }

  if(n1.getSymbol() != n2.getSymbol()) { // special case
    if(n1.isUnaryMinus()) return treesEqual(n1.left(), n2);
    if(n2.isUnaryMinus()) return treesEqual(n2.left(), n1);
    return false;
  }
  // symbols are the same
  switch(n1.getSymbol()) {
  case NUMBER:
    return n1.getNumber() == -n2.getNumber();
  case NAME  :
    return false;
  case SUM:
    { const AddentArray &a1 = n1.getAddentArray(); // they have been sorted
      const AddentArray &a2 = n2.getAddentArray();
      if(a1.size() != a2.size()) return false;
      for(size_t i = 0; i < a1.size(); i++) {
        const SumElement *e1 = a1[i];
        const SumElement *e2 = a2[i];
        if(e1->isPositive() == e2->isPositive()) {
          if(!treesEqualMinus(e1->getNode(), e2->getNode())) return false;
        } else { // e1->isPositive() != e2->isPositive()
          if(!treesEqual(e1->getNode(), e2->getNode())) return false;
        }
      }
    }
    return true;
  case MINUS :
    assert(n1.isUnaryMinus());
    assert(n2.isUnaryMinus());
    return treesEqualMinus(n1.left(), n2.left());

  case PRODUCT  :
    { const FactorArray &a1 = n1.getFactorArray(); // they have been sorted
      const FactorArray &a2 = n2.getFactorArray();
      if(a1.size() != a2.size()) return false;
      int signShiftCount = 0;
      for(size_t i = 0; i < a1.size(); i++) {
        if(treesEqual(a1[i], a2[i])) continue;
        if(treesEqualMinus(a1[i], a2[i])) {
          signShiftCount++;
          continue;
        }
        return false;
      }
      return isOdd(signShiftCount);
    }

  case MOD   :
    if(!treesEqualMinus(n1.left(), n2.left())) return false;
    return treesEqual(n1.right(), n2.right()) || treesEqualMinus(n1.right(), n2.right());

  case POW   :
    { const SNode b1 = n1.left();
      const SNode b2 = n2.left();
      const SNode e1 = n1.right();
      const SNode e2 = n2.right();
      Rational er1, er2;
      if(!reducesToRationalConstant(e1, &er1) || !reducesToRationalConstant(e2, &er2)) return false;
      return (er1 == er2) && isOdd(er1.getNumerator()) && isOdd(er1.getDenominator()) && treesEqualMinus(b1, b2);
    }

  case INDEXEDSUM    :
    return (n1.child(0) == n2.child(0)) && (n1.child(1) == n2.child(1))
        && treesEqualMinus(n1.child(2), n2.child(2));

  case POLY  :
    { const ExpressionNodeArray  &coefList1 = n1.getCoefficientArray();
      const ExpressionNodeArray  &coefList2 = n2.getCoefficientArray();
      const SNode                 x1        = n1.getArgument();
      const SNode                 x2        = n2.getArgument();
      if(coefList1.size() != coefList2.size()) return false;
      if(x1 != x2) return false;
      for(size_t i = 0; i < coefList1.size(); i++) {
        if(!treesEqualMinus(coefList1[i], coefList2[i])) return false;
      }
      return true;
    }

  case PLUS  :
  case PROD  :
  case QUOT  :
  case SQR   :
  case ROOT  :
  case SQRT  :
  case EXP   :
  case SEC   :
  case CSC   :
  case COT   :
    throwMethodInvalidArgumentException(s_className, method, _T("Invalid symbol in canonical form:%s"), n1.getSymbolName().cstr());

  case RETURNREAL:
    return treesEqualMinus(n1.left(), n2.left());

  case RETURNBOOL :
  case SEMI       :
    return false;

  default:
    if(n1.isAsymmetricFunction()) {
      return treesEqualMinus(n1.left(), n2.left());
    }

    return false;
  }
}
