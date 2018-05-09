#include "pch.h"
#include <Math/Expression/ParserTree.h>

ExpressionNodePoly::ExpressionNodePoly(ParserTree *tree, va_list argptr) : ExpressionNode(tree, POLY) {
  m_coefArray = getExpressionList(va_arg(argptr, ExpressionNode*));
  m_arg       = va_arg(argptr, ExpressionNode*);
  m_firstCoefIndex   = -1;
}

ExpressionNodePoly::ExpressionNodePoly(ParserTree *tree, const ExpressionNodeArray &coefArray, ExpressionNode *arg) : ExpressionNode(tree, POLY) {
  m_coefArray.setCapacity(coefArray.size());
  for(size_t i = 0; i < coefArray.size(); i++) {
    m_coefArray.add(coefArray[i]);
  }
  m_arg = arg;
  m_firstCoefIndex   = -1;
}

ExpressionNodePoly::ExpressionNodePoly(ParserTree *tree, const ExpressionNodePoly *src) : ExpressionNode(tree, POLY) {
  const ExpressionNodeArray &sa = src->getCoefficientArray();
  m_coefArray.setCapacity(sa.size());
  for(size_t i = 0; i < sa.size(); i++) {
    m_coefArray.add(sa[i]->clone(tree));
  }
  m_arg            = src->getArgument()->clone(tree);
  m_firstCoefIndex = -1;
}

ExpressionNode *ExpressionNodePoly::expand() {
  DEFINEMETHODNAME;

  ParserTree                 *tree             = getTree();
  const ExpressionNodeArray  &coefficientArray = getCoefficientArray();
  const SNode                 arg              = getArgument();
  int                         expo             = getDegree();
  SNode                       result           = tree->getZero();

  for(size_t i = 0; i < coefficientArray.size(); i++) {
    SNode coef  = coefficientArray[i];
    if(!coef.isZero()) {
      result += coef * pow(arg, SNode(tree, expo));
    }
    expo--;
  }
  return result;
}

int ExpressionNodePoly::compare(ExpressionNode *n) {
  if(n->getNodeType() != getNodeType()) {
    return ExpressionNode::compare(n);
  }
  int c = getDegree() - n->getDegree();
  if(c) return c;
  const ExpressionNodeArray &coef1 = getCoefficientArray();
  const ExpressionNodeArray &coef2 = getCoefficientArray();
  for(size_t i = 0; i < coef1.size(); i++) {
    c = coef1[i]->compare(coef2[i]);
    if(c) return c;
  }
  return 0;
}

ExpressionNode *ExpressionNodePoly::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodePoly(tree, this); TRACE_NEW(n);
  return n;
}

bool ExpressionNodePoly::isConstant() const {
  return m_arg->isConstant() && m_coefArray.isConstant();
}

static void getCoefficients(CompactArray<Real> &dst, const ExpressionNodeArray &coefficientArray) {
  dst.setCapacity(coefficientArray.size());
  for(size_t i = 0; i < coefficientArray.size(); i++) {
    dst.add(coefficientArray[i]->evaluateReal());
  }
}
/**
 * n.symbol = POLY, n.getCoefficientArray = coefficients. coef[0..n] poly(x) = ((coef[0] * x) + coef[1]) * x)... ) + coef[n]
 * return value of polynomial p(x) with coefficients contained in n.getCoefficientArray, and x in n.getArgument()
 */
Real ExpressionNodePoly::evaluateReal() const {
  const Real         arg = getArgument()->evaluateReal();
  CompactArray<Real> coefArray;
  getCoefficients(coefArray, getCoefficientArray());
  Real result = 0;
  for(size_t i = 0; i < coefArray.size(); i++) {
    result = result * arg + coefArray[i];
  }
  return result;
}

bool ExpressionNodePoly::traverseExpression(ExpressionNodeHandler &handler, int level) {
  if(!handler.handleNode(this, level)) return false;

  ExpressionNodeArray &coef = getCoefficientArray();
  level++;
  for(size_t i = 0; i < coef.size(); i++) {
    if(!coef[i]->traverseExpression(handler, level)) return false;
  }
  return getArgument()->traverseExpression(handler, level);
}

void ExpressionNodePoly::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += format(_T("%s first coefIndex:%d\n"), getSymbolName().cstr(), getFirstCoefIndex());
  for(size_t i = 0; i < m_coefArray.size(); i++) {
    m_coefArray[i]->dumpNode(s, level+1);
  }
  m_arg->dumpNode(s, level+1);
}

String ExpressionNodePoly::toString() const {
  return _T("poly[") + m_coefArray.toString() + _T("](") + m_arg->toString() + _T(")");
}
