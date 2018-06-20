#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

ExpressionNodeAddent::ExpressionNodeAddent(SNode n, bool positive)
: ExpressionNodeTree(&n.getTree(), ADDENT, n.node(), NULL)
, m_positive(positive)
{
  SETDEBUGSTRING();
}

ExpressionNodeAddent::ExpressionNodeAddent(ParserTree *tree, const ExpressionNodeAddent *src)
: ExpressionNodeTree(tree, src)
, m_positive(src->isPositive())
{
  SETDEBUGSTRING();
}

ExpressionNode *ExpressionNodeAddent::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodeAddent(tree, this); TRACE_NEW(n);
  return n;
}

Real ExpressionNodeAddent::evaluateReal() const {
  return m_positive ? left()->evaluateReal() : -left()->evaluateReal();
}

int ExpressionNodeAddent::compare(const ExpressionNode *n) const {
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

bool ExpressionNodeAddent::equal(const ExpressionNode *n) const {
  return (((isPositive() == n->isPositive()) && Expr::equal(     left(),n->left()))
       || ((isPositive() != n->isPositive()) && Expr::equalMinus(left(),n->left())));
}

bool ExpressionNodeAddent::equalMinus(const ExpressionNode *n) const {
  return (((isPositive() != n->isPositive()) && Expr::equal(     left(),n->left()))
       || ((isPositive() == n->isPositive()) && Expr::equalMinus(left(),n->left())));
}

void ExpressionNodeAddent::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += isPositive() ? _T("+ADDENT\n") : _T("-ADDENT\n");
  left()->dumpNode(s, level+1);
}

String ExpressionNodeAddent::toString() const {
  if(m_positive) {
    return left()->toString();
  } else {
    return _T("-(") + left()->toString() + _T(")");
  }
}

}; // namespace Expr