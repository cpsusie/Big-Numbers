#include "pch.h"
#include <Math/Expression/ExpressionNode.h>

namespace Expr {

// Should only be called in Canonical treeform
bool equal(const ExpressionNode *n1, const ExpressionNode *n2) {
  if(n1 == n2) {
    return true;
  } else if((n1 == nullptr) || (n2 == nullptr) || (n1->getSymbol() != n2->getSymbol())) {
    return false;
  }
  return n1->equal(n2);
}

// Should only be called in Canonical treeform
bool equalMinus(const ExpressionNode *n1, const ExpressionNode *n2) {
  if((n1 == n2) || (n1 == nullptr) || (n2 == nullptr)) {
    return false;
  }

  if(n1->getSymbol() != n2->getSymbol()) { // special case
    if(n1->isUnaryMinus()) return equal(n1->left(), n2);
    if(n2->isUnaryMinus()) return equal(n2->left(), n1);
    return false;
  }
  return n1->equalMinus(n2);
}

}; // namespace Expr
