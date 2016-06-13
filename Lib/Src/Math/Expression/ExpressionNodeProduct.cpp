#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionFactor.h>

static int compareFactors(const ExpressionFactor * const &f1, const ExpressionFactor * const &f2) {
  return f1->compare(f2);
}

ExpressionNodeProduct::ExpressionNodeProduct(const ParserTree *tree, const FactorArray &factors) : ExpressionNode(tree, PRODUCT) {
  m_factors = factors;
  m_factors.sort(compareFactors);
  INITEXPRESSIONNODEDEBUGSTRING();
}

int ExpressionNodeProduct::compare(const ExpressionNode *n) const {
  if(n->getNodeType() != EXPRESSIONNODEPRODUCT) {
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

const ExpressionNode *ExpressionNodeProduct::clone(const ParserTree *tree) const {
  FactorArray factors(m_factors.size());
  const bool oldEnable = FactorArray::enableDebugString(false);
  for(size_t i = 0; i < m_factors.size(); i++) {
    factors.add(m_factors[i]->clone(tree));
  }
  FactorArray::enableDebugString(oldEnable); INITFACTORARRAYEBUGSTRING(&factors);

  return new ExpressionNodeProduct(tree, factors);
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

bool ExpressionNodeProduct::traverseExpression(ExpressionNodeHandler &handler, int level) const {
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
  if(m_factors.size() == 0) {
    return _T("1");
  } else {
    String result = m_factors[0]->parenthesizedExpressionToString(this);
    for(size_t i = 1; i < m_factors.size(); i++) {
      result += _T("*");
      result += m_factors[i]->parenthesizedExpressionToString(this);
    }
    return result;
  }

/*
  String result;
  FactorArray p = m_factors.selectConstantPositiveExponentFactors();
  p.addAll(m_factors.selectNonConstantExponentFactors());
  FactorArray q = m_factors.selectConstantNegativeExponentFactors();

  String op;

  if(p.size() > 0) {
    for(int i = 0; i < p.size(); i++) {
      const ExpressionFactor *f = p[i];
      if(op.length() == 0) {
        op = _T("*");
      } else {
        result += op;
      }
      if(f->getExponent()->isOne()) {
        result += f->getBase()->parenthesizedExpressionToString(this);
      } else {
        result += f->parenthesizedExpressionToString(this);
      }
    }
  }

  if(q.size() > 0) {
    if(op.length() == 0) {
      result += _T("1");
    }
    op = _T("/");

    for(int i  = 0; i < q.size(); i++) {
      const ExpressionFactor *f = q[i];
      result += op;
      if(f->getExponent()->isMinusOne()) {
        result += f->getBase()->parenthesizedExpressionToString(this);
      } else {
        const ParserTree *tree = f->getTree();
        result += tree->power(f->getBase(), tree->minus(f->getExponent()))->parenthesizedExpressionToString(this);
      }
    }
  }
  return result;
*/
}

#ifdef _DEBUG
void ExpressionNodeProduct::initDebugString() {
  m_debugString = m_factors.getDebugString();
}
#endif

