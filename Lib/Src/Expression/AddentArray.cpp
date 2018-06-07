#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

void AddentArray::add(SNode n) {
  CHECKNODETYPE(n,NT_ADDENT);
  __super::add(n);
}

void AddentArray::addAll(const AddentArray &src) {
  DISABLEDEBUGSTRING(*this);
  __super::addAll(src);
  ENABLEDEBUGSTRING(*this);
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

}; // namespace Expr
