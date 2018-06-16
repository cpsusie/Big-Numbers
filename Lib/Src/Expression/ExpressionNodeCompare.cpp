#include "pch.h"
#include <Math/Expression/ExpressionNode.h>

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
  case NUMBER  :
    return n1->getNumber() == n2->getNumber();
  case TYPEBOOL:
    return n1->getBool()   == n2->getBool();
  case NAME    :
    return n1->getName().equalsIgnoreCase(n2->getName());
  case MINUS :
    assert(n1->isUnaryMinus());
    assert(n2->isUnaryMinus());
    return equal(n1->left(), n2->left());

  case ADDENT   :
    return (equal(     n1->left(),n2->left()) && (n1->isPositive() == n2->isPositive()))
        || (equalMinus(n1->left(),n2->left()) && (n1->isPositive() != n2->isPositive()));

  case PRODUCT  :
    return n1->getFactorArray().equal(n2->getFactorArray());

  case MOD      :
    if(!equal(n1->left(), n2->left())) return false;
    return equal(n1->right(), n2->right()) || equalMinus(n1->right(), n2->right());

  case POLY     :
    return ((ExpressionNodePoly*)n1)->equal(n2);

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
    if(n1->isSymmetricFunction()) {
      return equal(n1->left(), n2->left()) || equalMinus(n1->left(), n2->left());
    }
    return n1->getChildArray().equal(n2->getChildArray());
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
  case ADDENT:
    return (equal(     n1->left(),n2->left()) && (n1->isPositive() != n2->isPositive()))
        || (equalMinus(n1->left(),n2->left()) && (n1->isPositive() == n2->isPositive()));

  case SUM   :
    { const AddentArray &a1 = n1->getAddentArray();
      const AddentArray &a2 = n2->getAddentArray();
      const size_t      sz1 = a1.size();
      if(sz1 != a2.size()) {
        return false;
      }
      for(size_t i = 0; i < sz1; i++) {
        if(!equalMinus(a1[i].node(), a2[i].node())) {
          return false;
        }
      }
    }
    return true;
  case MINUS :
    assert(n1->isUnaryMinus());
    assert(n2->isUnaryMinus());
    return equalMinus(n1->left(), n2->left());

  case PRODUCT  :
    return n1->getFactorArray().equalMinus(n2->getFactorArray());

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
    return ((ExpressionNodePoly*)n1)->equalMinus(n2);

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
