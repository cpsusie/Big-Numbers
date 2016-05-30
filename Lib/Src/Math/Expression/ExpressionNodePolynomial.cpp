#include "pch.h"
#include <Math/Expression/ParserTree.h>

ExpressionNodePoly::ExpressionNodePoly(const ParserTree *tree, va_list argptr) : ExpressionNode(tree, POLY) {
  m_coefficientArray = getExpressionList(va_arg(argptr, ExpressionNode*));
  m_argument         = va_arg(argptr, ExpressionNode*);
  INITEXPRESSIONNODEDEBUGSTRING();
}

ExpressionNodePoly::ExpressionNodePoly(const ParserTree *tree, const ExpressionNodeArray &coefficientArray, const ExpressionNode *argument) : ExpressionNode(tree, POLY) {
  m_coefficientArray.setCapacity(coefficientArray.size());
  for(int i = 0; i < coefficientArray.size(); i++) {
    m_coefficientArray.add((ExpressionNode*)coefficientArray[i]);
  }
  m_argument = argument;
  INITEXPRESSIONNODEDEBUGSTRING();
}

ExpressionNodePoly::ExpressionNodePoly(const ParserTree *tree, const ExpressionNodePoly *src) : ExpressionNode(tree, POLY) {
  const ExpressionNodeArray &sa = src->getCoefficientArray();
  m_coefficientArray.setCapacity(sa.size());
  for(int i = 0; i < sa.size(); i++) {
    m_coefficientArray.add(sa[i]->clone(tree));
  }
  m_argument = src->getArgument()->clone(tree);
  INITEXPRESSIONNODEDEBUGSTRING();
}

const ExpressionNode *ExpressionNodePoly::expand() const {
  DEFINEMETHODNAME(expand);

  const ParserTree           *tree             = getTree();
  const ExpressionNodeArray  &coefficientArray = getCoefficientArray();
  const SNode                 arg              = getArgument();
  int                         expo             = getDegree();
  SNode                       result           = tree->_0();

  for(int i = 0; i < coefficientArray.size(); i++) {
    SNode coef  = coefficientArray[i];
    if(!coef.isZero()) {
      result += coef * pow(arg, SNode(tree, expo));
    }
    expo--;
  }
  return result;
}

int ExpressionNodePoly::compare(const ExpressionNode *n) const {
  if(n->getNodeType() != getNodeType()) {
    return ExpressionNode::compare(n);
  }
  int c = getDegree() - n->getDegree();
  if(c) return c;
  const ExpressionNodeArray &coef1 = getCoefficientArray();
  const ExpressionNodeArray &coef2 = getCoefficientArray();
  for(int i = 0; i < coef1.size(); i++) {
    c = coef1[i]->compare(coef2[i]);
    if(c) return c;
  }
  return 0;
}

const ExpressionNode *ExpressionNodePoly::clone(const ParserTree *tree) const {
  return new ExpressionNodePoly(tree, this);
}

bool ExpressionNodePoly::isConstant() const {
  return m_argument->isConstant() && m_coefficientArray.isConstant();
}

bool ExpressionNodePoly::traverseExpression(ExpressionNodeHandler &handler, int level) const {
  if(!handler.handleNode(this, level)) return false;

  const ExpressionNodeArray &coef = getCoefficientArray();
  level++;
  for(int i = 0; i < coef.size(); i++) {
    if(!coef[i]->traverseExpression(handler, level)) return false;
  }
  return getArgument()->traverseExpression(handler, level);
}

void ExpressionNodePoly::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += format(_T("%s\n"), getSymbolName().cstr());
  for(int i = 0; i < m_coefficientArray.size(); i++) {
    m_coefficientArray[i]->dumpNode(s, level+1);
  }
  m_argument->dumpNode(s, level+1);
}

String ExpressionNodePoly::toString() const {
  return _T("poly[") + m_coefficientArray.toString() + _T("](") + m_argument->toString() + _T(")");
}

#ifdef _DEBUG

void ExpressionNodePoly::initDebugString() {
  m_debugString = _T("poly[") + m_coefficientArray.getDebugString() + _T("](") + m_argument->getDebugString() + _T(")");
}

#endif

