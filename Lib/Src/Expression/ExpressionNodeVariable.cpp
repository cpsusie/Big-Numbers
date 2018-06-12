#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

ExpressionNodeVariable::ExpressionNodeVariable(ParserTree *tree, const String &name) : ExpressionNode(tree, NAME) {
  m_name = name;
  tree->m_symbolTable.allocateSymbol(this, false, false, false);
  SETDEBUGSTRING();
}

int ExpressionNodeVariable::compare(const ExpressionNode *n) const {
  if(n->getNodeType() != getNodeType()) {
    return __super::compare(n);
  }
  return stringiHashCmp(getName(), n->getName());
}

ExpressionNode *ExpressionNodeVariable::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodeVariable(tree, m_name); TRACE_NEW(n);
  return n;
}

bool ExpressionNodeVariable::traverseExpression(ExpressionNodeHandler &handler, int level) {
  return handler.handleNode(this, level);
}

void ExpressionNodeVariable::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += format(_T("NAME:%s"), getName().cstr());
  if(m_var == NULL) {
    s += _T(" var=NULL\n");
  } else if(!m_var->isMarked()) {
    s += format(_T(" var=%s\n"), m_var->toString().cstr());
    m_var->mark();
  } else {
    s += _T("\n");
  }
}

}; // namespace Expr
