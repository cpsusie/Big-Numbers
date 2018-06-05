#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

static int compareFactors(ExpressionFactor * const &f1, ExpressionFactor * const &f2) {
  return f1->compare(f2);
}

void FactorArray::sort() {
  __super::sort(compareFactors);
  SETDEBUGSTRING();
}

void FactorArray::clear(intptr_t capacity) {
  __super::clear(capacity);
  SETDEBUGSTRING();
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

void FactorArray::add(SNode n) {
  CHECKNODETYPE(n,NT_FACTOR);
  __super::add((ExpressionFactor*)n.node());
  SETDEBUGSTRING();
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
