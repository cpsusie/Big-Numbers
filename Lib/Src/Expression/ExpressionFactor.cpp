#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

ExpressionFactor::ExpressionFactor(SNode base)
: ExpressionNodeTree(&base.getTree(), POW, base.node(), base._1().node(), NULL)
{
}

ExpressionFactor::ExpressionFactor(SNode base, SNode exponent)
: ExpressionNodeTree(&base.getTree(), POW, base.node(), exponent.node(), NULL)
{
}

ExpressionNode *ExpressionFactor::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionFactor(base().node()->clone(tree), exponent().node()->clone(tree)); TRACE_NEW(n);
  return n;
}

int ExpressionFactor::compare(ExpressionNode *n) {
  if(n->getNodeType() != getNodeType()) {
    return ExpressionNode::compare(n);
  }
  ExpressionFactor *f = (ExpressionFactor*)n;
  bool b1 = isConstant();
  bool b2 = f->isConstant();
  int c = b1 - b2;
  if(c) return -c; // Constants are first

  b1 = exponent().isNumber();
  b2 = f->exponent().isNumber();
  c = b1 - b2;
  if(c) return c; // Numeric exponents are next

  if(b1) { // both have numeric exponents, but not constant bases. 1. priority: Order by exponent desc
    c = numberCmp(f->exponent().getNumber(), exponent().getNumber());
    if(c) return c;
    return base().node()->compare(f->base().node()); // 2. priority: Order by base
  }

  c = base().node()->compare(f->base().node());
  if(c) return c;
  return exponent().node()->compare(f->exponent().node());
}

bool ExpressionFactor::isConstant() const {
  if(exponent().isZero()) return true;
  return base().isConstant() && exponent().isConstant();
}

void ExpressionFactor::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += _T("POW\n");
  base().node()->dumpNode(s, level+1);
  exponent().node()->dumpNode(s, level+1);
}

}; // namespace Expr
