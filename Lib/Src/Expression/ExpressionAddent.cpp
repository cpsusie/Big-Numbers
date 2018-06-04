#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

ExpressionAddent::ExpressionAddent(SNode n, bool positive)
: ExpressionNodeTree(&n.getTree(), ADDENT, SNodeArray(n.getTree(), 1,n))
, m_positive(positive)
{
  SETDEBUGSTRING();
}

ExpressionAddent::ExpressionAddent(ParserTree *tree, const ExpressionAddent *src)
: ExpressionNodeTree(tree, src)
, m_positive(src->isPositive())
{
  SETDEBUGSTRING();
}

ExpressionNode *ExpressionAddent::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionAddent(tree, this); TRACE_NEW(n);
  return n;
}

int ExpressionAddent::compare(ExpressionNode *n) {
  const bool b1 = isConstant();
  const bool b2 = n->isConstant();
  int c = ordinal(b1) - ordinal(b2);
  if(c) return c; // constants are last

  c = left()->compare(n->left());
  if(c) return c;
  const bool p1 = isPositive();
  const bool p2 = n->isPositive();
  return ordinal(p1) - ordinal(p2);
}

Real ExpressionAddent::evaluateReal() const {
  return m_positive ? left()->evaluateReal() : -left()->evaluateReal();
}

void ExpressionAddent::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += isPositive() ? _T("+ADDENT\n") : _T("-ADDENT\n");
  left()->dumpNode(s, level+1);
}

String ExpressionAddent::toString() const {
  if(m_positive) {
    return left()->toString();
  } else {
    return _T("-(") + left()->toString() + _T(")");
  }
}

}; // namespace Expr