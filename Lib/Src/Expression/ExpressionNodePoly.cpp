#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

ExpressionNodePoly::ExpressionNodePoly(ParserTree *tree, const SNodeArray &coefArray, SNode arg)
  : ExpressionNode(tree, POLY)
  , m_coefArray(coefArray)
  , m_arg(arg)
{
  m_firstCoefIndex   = -1;
}

ExpressionNodePoly::ExpressionNodePoly(ParserTree *tree, const ExpressionNodePoly *src) : ExpressionNode(tree, POLY) {
  src->getCoefArray().cloneNodes(m_coefArray, tree);
  m_arg            = src->getArgument().node()->clone(tree);
  m_firstCoefIndex = -1;
}

ExpressionNode *ExpressionNodePoly::expand() {
  DEFINEMETHODNAME;

  ParserTree        &tree             = getTree();
  const SNodeArray  &coefficientArray = getCoefArray();
  const SNode        arg              = getArgument();
  int                expo             = getDegree();
  SNode              result           = tree.getZero();

  for(size_t i = 0; i < coefficientArray.size(); i++) {
    SNode coef  = coefficientArray[i];
    if(!coef.isZero()) {
      result += coef * pow(arg, SNode(tree, expo));
    }
    expo--;
  }
  return result.node();
}

int ExpressionNodePoly::compare(ExpressionNode *n) {
  if(n->getNodeType() != getNodeType()) {
    return ExpressionNode::compare(n);
  }
  int c = getDegree() - n->getDegree();
  if(c) return c;
  const SNodeArray &coef1 = getCoefArray();
  const SNodeArray &coef2 = getCoefArray();
  const size_t      sz    = coef1.size();
  for(size_t i = 0; i < sz; i++) {
    c = coef1[i].node()->compare(coef2[i].node());
    if(c) return c;
  }
  return 0;
}

ExpressionNode *ExpressionNodePoly::clone(ParserTree *tree) const {
  ExpressionNode *n = new ExpressionNodePoly(tree, this); TRACE_NEW(n);
  return n;
}

bool ExpressionNodePoly::isConstant() const {
  return m_arg.isConstant() && m_coefArray.isConstant();
}

static void getCoefficients(CompactArray<Real> &dst, const SNodeArray &coefArray) {
  dst.setCapacity(coefArray.size());
  for(size_t i = 0; i < coefArray.size(); i++) {
    dst.add(coefArray[i].evaluateReal());
  }
}
/**
 * n.symbol = POLY, n.getCoefArray = coefficients. coef[0..n] poly(x) = ((coef[0] * x) + coef[1]) * x)... ) + coef[n]
 * return value of polynomial p(x) with coefficients contained in n.getCoefArray, and x in n.getArgument()
 */
Real ExpressionNodePoly::evaluateReal() const {
  const Real         arg = getArgument().evaluateReal();
  CompactArray<Real> coefArray;
  getCoefficients(coefArray, getCoefArray());
  Real result = 0;
  for(size_t i = 0; i < coefArray.size(); i++) {
    result = result * arg + coefArray[i];
  }
  return result;
}

bool ExpressionNodePoly::traverseExpression(ExpressionNodeHandler &handler, int level) {
  if(!handler.handleNode(this, level)) return false;

  const SNodeArray &coef = getCoefArray();
  level++;
  for(size_t i = 0; i < coef.size(); i++) {
    if(!coef[i].node()->traverseExpression(handler, level)) return false;
  }
  return getArgument().node()->traverseExpression(handler, level);
}

void ExpressionNodePoly::dumpNode(String &s, int level) const {
  addLeftMargin(s, level) += format(_T("%s first coefIndex:%d\n"), getSymbolName().cstr(), getFirstCoefIndex());
  for(size_t i = 0; i < m_coefArray.size(); i++) {
    m_coefArray[i].node()->dumpNode(s, level+1);
  }
  m_arg.node()->dumpNode(s, level+1);
}

String ExpressionNodePoly::toString() const {
  return _T("poly[") + m_coefArray.toString() + _T("](") + m_arg.toString() + _T(")");
}

}; // namespace Expr
