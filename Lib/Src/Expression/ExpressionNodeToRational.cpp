#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionNode.h>

namespace Expr {

bool ExpressionNode::reducesToRational(Rational *r) const {
  Number v;
  if(!isConstant(&v) || !v.isRational()) {
    return false;
  }
  if(r != nullptr) {
    *r = (Rational)v;
  }
  return true;
}

}; // namespace Expr
