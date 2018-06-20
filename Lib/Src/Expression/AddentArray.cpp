#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

AddentArray &AddentArray::operator=(const AddentArray &rhs) {
  DISABLEDEBUGSTRING(*this);
  clear(rhs.size());
  __super::addAll(rhs);
  ENABLEDEBUGSTRING(*this);
  return *this;
}

void AddentArray::add(SNode n) {
  CHECKNODETYPE(n,NT_ADDENT);
  __super::add(n);
}

AddentArray &AddentArray::operator+=(const AddentArray &rhs) {
  if(rhs.isEmpty()) return *this;
  __super::addAll(rhs);
  return *this;
}

AddentArray &AddentArray::operator-=(const AddentArray &rhs) {
  if(rhs.isEmpty()) return *this;
  const size_t n = rhs.size();
  DISABLEDEBUGSTRING(*this);
  for(size_t i = n; i < n; i++) {
    SNode e = rhs[i];
    add(-e);
  }
  ENABLEDEBUGSTRING(*this);
  return *this;
}

AddentArray &AddentArray::operator+=(const Rational &rhs) {
  if(rhs == 0) return *this;
  if(rhs > 0) {
    add(addentExp(SNV(rhs),true ));
  } else { // rhs < 0
    add(addentExp(SNV(-rhs),false));
  }
  return *this;
}

AddentArray &AddentArray::operator-=(const Rational &rhs) {
  if(rhs == 0) return *this;
  if(rhs > 0) {
    add(addentExp(SNV(rhs),false));
  } else { // rhs < 0
    add(addentExp(SNV(-rhs),true ));
  }
  return *this;
}

static int compareMany(const SNode &e1, const SNode &e2) {
  return e1.node()->compare(e2.node());
}

void AddentArray::sort() {
  __super::sort(compareMany);
}

static int compare2(const SNode &e1, const SNode &e2) {
  const bool p1 = e1.isPositive();
  const bool p2 = e2.isPositive();
  int c = ordinal(p2) - ordinal(p1);
  if(c) return c;
  return compareMany(e1,e2);
}

void AddentArray::sortStdForm() {
  if(size() == 2) {
    __super::sort(compare2);
  } else {
    __super::sort(compareMany);
  }
}

bool AddentArray::equalMinus(const AddentArray &a) const {
  const size_t n = size();
  if(n != a.size()) {
    return false;
  }
  for(size_t i = 0; i < n; i++) {
    if(!(*this)[i].equalMinus(a[i])) {
      return false;
    }
  }
  return true;
}

}; // namespace Expr
