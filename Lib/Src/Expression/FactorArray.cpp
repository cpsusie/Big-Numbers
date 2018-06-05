#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

static int compareFactors(ExpressionFactor * const &f1, ExpressionFactor * const &f2) {
  return f1->compare(f2);
}

FactorArray &FactorArray::sort() {
  __super::sort(compareFactors);
  SETDEBUGSTRING();
  return *this;
}

void FactorArray::clear(intptr_t capacity) {
  __super::clear(capacity);
  SETDEBUGSTRING();
}

void FactorArray::add(ExpressionFactor *f) { // private
  SNode base     = f->base();
  SNode exponent = f->exponent();

  DISABLEDEBUGSTRING(*this);
  if(base.getSymbol() != PRODUCT) {
    if(!base.isOne() && !(exponent.isZero())) {
      __super::add(f);
    }
  } else {
    const FactorArray &a = base.getFactorArray();
    if(exponent.isOne()) {
      addAll(a);
    } else {
      ParserTree &tree = f->getTree();
      for(size_t i = 0; i < a.size(); i++) {
        ExpressionFactor *f = a[i];
        add(f->base(), tree.multiplyExponents(f->right(),exponent.node()));
      }
    }
  }
  ENABLEDEBUGSTRING(*this);
}

void FactorArray::addAll(const FactorArray &src) {
  DISABLEDEBUGSTRING(*this);
  __super::addAll(src);
  ENABLEDEBUGSTRING(*this);
}

void FactorArray::remove(size_t index) {
  __super::remove(index);
  SETDEBUGSTRING();
}


void FactorArray::add(SNode base, SNode exponent) {
  add(factorExp(base, exponent));
}

void FactorArray::add(SNode base) {
  if(base.getNodeType() == NT_FACTOR) {
    __super::add((ExpressionFactor*)base.node());
    SETDEBUGSTRING();
  } else {
    add(factorExp(base,base._1()));
  }
}

void FactorArray::add(SNode base, const Rational &exponent) {
  add(factorExp(base, base.getTree().numberExpr(exponent)));
}

FactorArray FactorArray::selectConstantPositiveExponentFactors() const {
  FactorArray result(getTree());
  DISABLEDEBUGSTRING(result);
  for(size_t i = 0; i < size(); i++) {
    ExpressionFactor *f = (*this)[i];
    if(f->exponent().isPositiveNumber()) {
      result.add(f);
    }
  }
  ENABLEDEBUGSTRING(result);
  return result;
}

FactorArray FactorArray::selectConstantNegativeExponentFactors() const {
  FactorArray result(getTree());
  DISABLEDEBUGSTRING(result);
  for(size_t i = 0; i < size(); i++) {
    ExpressionFactor *f = (*this)[i];
    if(f->exponent().isNegativeNumber()) {
      result.add(f);
    }
  }
  ENABLEDEBUGSTRING(result);
  return result;
}

FactorArray FactorArray::selectNonConstantExponentFactors() const {
  FactorArray result(getTree());
  DISABLEDEBUGSTRING(result);
  for(size_t i = 0; i < size(); i++) {
    ExpressionFactor *f = (*this)[i];
    if(!f->exponent().isNumber()) {
      result.add(f);
    }
  }
  ENABLEDEBUGSTRING(result);
  return result;
}

int FactorArray::findFactorWithChangeableSign() const {
  const size_t n = size();
  for(UINT i = 0; i < n; i++) {
    if((*this)[i]->isConstant()) {
      return i;
    }
  }
  for(UINT i = 0; i < n; i++) {
    ExpressionFactor *f = (*this)[i];
    if(f->hasOddExponent() && f->exponent().isPositiveNumber()) {
      return i;
    }
  }
  for(UINT i = 0; i < n; i++) {
    if((*this)[i]->hasOddExponent()) {
      return i;
    }
  }
  return -1;
}

// compare if ExpressionNode* equals
bool FactorArray::isSameNodes(const FactorArray &a) const {
  return __super::operator==(a);
}

bool FactorArray::equal(const FactorArray &a) const {
  const size_t n = size();
  if(a.size() != n) return false;
  for(size_t i = 0; i < n; i++) {
    if(!Expr::equal((*this)[i], a[i])) {
      return false;
    }
  }
  return true;
}

bool FactorArray::equalMinus(const FactorArray &a) const {
  const size_t n = size();
  if(a.size() != n) return false;
  int signShiftCount = 0;
  for(size_t i = 0; i < n; i++) {
    if(Expr::equal((*this)[i], a[i])) {
      continue;
    }
    if(Expr::equalMinus((*this)[i], a[i])) {
      signShiftCount++;
      continue;
    }
    return false;
  }
  return isOdd(signShiftCount);
}

String FactorArray::toString() const {
  return _T("PRODUCT") + __super::toStringPointerType();
}

}; // namespace Expr
