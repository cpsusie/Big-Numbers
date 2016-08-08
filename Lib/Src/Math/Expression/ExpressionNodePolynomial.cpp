#include "pch.h"
#include <Math/Expression/ParserTree.h>

ExpressionNodePoly::ExpressionNodePoly(ParserTree *tree, va_list argptr) : ExpressionNode(tree, POLY) {
  m_coefficientArray = getExpressionList(va_arg(argptr, ExpressionNode*));
  m_argument         = va_arg(argptr, ExpressionNode*);
  m_firstCoefIndex   = -1;
}

ExpressionNodePoly::ExpressionNodePoly(ParserTree *tree, const ExpressionNodeArray &coefficientArray, ExpressionNode *argument) : ExpressionNode(tree, POLY) {
  m_coefficientArray.setCapacity(coefficientArray.size());
  for(size_t i = 0; i < coefficientArray.size(); i++) {
    m_coefficientArray.add(coefficientArray[i]);
  }
  m_argument = argument;
  m_firstCoefIndex   = -1;
}

ExpressionNodePoly::ExpressionNodePoly(ParserTree *tree, const ExpressionNodePoly *src) : ExpressionNode(tree, POLY) {
  const ExpressionNodeArray &sa = src->getCoefficientArray();
  m_coefficientArray.setCapacity(sa.size());
  for(size_t i = 0; i < sa.size(); i++) {
    m_coefficientArray.add(sa[i]->clone(tree));
  }
  m_argument = src->getArgument()->clone(tree);
  m_firstCoefIndex   = -1;
}

ExpressionNode *ExpressionNodePoly::expand() {
  DEFINEMETHODNAME;

  ParserTree                 *tree             = getTree();
  const ExpressionNodeArray  &coefficientArray = getCoefficientArray();
  const SNode                 arg              = getArgument();
  int                         expo             = getDegree();
  SNode                       result           = tree->_0();

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
  return new ExpressionNodePoly(tree, this);
}

bool ExpressionNodePoly::isConstant() const {
  return m_argument->isConstant() && m_coefficientArray.isConstant();
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
  addLeftMargin(s, level) += format(_T("%s\n"), getSymbolName().cstr());
  for(size_t i = 0; i < m_coefficientArray.size(); i++) {
    m_coefficientArray[i]->dumpNode(s, level+1);
  }
  m_argument->dumpNode(s, level+1);
}

String ExpressionNodePoly::toString() const {
  return _T("poly[") + m_coefficientArray.toString() + _T("](") + m_argument->toString() + _T(")");
}
