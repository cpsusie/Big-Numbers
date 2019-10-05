#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

int ExpressionNodeNumber::compare(const ExpressionNode *n) const {
  if(n->getNodeType() != getNodeType()) {
    return __super::compare(n);
  }
  return numberCmp(getNumber(), n->getNumber());
}

bool ExpressionNodeNumber::equal(const ExpressionNode *n) const {
  return getNumber() == n->getNumber();
}

bool ExpressionNodeNumber::equalMinus(const ExpressionNode *n) const {
  return getNumber() == -n->getNumber();
}

bool ExpressionNodeNumber::isConstant(Number *v) const {
  if(v != NULL) {
    *v = getNumber();
  }
  return true;
}

ExpressionNode *ExpressionNodeNumber::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodeNumber(tree, m_number); TRACE_NEW(n);
  return n;
}

bool ExpressionNodeNumber::traverseNode(ExpressionNodeHandler &handler) {
  return handler.handleNode(this);
}

void ExpressionNodeNumber::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += format(_T("NUMBER:%s V.Index:%2d\n"), ::toString(getNumber()).cstr(), getValueIndex());
}

}; // namespace Expr
