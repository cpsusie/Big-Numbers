#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionNode.h>

namespace Expr {

ExpressionNodeSum::ExpressionNodeSum(ParserTree *tree, const AddentArray &addentArray)
: ExpressionNodeTree(tree, SUM, addentArray) {

#ifdef _DEBUG
  validateNodeArray(addentArray);
#endif // _DEBUG

  getAddentArray().sort();
  SETDEBUGSTRING();
}

void ExpressionNodeSum::validateNodeArray(const AddentArray &a) const {
  const size_t sz = a.size();
  for(size_t i = 0; i < sz; i++) {
    const SNode &n = a[i];
    CHECKNODETYPE(n,NT_ADDENT);
  }
}

ExpressionNodeSum::ExpressionNodeSum(ParserTree *tree, const ExpressionNodeSum *src)
: ExpressionNodeTree(tree, src)
{
}

ExpressionNode *ExpressionNodeSum::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodeSum(tree, this); TRACE_NEW(n);
  return n;
}

Real ExpressionNodeSum::evaluateReal() const {
  Real sum = 0;
  const AddentArray &a = getAddentArray();
  for(size_t i = 0; i < a.size(); i++) {
    sum += a[i].evaluateReal();
  }
  return sum;
}

bool ExpressionNodeSum::equalMinus(const ExpressionNode *n) const {
  return getAddentArray().equalMinus(n->getAddentArray());
}

String ExpressionNodeSum::toString() const {
  return _T("SUM") + getAddentArray().toString();
}

}; // namespace Expr
