#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionNode.h>

namespace Expr {

ExpressionNodeProduct::ExpressionNodeProduct(ParserTree *tree, const FactorArray &factors)
: ExpressionNodeTree(tree, PRODUCT, factors)
{
#ifdef _DEBUG
  validateFactorArray(getFactorArray());
#endif
  getFactorArray().sort();
  SETDEBUGSTRING();
}

void ExpressionNodeProduct::validateFactorArray(const FactorArray &factors) const {
  const size_t sz = factors.size();
  for(size_t i = 0; i < sz; i++) {
    const SNode &n = factors[i];
    CHECKNODETYPE(n,NT_POWER);
  }
}

ExpressionNodeProduct::ExpressionNodeProduct(ParserTree *tree, const ExpressionNodeProduct *src)
: ExpressionNodeTree(tree, src)
{
}

ExpressionNode *ExpressionNodeProduct::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodeProduct(tree, this); TRACE_NEW(n);
  return n;
}

Real ExpressionNodeProduct::evaluateReal() const {
  Real prod = 1;
  const FactorArray &a = getFactorArray();
  const size_t       n = a.size();
  for(size_t i = 0; i < n; i++) {
    prod *= a[i].evaluateReal();
  }
  return prod;
}

int ExpressionNodeProduct::compare(const ExpressionNode *n) const {
  if(n->getNodeType() != getNodeType()) {
    return ExpressionNode::compare(n);
  }
  const FactorArray &a            = getFactorArray();
  const FactorArray &na           = n->getFactorArray();
  const int          count        = (int)a.size();
  int                c            = count - (int)na.size();
  if(c) return -c; // put the product with highest number of factors first
  int                constCompare = 0;
  for(int i = 0; i < count; i++) {
    ExpressionNode *ai = a[i].node(), *nai = na[i].node();
    c = ai->compare(nai);
    if(c) {
      const bool aiConst = ai->isConstant(), naiConst = nai->isConstant();
      if(aiConst && naiConst) {
        if(constCompare == 0) {
          constCompare = c;
        }
      } else {
        return ordinal(aiConst) - ordinal(naiConst); // products with most non-const factors first
      }
    }
  }
  return constCompare;
}

bool ExpressionNodeProduct::equal(const ExpressionNode *n) const {
  return getFactorArray().equal(n->getFactorArray());
}

bool ExpressionNodeProduct::equalMinus(const ExpressionNode *n) const {
  return getFactorArray().equalMinus(n->getFactorArray());
}

String ExpressionNodeProduct::toString() const {
  return _T("PROD") + getFactorArray().toString();
}

}; // namespace Expr
