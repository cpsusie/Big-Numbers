#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

bool ExpressionNode::reducesToRational(Rational *r) const {
  Number v;
  if(!isConstant(&v) || !v.isRational()) {
    return false;
  }
  if(r != NULL) {
    *r = v.getRationalValue();
  }
  return true;
}

}; // namespace Expr
