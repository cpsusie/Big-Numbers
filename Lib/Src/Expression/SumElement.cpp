#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/SNode.h>

namespace Expr {

void SumElement::init(ExpressionNode *n, bool positive) {
  if(n->getSymbol() == POW) {
    if(n->left()->isOne() || n->right()->isOne()) {
      m_n = n->left();
    } else if(n->right()->isZero()) {
      m_n = n->getTree()->getOne();
    } else {
      m_n = n;
    }
  } else {
    m_n = n;
  }
  m_positive = positive;
  if(m_n->isNegative()) {
    m_n = m_n->getTree()->minus(m_n);
    m_positive = !m_positive;
  }
  m_n->getTree()->m_addentTable.add(this);
}

SumElement::SumElement(const SNode &n, bool positive) {
  init(n.node(), positive);
}

const ExpressionNode *SumElement::createExpressionNode() const {
  return m_positive ? m_n : m_n->getTree()->minus(m_n);
}

bool SumElement::isConstant() const {
  return m_n->isConstant();
}

SumElement *SumElement::clone(ParserTree *tree) const {
  SumElement *e = new SumElement(m_n->clone(tree), m_positive); TRACE_NEW(e);
  return e;
}

int SumElement::compare(const SumElement *e) const {
  const bool b1 = isConstant();
  const bool b2 = e->isConstant();
  int c = b1 - b2;
  if(c) return c; // constants are last

  c = m_n->compare(e->m_n);
  if(c) return c;
  const bool p1 = m_positive;
  const bool p2 = e->m_positive;
  return p1 - p2;
}

String SumElement::toString() const {
  if(m_positive) {
    return m_n->toString();
  } else {
    return _T("-(") + m_n->toString() + _T(")");
  }
}

}; // namespace Expr
