#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionFactor.h>

DEFINECLASSNAME(ExpressionFactor);

ExpressionNode *ExpressionFactor::clone(ParserTree *tree) const {
  return new ExpressionFactor(base()->clone(tree), exponent()->clone(tree));
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

  b1 = exponent()->isNumber();
  b2 = f->exponent()->isNumber();
  c = b1 - b2;
  if(c) return c; // Numeric exponents are next

  if(b1) { // both have numeric exponents, but not constant bases. 1. priority: Order by exponent desc
    c = numberCmp(f->exponent()->getNumber(), exponent()->getNumber());
    if(c) return c;
    return base()->compare(f->base()); // 2. priority: Order by base
  }

  c = base()->compare(f->base());
  if(c) return c;
  return exponent()->compare(f->exponent());
}

bool ExpressionFactor::isConstant() const {
  const ExpressionNode *expo = exponent();
  if(expo->isZero()) return true;
  return base()->isConstant() && expo->isConstant();
}

void ExpressionFactor::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += _T("POW\n");
  base()->dumpNode(s, level+1);
  exponent()->dumpNode(s, level+1);
}
