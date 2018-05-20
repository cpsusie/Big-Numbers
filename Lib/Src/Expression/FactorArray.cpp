#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionFactor.h>

namespace Expr {

void FactorArray::add(ExpressionFactor *f) {
  SNode base     = f->base();
  SNode exponent = f->exponent();

  if(base.getSymbol() != PRODUCT) {
    if(!base.isOne() && !(exponent.isZero())) {
      CompactArray<ExpressionFactor*>::add(f);
    }
  } else {
    const FactorArray &a = base.getFactorArray();
    if(exponent.isOne()) {
      addAll(a);
    } else {
      ParserTree *tree = f->getTree();
      for(size_t i = 0; i < a.size(); i++) {
        ExpressionFactor *f = a[i];
        add(f->base(), f->exponent() * exponent);
      }
    }
  }
}

void FactorArray::add(SNode base, SNode exponent) {
  add(base.getTree()->fetchFactorNode(base, exponent));
}

void FactorArray::add(SNode base) {
  if (base.getNodeType() == EXPRESSIONNODEFACTOR) {
    __super::add((ExpressionFactor*)base.node());
  } else {
    add(base,base._1());
  }
}

void FactorArray::add(SNode base, const Rational &exponent) {
  add(base, base.getTree()->numberExpression(exponent));
}

FactorArray FactorArray::selectConstantPositiveExponentFactors() const {
  FactorArray result;
  for(size_t i = 0; i < size(); i++) {
    ExpressionFactor *f = (*this)[i];
    if(f->exponent().isPositive()) {
      result.add(f);
    }
  }
  return result;
}

FactorArray FactorArray::selectConstantNegativeExponentFactors() const {
  FactorArray result;
  for(size_t i = 0; i < size(); i++) {
    ExpressionFactor *f = (*this)[i];
    if(f->exponent().isNegative()) {
      result.add(f);
    }
  }
  return result;
}

FactorArray FactorArray::selectNonConstantExponentFactors() const {
  FactorArray result;
  for(size_t i = 0; i < size(); i++) {
    ExpressionFactor *f = (*this)[i];
    if(!f->exponent().isNumber()) {
      result.add(f);
    }
  }
  return result;
}

int FactorArray::findFactorWithChangeableSign() const {
  const UINT n = (UINT)size();
  for(UINT i = 0; i < n; i++) {
    if((*this)[i]->isConstant()) return i;
  }
  for(UINT i = 0; i < n; i++) {
    ExpressionFactor *f = (*this)[i];
    if(f->hasOddExponent() && f->exponent().isPositive()) return i;
  }
  for(UINT i = 0; i < n; i++) {
    if((*this)[i]->hasOddExponent()) return i;
  }
  return -1;
}

String FactorArray::toString() const {
  if(size() == 0) {
    return _T("1");
  }
  String result = format(_T("(%s)"), (*this)[0]->toString().cstr());;
  for(size_t i = 1; i < size(); i++) {
    result += format(_T("*(%s)"), (*this)[i]->toString().cstr());
  }
  return result;
}

}; // namespace Expr
