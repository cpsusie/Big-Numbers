#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

ExpressionNodePoly::ExpressionNodePoly(ParserTree *tree, const SNodeArray &coefArray, SNode arg)
  : ExpressionNode(tree, POLY)
  , m_coefArray(coefArray)
  , m_arg(arg)
{
  m_firstCoefIndex   = -1;
  SETDEBUGSTRING();
}

ExpressionNodePoly::ExpressionNodePoly(ParserTree *tree, const ExpressionNodePoly *src)
: ExpressionNode(tree, POLY)
, m_coefArray(*tree)
{
  src->getCoefArray().cloneNodes(m_coefArray, tree);
  m_arg            = src->getArgument().node()->clone(tree);
  m_firstCoefIndex = -1;
  SETDEBUGSTRING();
}

bool ExpressionNodePoly::isCoefArrayConstant() const {
  if(!m_info.m_coefArrayChecked) {
    m_info.m_coefArrayConstant = getCoefArray().isConstant() ? 1 : 0;
    m_info.m_coefArrayChecked          = 1;
  }
  return m_info.m_coefArrayConstant ? true : false;
}

bool ExpressionNodePoly::isSymmetricFunction() const {
  if(!isCoefArrayConstant()) return false;
  const SNodeArray &coefArray = getCoefArray();
  const size_t      n         = coefArray.size();
  for(size_t i = 0; i < n; i++) {
    const Real c = coefArray[i].evaluateReal();
    if(::isOdd(i) && (c != 0)) {
      return false;
    }
  }
  return true;
}

bool ExpressionNodePoly::isAsymmetricFunction() const {
  if(!isCoefArrayConstant()) return false;
  const SNodeArray &coefArray = getCoefArray();
  const size_t      n         = coefArray.size();
  for(size_t i = 0; i < n; i++) {
    const Real c = coefArray[i].evaluateReal();
    if(::isEven(i) && (c != 0)) {
      return false;
    }
  }
  return true;
}

bool ExpressionNodePoly::equal(const ExpressionNode *poly) const { // assume poly.symbol == POLY
  if(Expr::equal(getArgument().node(), poly->getArgument().node())) {
    return getCoefArray().equal(poly->getCoefArray());
  }
  if(Expr::equalMinus(getArgument().node(), poly->getArgument().node())) {
    if(isSymmetricFunction()) {
      return getCoefArray().equal(poly->getCoefArray());
    } else if(isAsymmetricFunction()) {
      return getCoefArray().equalMinus(poly->getCoefArray());
    }
  }
  return false;
}

bool ExpressionNodePoly::equalMinus(const ExpressionNode *poly) const { // assume poly.symbol == POLY
  if(Expr::equal(getArgument().node(), poly->getArgument().node())) {
    return getCoefArray().equalMinus(poly->getCoefArray());
  }
  if(Expr::equalMinus(getArgument().node(), poly->getArgument().node())) {
    if(isSymmetricFunction()) {
      return getCoefArray().equalMinus(poly->getCoefArray());
    } else if(isAsymmetricFunction()) {
      return getCoefArray().equal(poly->getCoefArray());
    }
  }
  return false;
}

SNode ExpressionNodePoly::expand() const {
  ParserTree        &tree      = getTree();
  const SNodeArray  &coefArray = getCoefArray();
  const SNode        arg       = getArgument();
  int                expo      = getDegree();
  SNode              result    = arg._0();

  for(size_t i = 0; i < coefArray.size(); i++) {
    SNode coef  = coefArray[i];
    if(!coef.isZero()) {
      result += coef * pow(arg, SNode(tree, expo));
    }
    expo--;
  }
  return result;
}

int ExpressionNodePoly::compare(const ExpressionNode *n) const {
  if(n->getNodeType() != getNodeType()) {
    return __super::compare(n);
  }
  int c = getDegree() - n->getDegree();
  if(c) return c;
  const SNodeArray &coefArray1 = getCoefArray();
  const SNodeArray &coefArray2 = getCoefArray();
  const size_t      sz    = coefArray1.size();
  for(size_t i = 0; i < sz; i++) {
    c = coefArray1[i].node()->compare(coefArray2[i].node());
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

  const SNodeArray &coefArray = getCoefArray();
  level++;
  for(size_t i = 0; i < coefArray.size(); i++) {
    if(!coefArray[i].node()->traverseExpression(handler, level)) return false;
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
