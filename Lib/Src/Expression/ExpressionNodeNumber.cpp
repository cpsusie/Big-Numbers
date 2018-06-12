#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

int ExpressionNodeNumber::compare(const ExpressionNode *n) const {
  if(n->getNodeType() != getNodeType()) {
    return __super::compare(n);
  }
  return numberCmp(getNumber(), n->getNumber());
}

ExpressionNode *ExpressionNodeNumber::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodeNumber(tree, m_number); TRACE_NEW(n);
  return n;
}

bool ExpressionNodeNumber::traverseExpression(ExpressionNodeHandler &handler, int level) {
  return handler.handleNode(this, level);
}

void ExpressionNodeNumber::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += format(_T("NUMBER:%s valueIndex:%2d\n"), getNumber().toString().cstr(), getValueIndex());
}

}; // namespace Expr
