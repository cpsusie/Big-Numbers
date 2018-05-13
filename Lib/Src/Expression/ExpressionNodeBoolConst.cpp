#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

int ExpressionNodeBoolConst::compare(ExpressionNode *n) {
  if(n->getNodeType() != getNodeType()) {
    return ExpressionNode::compare(n);
  }
  return ordinal(getBool()) - ordinal(n->getBool());
}

ExpressionNode *ExpressionNodeBoolConst::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodeBoolConst(tree, getBool()); TRACE_NEW(n);
  return n;
}

bool ExpressionNodeBoolConst::traverseExpression(ExpressionNodeHandler &handler, int level) {
  return handler.handleNode(this, level);
}

void ExpressionNodeBoolConst::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += format(_T("BOOLEAN:%s\n"), boolToStr(getBool()));
}

}; // namespace Expr
