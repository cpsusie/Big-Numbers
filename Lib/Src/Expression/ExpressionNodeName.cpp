#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionSymbolTable.h>

namespace Expr {

ExpressionNodeName::ExpressionNodeName(ParserTree *tree, const String &name) : ExpressionNode(tree, NAME) {
  m_name = name;
  getSymbolTable().allocateSymbol(this, false, false, false);
  SETDEBUGSTRING();
}

void ExpressionNodeName::setName(const String &name) {
  m_name = name;
  ExpressionVariable *v = getSymbolTable().getVariable(getName());
  if(v) {
    setVariable(v);
  } else {
    getSymbolTable().allocateSymbol(this, false, false, false);
  }
  SETDEBUGSTRING();
}

int ExpressionNodeName::compare(const ExpressionNode *n) const {
  if(n->getNodeType() != getNodeType()) {
    return __super::compare(n);
  }
  return stringiHashCmp(getName(), n->getName());
}

bool ExpressionNodeName::equal(const ExpressionNode *n) const {
  return getName().equalsIgnoreCase(n->getName());
}

bool ExpressionNodeName::equalMinus(const ExpressionNode *n) const {
  return false;
}

bool ExpressionNodeName::isConstant(Number *v) const {
  const bool result = m_var->isConstant();
  if(result && (v != NULL)) {
    *v = getSymbolTable().getValue(m_var->getValueIndex());
  }
  return result;
}

ExpressionNode *ExpressionNodeName::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodeName(tree, m_name); TRACE_NEW(n);
  return n;
}

bool ExpressionNodeName::traverseNode(ExpressionNodeHandler &handler) {
  return handler.handleNode(this);
}

void ExpressionNodeName::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += format(_T("NAME:%s"), getName().cstr());
  if(m_var == NULL) {
    s += _T(" var=NULL\n");
  } else if(!m_var->isMarked()) {
    s += format(_T(" %s\n"), m_var->toString().cstr());
    m_var->mark();
  } else {
    s += _T("\n");
  }
}

}; // namespace Expr
