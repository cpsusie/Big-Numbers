#include "pch.h"
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>

namespace Expr {

// Should only be called in Canonical treeform
bool equal(const ExpressionNode *n1, const ExpressionNode *n2) {
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
        if((e1->isPositive() != e2->isPositive()) || !equal(e1->getNode(), e2->getNode())) return false;
      }
    }
    return true;

  case MINUS :
    assert(n1->isUnaryMinus());
    assert(n2->isUnaryMinus());
    return equal(n1->left(), n2->left());

  case PRODUCT  :
    { const FactorArray &a1 = n1->getFactorArray(); // they have been sorted
      const FactorArray &a2 = n2->getFactorArray();
      if(a1.size() != a2.size()) return false;
      for(size_t i = 0; i < a1.size(); i++) {
        if(!equal(a1[i], a2[i])) return false;
      }
    }
    return true;

  case MOD   :
    if(!equal(n1->left(), n2->left())) return false;
    return equal(n1->right(), n2->right()) || equalMinus(((ExpressionNode*)n1)->right(), ((ExpressionNode*)n2)->right());

  case POLY  :
    { const SNodeArray &coef1 = n1->getCoefArray();
      const SNodeArray &coef2 = n2->getCoefArray();
      const SNode       arg1  = n1->getArgument();
      const SNode       arg2  = n2->getArgument();
      const size_t      cn1   = coef1.size();
      if(cn1 != coef2.size()) return false;
      for(size_t i = 0; i < cn1; i++) {
        if(!equal(coef1[i].node(), coef2[i].node())) return false;
      }
      return equal(arg1.node(), arg2.node());
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
    n1->throwInvalidSymbolForTreeMode(method);

  default:
    { const SNodeArray &a1  = n1->getChildArray();
      const SNodeArray &a2  = n2->getChildArray();
      const size_t      sz1 = a1.size();
      if(sz1 != a2.size()) return false;
      for(size_t i = 0; i < sz1; i++) {
        if(!equal(a1[i].node(), a2[i].node())) return false;
      }
    }
    return true;
  }
}

// Should only be called in Canonical treeform
bool equalMinus(const ExpressionNode *n1, const ExpressionNode *n2) {
  DEFINEMETHODNAME;

  if((n1 == n2) || (n1 == NULL) || (n2 == NULL)) {
    return false;
  }

  if(n1->getSymbol() != n2->getSymbol()) { // special case
    if(n1->isUnaryMinus()) return equal(n1->left(), n2);
    if(n2->isUnaryMinus()) return equal(n2->left(), n1);
    return false;
  }
  // symbols are the same
  switch(n1->getSymbol()) {
  case NUMBER:
    return n1->getNumber() == -n2->getNumber();
  case NAME  :
    return false;
  case SUM:
    { const AddentArray &a1  = n1->getAddentArray(); // they have been sorted
      const AddentArray &a2  = n2->getAddentArray();
      const size_t       sz1 = a1.size();
      if(sz1 != a2.size()) return false;
      for(size_t i = 0; i < sz1; i++) {
        SumElement *e1 = a1[i];
        SumElement *e2 = a2[i];
        if(e1->isPositive() == e2->isPositive()) {
          if(!equalMinus(e1->getNode(), e2->getNode())) {
            return false;
          }
        } else { // e1->isPositive() != e2->isPositive()
          if(!equal(e1->getNode(), e2->getNode())) {
            return false;
          }
        }
      }
    }
    return true;
  case MINUS :
    assert(n1->isUnaryMinus());
    assert(n2->isUnaryMinus());
    return equalMinus(n1->left(), n2->left());

  case PRODUCT  :
    { const FactorArray &a1 = n1->getFactorArray(); // they have been sorted
      const FactorArray &a2 = n2->getFactorArray();
      if(a1.size() != a2.size()) {
        return false;
      }
      int signShiftCount = 0;
      for(size_t i = 0; i < a1.size(); i++) {
        if(equal(a1[i], a2[i])) continue;
        if(equalMinus(a1[i], a2[i])) {
          signShiftCount++;
          continue;
        }
        return false;
      }
      return isOdd(signShiftCount);
    }

  case MOD   :
    if(!equalMinus(n1->left(), n2->left())) {
      return false;
    }
    return equal(n1->right(), n2->right()) || equalMinus(n1->right(), n2->right());

  case POW   :
    { const ExpressionNode *b1 = n1->left();
      const ExpressionNode *b2 = n2->left();
      const ExpressionNode *e1 = n1->right();
      const ExpressionNode *e2 = n2->right();
      Rational er1, er2;
      if(!e1->reducesToRationalConstant(&er1) || !e2->reducesToRationalConstant(&er2)) return false;
      return (er1 == er2) 
          && isOdd(er1.getNumerator()) 
          && isOdd(er1.getDenominator())
          && equalMinus(b1, b2);
    }

  case INDEXEDSUM    :
    return equal(n1->left(), n2->left()) && equal(n1->right(), n2->right())
        && equalMinus(n1->child(2).node(), n2->child(2).node());

  case POLY  :
    { const SNodeArray &coefList1 = n1->getCoefArray();
      const SNodeArray &coefList2 = n2->getCoefArray();
      const SNode       arg1      = n1->getArgument();
      const SNode       arg2      = n2->getArgument();
      const size_t      c1size    = coefList1.size();
      if(c1size != coefList2.size()) {
        return false;
      }
      if(arg1 != arg2) return false;
      for(size_t i = 0; i < c1size; i++) {
        if(!equalMinus(coefList1[i].node(), coefList2[i].node())) {
          return false;
        }
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
    n1->throwInvalidSymbolForTreeMode(method);

  default:
    if(n1->isAsymmetricFunction()) {
      return equalMinus(n1->left(), n2->left());
    }
    return false;
  }
}

}; // namespace Expr
