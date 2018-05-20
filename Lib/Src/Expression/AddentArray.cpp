#include "pch.h"
#include <Math/Expression/ExpressionNode.h>
#include <Math/Expression/SumElement.h>

namespace Expr {

void AddentArray::add(SNode n, bool positive) {
  switch(n.getSymbol()) {
  case NUMBER:
    if(!n.isZero()) {      // dont add Zero
      SumElement *e = new SumElement(n.node(), positive); TRACE_NEW(e);
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
    { SumElement *e = new SumElement(n.node(), positive); TRACE_NEW(e);
      add(e);
    }
    break;
  }
}

String AddentArray::toString() const {
  if(size() == 0) {
    return _T("0");
  }
  String result = (*this)[0]->toString();
  for(size_t i = 1; i < size(); i++) {
    const SumElement *e = (*this)[i];
    result += e->isPositive() ? _T("+") : _T("-");
    result += e->getNode()->toString();
  }
  return result;
}

}; // namespace Expr
