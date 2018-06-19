#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

ExpressionNode *ExpressionNodePower::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodePower(base().node()->clone(tree), exponent().node()->clone(tree)); TRACE_NEW(n);
  return n;
}

static Rational reduceToRational(SNode n) {
  Rational r;
  if(!n.reducesToRational(&r)) {
    throwInvalidArgumentException(__TFUNCTION__
                                 ,_T("n=%s doesn't reduce to rational")
                                 ,n.toString().cstr());
  }
  return r;
}

SNode ExpressionNodePower::expand() const {
  if(!isExpandable()) {
    throwUnExpandableException();
  }
  ParserTree     &tree  = getTree();
  ExpressionNode *expo  = right();
  Rational        expoR = reduceToRational(expo);
  return tree.expandPower(left(), expoR);
}

bool ExpressionNodePower::isExpandable() const {
  ExpressionNode *expo = right();
  Rational        expoR;
  if(!expo->reducesToRational(&expoR) || (::abs(expoR.getNumerator()) <= 1)) {
    return false;
  }

  const ExpressionNode *base = left();
  switch(base->getSymbol()) {
  case MINUS:
    if(base->isUnaryMinus()) {
      return false;
    }
    // NB continue case;
  case SUM  :
  case PLUS :
    return true;
  default   :
    return false;
  }
}

int ExpressionNodePower::compare(const ExpressionNode *n) const {
  if(n->getNodeType() != getNodeType()) {
    return ExpressionNode::compare(n);
  }
  ExpressionNodePower *f = (ExpressionNodePower*)n;
  bool b1 = isConstant();
  bool b2 = f->isConstant();
  int c = ordinal(b1) - ordinal(b2);
  if(c) return -c; // Constants are first

  b1 = exponent().isNumber();
  b2 = f->exponent().isNumber();
  c = ordinal(b1) - ordinal(b2);
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

bool ExpressionNodePower::isConstant(Number *v) const {
  if(exponent().isZero()) {
    if(v != NULL) {
      *v = 1;
    }
    return true;
  }
  if(base().isZero()) {
    if(v != NULL) {
      *v = 0;
    }
    return true;
  }
  if(base().isOne()) {
    if(v != NULL) {
      *v = 1;
    }
    return true;
  }
  if(v != NULL) {
    Rational baseR, expoR;
    if(base().reducesToRational(&baseR) && exponent().reducesToRational(&expoR)) {
      Rational powR;
      if(Rational::isRationalPow(baseR, expoR, &powR)) {
        *v = powR;
        return true;
      }
    }
  }
  return __super::isConstant(v);
}

Real ExpressionNodePower::evaluateReal() const {
  Real base = left()->evaluateReal();
  const Real expo = right()->evaluateReal();
  if(base >= 0) {
    return mypow(base, expo);
  } else {
    Rational expoR;
    if(Rational::isRational(expo, &expoR)) {
      if(expoR.getNumerator() != 1) {
        base = mypow(base, (Real)expoR.getNumerator());
      }
      if(expoR.getDenominator() != 1) {
        base = ::root(base, (Real)expoR.getDenominator());
      }
      return base;
    } else {
      return ::pow(base, expo); // -1.#IND
    }
  }
}

}; // namespace Expr
