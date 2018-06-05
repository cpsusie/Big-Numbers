#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

ExpressionNodeAssign::ExpressionNodeAssign(ParserTree *tree, const ExpressionNodeAssign *src)
: ExpressionNodeTree(tree, src)
{
  SETDEBUGSTRING();
}

ExpressionNode *ExpressionNodeAssign::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodeAssign(tree, this); TRACE_NEW(n);
  return n;
}

Real &ExpressionNodeAssign::doAssignment() const {
  ExpressionVariable &var = left()->getVariable();
  Real &ref = left()->getValueRef();
  if(!var.isConstant()) {
    ref = right()->evaluateReal();
  }
  return ref;
}

String ExpressionNodeAssign::toString() const {
  return left()->toString() + _T(" = ") + right()->toString();
}

}; // namespace Expr
