#include "pch.h"
#include <Math/Expression/ExpressionNode.h>
#include <Math/Expression/SumElement.h>

namespace Expr {

void AddentArray::add(SNode n, bool positive) {
  DISABLEDEBUGSTRING(*this);
  switch(n.getSymbol()) {
  case NUMBER:
    if(!n.isZero()) {      // dont add Zero
      SumElement *e = new SumElement(n, positive); TRACE_NEW(e);
      add(e);
    }
    break;
  case SUM   :
    if(positive) {
      addAll(n.getAddentArray());
    } else {
      const AddentArray &a = n.getAddentArray();
      for(size_t i = 0; i < a.size(); i++) {
        SumElement *e = a[i];
        add(e->getNode(), !e->isPositive());
      }
    }
    break;
  default    :
    { SumElement *e = new SumElement(n, positive); TRACE_NEW(e);
      add(e);
    }
    break;
  }
  ENABLEDEBUGSTRING(*this);
}

bool AddentArray::equal(const AddentArray &a) const {
  const size_t n = size();
  if(a.size() != n) return false;
  for(size_t i = 0; i < n; i++) {
    if(!(*this)[i]->equal(a[i])) {
      return false;
    }
  }
  return true;
}

bool AddentArray::equalMinus(const AddentArray &a) const {
  const size_t n = size();
  if(a.size() != n) return false;
  for(size_t i = 0; i < n; i++) {
    if(!(*this)[i]->equalMinus(a[i])) {
      return false;
    }
  }
  return true;
}

static int compareSumElement(SumElement * const &e1, SumElement * const &e2) {
  return e1->compare(e2);
}

AddentArray &AddentArray::sort() {
  __super::sort(compareSumElement);
  SETDEBUGSTRING();
  return *this;
}

static int compare2(SumElement * const &e1, SumElement * const &e2) {
  const bool p1 = e1->isPositive();
  const bool p2 = e2->isPositive();
  int c = p2 - p1;
  if(c) return c;
  return e1->compare(e2);
}

static int compareMany(SumElement * const &e1, SumElement * const &e2) {
  return e1->compare(e2);
}

AddentArray &AddentArray::sortStdForm() {
  if(size() == 2) {
    __super::sort(compare2);
  } else {
    __super::sort(compareMany);
  }
  SETDEBUGSTRING();
  return *this;
}

String AddentArray::toString() const {
  return _T("SUM") + __super::toStringPointerType();
}

}; // namespace Expr
