#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/SumElement.h>

namespace Expr {

ExpressionNodeSum::ExpressionNodeSum(ParserTree *tree, const AddentArray &elements) : ExpressionNode(tree, SUM) {
  m_elements = elements;
  m_elements.sort();
  SETDEBUGSTRING();
}

int ExpressionNodeSum::compare(ExpressionNode *n) {
  if(n->getSymbol() != SUM) {
    return ExpressionNode::compare(n);
  }
  const AddentArray &na = n->getAddentArray();
  const int count = (int)min(m_elements.size(), na.size());
  int i;
  for(i = 0; i < count; i++) {
    const int c = m_elements[i]->compare(na[i]);
    if(c) return c;
  }
  if(i == m_elements.size()) {
    return (i == na.size()) ? 0 : 1;
  } else {
    return -1;
  }
}

ExpressionNode *ExpressionNodeSum::clone(ParserTree *tree) const {
  AddentArray a(m_elements.size());
  for(size_t i = 0; i < m_elements.size(); i++) {
    a.add(m_elements[i]->clone(tree));
  }
  ExpressionNode *n = new ExpressionNodeSum(tree, a); TRACE_NEW(n);
  return n;
}

bool ExpressionNodeSum::isConstant() const {
  const int n = (int)m_elements.size();
  for(int i = 0; i < n; i++) {
    if(!m_elements[i]->isConstant()) {
      return false;
    }
  }
  return true;
}

Real ExpressionNodeSum::evaluateReal() const {
  Real sum = 0;
  const AddentArray &a = getAddentArray();
  for(size_t i = 0; i < a.size(); i++) {
    const SumElement *e = a[i];
    const Real v = e->getNode()->evaluateReal();
    if(e->isPositive()) {
      sum += v;
    } else {
      sum -= v;
    }
  }
  return sum;
}

bool ExpressionNodeSum::traverseExpression(ExpressionNodeHandler &handler, int level) {
  if(!handler.handleNode(this, level)) return false;
  const AddentArray &a = getAddentArray();
  level++;
  for(size_t i = 0; i < a.size(); i++) {
    if(!a[i]->getNode()->traverseExpression(handler, level)) return false;
  }
  return true;
}

void ExpressionNodeSum::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += format(_T("%s\n"), getSymbolName().cstr());
  for(size_t i = 0; i < m_elements.size(); i++) {
    SumElement *e = m_elements[i];
    addLeftMargin(s, level+1) += e->isPositive() ? _T("+\n") : _T("\x96\n");
    e->getNode()->dumpNode(s, level+2);
  }
}

String ExpressionNodeSum::toString() const {
  return m_elements.toString();
}

}; // namespace Expr
