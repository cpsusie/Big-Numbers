#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

static int compareFactors(const SNode &n1, const SNode &n2) {
  return n1.node()->compare(n2.node());
}

void FactorArray::sort() {
  __super::sort(compareFactors);
  SETDEBUGSTRING();
}

void FactorArray::add(SNode n) {
  CHECKNODETYPE(n,NT_FACTOR);
  __super::add(n);
  SETDEBUGSTRING();
}

void FactorArray::addAll(const FactorArray &src) {
  DISABLEDEBUGSTRING(*this);
  __super::addAll(src);
  ENABLEDEBUGSTRING(*this);
}

void FactorArray::addAutoConvert(SNode n) {
  switch(n.getNodeType()) {
  case NT_FACTOR : add(n);                     break;
  case NT_PRODUCT: addAll(n.getFactorArray()); break;
  default        : add(factorExp(n,1));        break;
  }
}

FactorArray FactorArray::selectConstantPositiveExponentFactors() const {
  FactorArray result(getTree());
  DISABLEDEBUGSTRING(result);
  for(size_t i = 0; i < size(); i++) {
    const SNode &f = (*this)[i];
    if(f.exponent().isPositiveNumber()) {
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
    const SNode &f = (*this)[i];
    if(f.exponent().isNegativeNumber()) {
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
    const SNode &f = (*this)[i];
    if(!f.exponent().isNumber()) {
      result.add(f);
    }
  }
  ENABLEDEBUGSTRING(result);
  return result;
}

int FactorArray::findFactorWithChangeableSign() const {
  const size_t n = size();
  for(UINT i = 0; i < n; i++) {
    if((*this)[i].isConstant()) {
      return i;
    }
  }
  for(UINT i = 0; i < n; i++) {
    const SNode &f = (*this)[i];
    if(f.hasOddExponent() && f.exponent().isPositiveNumber()) {
      return i;
    }
  }
  for(UINT i = 0; i < n; i++) {
    if((*this)[i].hasOddExponent()) {
      return i;
    }
  }
  return -1;
}

bool FactorArray::equal(const FactorArray &a) const {
  const size_t n = size();
  if(a.size() != n) return false;
  int signShiftCount = 0;
  for(size_t i = 0; i < n; i++) {
    if(Expr::equal((*this)[i].node(), a[i].node())) {
      continue;
    }
    if(Expr::equalMinus((*this)[i].node(), a[i].node())) {
      signShiftCount++;
      continue;
    }
    return false;
  }
  return isEven(signShiftCount);
}

bool FactorArray::equalMinus(const FactorArray &a) const {
  const size_t n = size();
  if(a.size() != n) return false;
  int signShiftCount = 0;
  for(size_t i = 0; i < n; i++) {
    if(Expr::equal((*this)[i].node(), a[i].node())) {
      continue;
    }
    if(Expr::equalMinus((*this)[i].node(), a[i].node())) {
      signShiftCount++;
      continue;
    }
    return false;
  }
  return isOdd(signShiftCount);
}

}; // namespace Expr
