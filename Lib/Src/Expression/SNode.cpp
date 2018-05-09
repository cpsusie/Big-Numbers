#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionFactor.h>

SNode SNode::_0()  const { return SNode( getTree()->getZero());     }
SNode SNode::_1()  const { return SNode( getTree()->getOne());      }
SNode SNode::_m1() const { return SNode( getTree()->getMinusOne()); }
SNode SNode::_2()  const { return SNode( getTree()->getTwo());      }
SNode SNode::_10() const { return SNode( getTree()->getTen());      }
SNode SNode::_05() const { return SNode( getTree()->getHalf());     }

SNode::SNode(ParserTree *tree, int v) {
  m_node = tree->numberExpression(v);
}

SNode::SNode(ParserTree *tree, INT64 v) {
  m_node = tree->numberExpression(v);
}

SNode::SNode(ParserTree *tree, const Rational &v) {
  m_node = tree->numberExpression(v);
}

SNode::SNode(ParserTree *tree, const Real &v) {
  m_node = tree->numberExpression(v);
}

SNode::SNode(ParserTree *tree, const Number &v) {
  m_node = tree->numberExpression(v);
}

SNode::SNode(ParserTree *tree, bool v) {
  m_node = v ? tree->getTrue() : tree->getFalse();
}

SNode::SNode(ParserTree *tree, FactorArray &a) {
  m_node = tree->getProduct(a);
}

SNode::SNode(ParserTree *tree, AddentArray &a) {
  m_node = tree->getSum(a);
}

SNode SNode::operator+(const SNode &n) const {
  return getTree()->sum(m_node, n.m_node);
}

SNode SNode::operator-(const SNode &n) const { // binary -
  return getTree()->diff(m_node, n.m_node);
}

SNode SNode::operator-() const { // unary -
  return getTree()->minus(m_node);
}

SNode SNode::operator*(const SNode &n) const {
  return getTree()->prod(m_node, n.m_node);
}

SNode SNode::operator/(const SNode &n) const {
  return getTree()->quot(m_node, n.m_node);
}

SNode &SNode::operator+=(const SNode &n) {
  m_node = *this + n;
  return *this;
}

SNode &SNode::operator-=(const SNode &n) {
  m_node = *this - n;
  return *this;
}

SNode &SNode::operator*=(const SNode &n) {
  m_node = *this * n;
  return *this;
}

SNode &SNode::operator/=(const SNode &n) {
  m_node = *this / n;
  return *this;
}

SNode SNode::operator%(const SNode &n) const {
  return getTree()->mod(m_node, n.m_node);
}

SNode SNode::operator&&(const SNode &n) const {
  return m_node->getTree()->and(m_node, n.m_node);
}

SNode SNode::operator||(const SNode &n) const {
  return m_node->getTree()->or(m_node, n.m_node);
}

SNode SNode::operator!() const {
  return m_node->getTree()->unaryExpr(SYMNOT, m_node);
}

bool SNode::operator==(const SNode &n) const {
  return equal(m_node, n.m_node);
}

bool SNode::reducesToRationalConstant(Rational *r) const {
  return m_node->reducesToRationalConstant(r);
}

bool SNode::reducesToRational(Rational *r) const {
  return m_node->reducesToRational(r);
}

SNode reciprocal(const SNode &x) {
  return x.getTree()->reciprocal(x.m_node);
}

SNode sqrt(const SNode &x) {
  return x.getTree()->sqrt(x.m_node);
}

SNode sqr(const SNode &x) {
  return x.getTree()->sqr(x.m_node);
}

SNode pow(const SNode &x, const SNode &y) {
  return x.getTree()->power(x.m_node, y.m_node);
}

SNode root(const SNode &x, const SNode &y) {
  return x.getTree()->root(x.m_node, y.m_node);
}

SNode exp(const SNode &x) {
  return x.getTree()->exp(x.m_node);
}

SNode exp10(const SNode &x) {
  return x.getTree()->exp10(x.m_node);
}

SNode exp2(const SNode &x) {
  return x.getTree()->exp2(x.m_node);
}

SNode ln(const SNode &x) {
  return unaryExp(LN, x);
}

SNode log10(const SNode &x) {
  return unaryExp(LOG10, x);
}

SNode log2(const SNode &x) {
  return unaryExp(LOG2, x);
}

SNode sin(const SNode &x) {
  return unaryExp(SIN, x);
}

SNode cos(const SNode &x) {
  return unaryExp(COS, x);
}

SNode tan(const SNode &x) {
  return unaryExp(TAN, x);
}

SNode cot(const SNode &x) {
  return x.getTree()->cot(x.m_node);
}

SNode asin(const SNode &x) {
  return unaryExp(ASIN, x);
}

SNode acos(const SNode &x) {
  return unaryExp(ACOS, x);
}

SNode atan(const SNode &x) {
  return unaryExp(ATAN, x);
}

SNode atan2(const SNode &y, const SNode &x) {
  return binExp(ATAN2, y, x);
}

SNode acot(const SNode &x) {
  return unaryExp(ACOT, x);
}

SNode csc(const SNode &x) {
  return x.getTree()->csc(x.m_node);
}

SNode sec(const SNode &x) {
  return x.getTree()->sec(x.m_node);
}

SNode sinh(const SNode &x) {
  return unaryExp(SINH, x);
}

SNode cosh(const SNode &x) {
  return unaryExp(COSH, x);
}

SNode tanh(const SNode &x) {
  return unaryExp(TANH, x);
}

SNode gauss(const SNode &x) {
  return unaryExp(GAUSS, x);
}

SNode unaryExp(ExpressionInputSymbol symbol, SNode n) {
  return n.node()->getTree()->unaryExpr(symbol, n.node());
}

SNode binExp(ExpressionInputSymbol symbol, SNode n1, SNode n2) {
  return n1.node()->getTree()->binaryExpr(symbol, n1.node(), n2.node());
}

SNode treeExp(ExpressionInputSymbol symbol, const ExpressionNodeArray &a) {
  ExpressionNode *n = new ExpressionNodeTree(a[0]->getTree(), symbol, a); TRACE_NEW(n);
  return n;
}

SNode condExp(SNode condition, SNode nTrue, SNode nFalse) {
  return condition.node()->getTree()->condExpr(condition.node(), nTrue.node(), nFalse.node());
}

SNode polyExp(const ExpressionNodeArray &coefArray, SNode arg) {
  return arg.node()->getTree()->getPoly(coefArray, arg.node());
}

SNode stmtList(const ExpressionNodeArray &list) {
  return list[0]->getTree()->getStmtList(list);
}

SNode indexSum(SNode assignStmt, SNode endExpr, SNode expr) {
  return assignStmt.node()->getTree()->indexedSum(assignStmt.node(), endExpr.node(), expr.node());
}

SNode indexProd(SNode assignStmt, SNode endExpr, SNode expr) {
  return assignStmt.node()->getTree()->indexedProduct(assignStmt.node(), endExpr.node(), expr.node());
}

SNode assignStmt(SNode leftSide, SNode expr) {
  return leftSide.node()->getTree()->assignStmt(leftSide.node(), expr.node());
}

SNode factorExp(SNode b, SNode e) {
  ExpressionNode *n = new ExpressionFactor(b,e); TRACE_NEW(n);
  return n;
}

Real SNode::evaluateReal() const {
  return m_node->evaluateReal();
}

bool SNode::evaluateBool() const {
  return m_node->evaluateBool();
}

SNode SNodeArray::toTree(ExpressionInputSymbol delimiter) {
  if(size() == 0) {
    return SNode();
  }
  ExpressionNodeArray tmp(size());
  for(size_t i = 0; i < size(); i++) tmp.add((*this)[i].node());
  return tmp.toTree(delimiter);
}

SStmtList::SStmtList(SNode n) {
  const ExpressionNodeArray &stmtList = n.node()->getChildArray();
  for(size_t i = 0; i < stmtList.size(); i++) {
    add(stmtList[i]);
  }
}

SStmtList &SStmtList::removeUnusedAssignments() {
  for(int i = (int)size()-1; i--;) { // Remove unused assignments
    const SNode &stmt = (*this)[i];
    const String &varName = stmt.left().name();
    bool isUsed = false;
    for(size_t j = i+1; j < size(); j++) {
      if((*this)[j].node()->dependsOn(varName)) {
        isUsed = true;
        break;
      }
    }
    if(!isUsed) {
      remove(i);
    }
  }
  return *this;
}

SExprList::SExprList(ExpressionNodeArray &a) {
  for(size_t i = 0; i < a.size(); i++) {
    add(a[i]);
  }
}

SExprList::operator ExpressionNodeArray() {
  ExpressionNodeArray result(size());
  for(size_t i = 0; i < size(); i++) {
    result.add((*this)[i].node());
  }
  return result;
}
