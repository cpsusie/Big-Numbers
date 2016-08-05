#include "pch.h"
#include <Math/Expression/ParserTree.h>

ExpressionNodeVariable::ExpressionNodeVariable(const ParserTree *tree, const String &name) : ExpressionNode(tree, NAME) {
  m_name = name;
  tree->allocateSymbol(this, false, false, false);
}

int ExpressionNodeVariable::compare(const ExpressionNode *n) const {
  if(n->getNodeType() != getNodeType()) {
    return ExpressionNode::compare(n);
  }
  return stringHashCmp(getName(), n->getName());
}

const ExpressionNode *ExpressionNodeVariable::clone(const ParserTree *tree) const {
  return new ExpressionNodeVariable(tree, m_name);
}

bool ExpressionNodeVariable::traverseExpression(ExpressionNodeHandler &handler, int level) const {
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
