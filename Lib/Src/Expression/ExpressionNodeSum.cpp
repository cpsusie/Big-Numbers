#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

ExpressionNodeSum::ExpressionNodeSum(ParserTree *tree, const SNodeArray &childArray)
: ExpressionNodeTree(tree, SUM, childArray) {

#ifdef _DEBUG
  validateNodeArray(childArray);
#endif // _DEBUG

  sort(getChildArray());
  SETDEBUGSTRING();
}

void ExpressionNodeSum::validateNodeArray(const SNodeArray &a) const {
  const size_t sz = a.size();
  for(size_t i = 0; i < sz; i++) {
    const SNode &n = a[i];
    if(n.getNodeType() != NT_ADDENT) {
      throwInvalidArgumentException(__TFUNCTION__
                                   ,_T("node[%zu] not type NT_ADDENT (=%s)")
                                   ,i, n.getNodeTypeName().cstr());
    }
  }
}

ExpressionNodeSum::ExpressionNodeSum(ParserTree *tree, const ExpressionNodeSum *src)
: ExpressionNodeTree(tree, src)
{
  SETDEBUGSTRING();
}

ExpressionNode *ExpressionNodeSum::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodeSum(tree, this); TRACE_NEW(n);
  return n;
}

Real ExpressionNodeSum::evaluateReal() const {
  Real sum = 0;
  const SNodeArray &a = getChildArray();
  for(size_t i = 0; i < a.size(); i++) {
    sum += a[i].evaluateReal();
  }
  return sum;
}

static int compareMany(const SNode &e1, const SNode &e2) {
  return e1.node()->compare(e2.node());
}

void ExpressionNodeSum::sort(SNodeArray &a) { // static
  a.sort(compareMany);
}

static int compare2(const SNode &e1, const SNode &e2) {
  const bool p1 = e1.isPositive();
  const bool p2 = e2.isPositive();
  int c = ordinal(p2) - ordinal(p1);
  if(c) return c;
  return compareMany(e1,e2);
}

void ExpressionNodeSum::sortStdForm(SNodeArray &a) { // static
  if(a.size() == 2) {
    a.sort(compare2);
  } else {
    a.sort(compareMany);
  }
}

String ExpressionNodeSum::toString() const {
  return _T("SUM") + getChildArray().toString();
}

}; // namespace Expr
