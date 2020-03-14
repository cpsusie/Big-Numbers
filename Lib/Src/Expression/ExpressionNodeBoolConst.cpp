#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionNode.h>

namespace Expr {

ExpressionNode *ExpressionNodeBoolConst::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodeBoolConst(tree, getBool()); TRACE_NEW(n);
  return n;
}

int ExpressionNodeBoolConst::compare(const ExpressionNode *n) const {
  if(n->getNodeType() != getNodeType()) {
    return __super::compare(n);
  }
  return ordinal(getBool()) - ordinal(n->getBool());
}

bool ExpressionNodeBoolConst::equal(const ExpressionNode *n) const {
  return getBool() == n->getBool();
}

bool ExpressionNodeBoolConst::traverseNode(ExpressionNodeHandler &handler) {
  return handler.handleNode(this);
}

void ExpressionNodeBoolConst::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += format(_T("BOOLEAN:%s\n"), boolToStr(getBool()));
}

}; // namespace Expr
