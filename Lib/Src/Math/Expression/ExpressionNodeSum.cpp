#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/SumElement.h>

static int compareSumElement(const SumElement * const &e1, const SumElement * const &e2) {
  return e1->compare(e2);
}

ExpressionNodeSum::ExpressionNodeSum(const ParserTree *tree, const AddentArray &elements) : ExpressionNode(tree, SUM) {
  m_elements = elements;
  m_elements.sort(compareSumElement);
  INITEXPRESSIONNODEDEBUGSTRING();
}

const ExpressionNodeSum *ExpressionNodeSum::multiply(const ExpressionNodeSum *n1, const ExpressionNodeSum *n2) { // static
  AddentArray newAddentArray;

  const AddentArray &aa1 = n1->getAddentArray();
  const AddentArray &aa2 = n2->getAddentArray();

  const ParserTree *tree = n1->getTree();
  const bool oldEnable = AddentArray::enableDebugString(false);
  for(size_t i = 0; i < aa1.size(); i++) {
    const SumElement *e1 = aa1[i];
    const SNode       s1 = e1->getNode();
    for(size_t j = 0; j < aa2.size(); j++) {
      const SumElement *e2 = aa2[j];
      const SNode       s2 = e2->getNode();
      newAddentArray.add(s1 * s2, e1->isPositive() == e2->isPositive());
    }
  }
  AddentArray::enableDebugString(oldEnable);  INITADDENTARRAYDEBUGSTRING(&newAddentArray);

  return new ExpressionNodeSum(tree, newAddentArray); // dont use getSum here. It has to be an ExpressionNodeSum
}

int ExpressionNodeSum::compare(const ExpressionNode *n) const {
  if(n->getSymbol() != SUM) {
    return ExpressionNode::compare(n);
  }
  const AddentArray &na = n->getAddentArray();
  const int count = min(m_elements.size(), na.size());
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

const ExpressionNode *ExpressionNodeSum::clone(const ParserTree *tree) const {
  AddentArray a(m_elements.size());
  for(size_t i = 0; i < m_elements.size(); i++) {
    a.add(m_elements[i]->clone(tree));
  }
  return new ExpressionNodeSum(tree, a);
}

bool ExpressionNodeSum::isConstant() const {
  const int n = m_elements.size();
  for(int i = 0; i < n; i++) {
    if(!m_elements[i]->isConstant()) {
      return false;
    }
  }
  return true;
}

bool ExpressionNodeSum::traverseExpression(ExpressionNodeHandler &handler, int level) const {
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
    const SumElement *e = m_elements[i];
    addLeftMargin(s, level+1) += e->isPositive() ? _T("+\n") : _T("\x96\n");
    e->getNode()->dumpNode(s, level+2);
  }
}

String ExpressionNodeSum::toString() const {
  bool first = true;
  String result;
  const int n = m_elements.size();
  for(int i = 0; i < n; i++) {
    const SumElement *e = m_elements[i];
    if(e->isPositive()) {
      if(!first) {
        result += _T(" + ");
      }
    } else {
      result += _T(" - ");
    }
    first = false;
    result += e->getNode()->parenthesizedExpressionToString(this);
  }
  return result;
}

#ifdef _DEBUG
void ExpressionNodeSum::initDebugString() {
  m_debugString = m_elements.getDebugString();
}
#endif

