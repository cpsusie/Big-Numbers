#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

static int compareFactors(const SNode &n1, const SNode &n2) {
  return n1.node()->compare(n2.node());
}

void FactorArray::sort() {
  __super::sort(compareFactors);
}

void FactorArray::add(SNode n) {
  CHECKNODETYPE(n,NT_POWER);
  __super::add(n);
}

void FactorArray::addAll(const FactorArray &src) {
  __super::addAll(src);
}

FactorArray &FactorArray::operator*=(SNode n) {
  switch(n.getNodeType()) {
  case NT_POWER  : add(n);                      break;
  case NT_PRODUCT: *this *= n.getFactorArray(); break;
  default        : add(powerExp(n,1));          break;
  }
  return *this;
}

FactorArray &FactorArray::operator/=(SNode n) {
  DISABLEDEBUGSTRING(*this);
  switch(n.getNodeType()) {
  case NT_POWER  : add(powerExp(n.base(),-n.exponent())); break;
  case NT_PRODUCT: *this /= n.getFactorArray();           break;
  default        : add(powerExp(n,-1));                   break;
  }
  ENABLEDEBUGSTRING(*this);
  return *this;
}

FactorArray &FactorArray::operator*=(const FactorArray &a) {
  addAll(a);
  return *this;
}

FactorArray &FactorArray::operator/=(const FactorArray &a) {
  DISABLEDEBUGSTRING(*this);
  const size_t n = a.size();
  for(size_t i = 0; i < n; i++) {
    *this /= a[i];
  }
  ENABLEDEBUGSTRING(*this);
  return *this;
}

FactorArray &FactorArray::operator*=(const Rational &r) {
  if(r != 1) {
    const INT64 num = r.getNumerator();
    const INT64 den = r.getDenominator();
    if(den == 1) {
      *this *= SNV(num);
    } else if(num == 1) {
      *this /= SNV(den);
    } else {
      *this *= SNV(r);
    }
  }
  return *this;
}

FactorArray &FactorArray::operator/=(const Rational &r) {
  *this *= ::reciprocal(r);
  return *this;
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
