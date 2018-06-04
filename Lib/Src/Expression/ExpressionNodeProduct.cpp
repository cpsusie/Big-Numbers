#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

ExpressionNodeProduct::ExpressionNodeProduct(ParserTree *tree, FactorArray &factors)
: ExpressionNode(tree, PRODUCT)
, m_factors(*tree, factors.size())
{
#ifdef _DEBUG
  validateFactorArray(factors);
#endif
  m_factors.addAll(factors);
  m_factors.sort();
  SETDEBUGSTRING();
}

void ExpressionNodeProduct::validateFactorArray(const FactorArray &factors) const {
  const size_t sz = factors.size();
  for(size_t i = 0; i < sz; i++) {
    const ExpressionNode *n = factors[i];
    if(n->getNodeType() != NT_FACTOR) {
      throwInvalidArgumentException(__TFUNCTION__
                                   ,_T("node[%zu] not type NT_FACTOR (=%s)")
                                   ,i, n->getNodeTypeName().cstr());
    }
  }
}

int ExpressionNodeProduct::compare(ExpressionNode *n) {
  if(n->getNodeType() != NT_PRODUCT) {
    return ExpressionNode::compare(n);
  }
  const FactorArray &na    = n->getFactorArray();
  const int          count = (int)min(m_factors.size(), na.size());
  int                i,constCompare = 0;
  for(i = 0; i < count; i++) {
    const int c = m_factors[i]->compare(na[i]);
    if(c) {
      if(na[i]->isConstant() && m_factors[i]->isConstant()) {
        if(constCompare == 0) {
          constCompare = c;
        }
        continue;
      }
      return c;
    }
  }
  if(i == m_factors.size()) {
    return (i == na.size()) ? constCompare : 1;
  } else {
    return -1;
  }
}

ExpressionNode *ExpressionNodeProduct::clone(ParserTree *tree) const {
  FactorArray factors(*tree, m_factors.size());
  for(size_t i = 0; i < m_factors.size(); i++) {
    factors.add(m_factors[i]->clone(tree));
  }
  return tree->productExpr(factors);
}

bool ExpressionNodeProduct::isConstant() const {
  const int n = (int)m_factors.size();
  for(int i = 0; i < n; i++) {
    if(!m_factors[i]->isConstant()) {
      return false;
    }
  }
  return true;
}

Real ExpressionNodeProduct::evaluateReal() const {
  Real prod = 1;
  const FactorArray &factors = m_factors;
  for(size_t i = 0; i < factors.size(); i++) {
    prod *= factors[i]->evaluateReal();
  }
  return prod;
}

bool ExpressionNodeProduct::traverseExpression(ExpressionNodeHandler &handler, int level) {
  if(!handler.handleNode(this, level)) return false;
  const FactorArray &a = getFactorArray();
  level++;
  for(size_t i = 0; i < a.size(); i++) {
    if(!a[i]->traverseExpression(handler, level)) return false;
  }
  return true;
}

void ExpressionNodeProduct::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += format(_T("%s\n"), getSymbolName().cstr());
  for(size_t i = 0; i < m_factors.size(); i++) {
    m_factors[i]->dumpNode(s, level+1);
  }
}

String ExpressionNodeProduct::toString() const {
  return m_factors.toString();
}

}; // namespace Expr
