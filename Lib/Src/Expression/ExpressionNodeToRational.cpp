#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

bool ExpressionNode::reducesToRational(Rational *r) const {
  Number v;
  if(!isConstant(&v) || !v.isRational()) {
    return false;
  }
  if(r != NULL) {
    *r = ::getRational(v);
  }
  return true;
}

}; // namespace Expr
