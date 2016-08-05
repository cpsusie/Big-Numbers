#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionFactor.h>

void FactorArray::add(const ExpressionFactor *f) {
  const ExpressionNode *base     = f->base();
  const ExpressionNode *exponent = f->exponent();

  if(base->getSymbol() != PRODUCT) {
    if(!base->isOne() && !(exponent->isZero())) {
      CompactArray<const ExpressionFactor*>::add(f);
    }
  } else {
    const FactorArray &a = base->getFactorArray();
    if(exponent->isOne()) {
      addAll(a);
    } else {
      const ParserTree *tree = f->getTree();
      for(size_t i = 0; i < a.size(); i++) {
        const ExpressionFactor *f = a[i];
        add(tree->fetchFactorNode(f->base(), tree->productC(f->exponent(), exponent)));
      }
    }
  }
}

void FactorArray::add(const ExpressionNode *base, const ExpressionNode *exponent) {
  add(base->getTree()->fetchFactorNode(base, exponent));
}

FactorArray FactorArray::selectConstantPositiveExponentFactors() const {
  FactorArray result;
  for(size_t i = 0; i < size(); i++) {
    const ExpressionFactor *f = (*this)[i];
    if(f->exponent()->isPositive()) {
      result.add(f);
    }
  }
  return result;
}

FactorArray FactorArray::selectConstantNegativeExponentFactors() const {
  FactorArray result;
  for(size_t i = 0; i < size(); i++) {
    const ExpressionFactor *f = (*this)[i];
    if(f->exponent()->isNegative()) {
      result.add(f);
    }
  }
  return result;
}

FactorArray FactorArray::selectNonConstantExponentFactors() const {
  FactorArray result;
  for(size_t i = 0; i < size(); i++) {
    const ExpressionFactor *f = (*this)[i];
    if(!f->exponent()->isNumber()) {
      result.add(f);
    }
  }
  return result;
}

int FactorArray::findFactorWithChangeableSign() const {
  for(size_t i = 0; i < size(); i++) {
    if((*this)[i]->isConstant()) return (int)i;
  }
  for(size_t i = 0; i < size(); i++) {
    const ExpressionFactor *f = (*this)[i];
    if(f->hasOddExponent() && f->exponent()->isPositive()) return (int)i;
  }
  for(size_t i = 0; i < size(); i++) {
    if((*this)[i]->hasOddExponent()) return (int)i;
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
