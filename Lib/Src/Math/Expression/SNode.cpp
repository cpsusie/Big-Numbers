#include "pch.h"
#include <Math/Expression/Expression.h>

SNode::SNode(ParserTree *tree, int v) {
  m_node = tree->numberExpression(v);
}

SNode::SNode(ParserTree *tree, __int64 v) {
  m_node = tree->numberExpression(v);
}

SNode::SNode(ParserTree *tree, const Rational &v) {
  m_node = tree->numberExpression(v);
}

SNode::SNode(ParserTree *tree, const Real &v) {
  m_node = tree->numberExpression(v);
}


#define CALLUNARYOP(op, n1)               \
  ParserTree *tree = n1->getTree();       \
  return (tree->*(tree->op))(n1);

#define CALLBINOP(op, n1,n2)              \
  ParserTree *tree = n1->getTree();       \
  return (tree->*(tree->op))(n1, n2);


SNode SNode::operator+(const SNode &n) const {
  CALLBINOP(psum, m_node, n.m_node);
}

SNode SNode::operator-(const SNode &n) const { // binary -
  CALLBINOP(pdiff, m_node, n.m_node);
}

SNode SNode::operator-() const { // unary  -
  CALLUNARYOP(pminus, m_node);
}

SNode SNode::operator*(const SNode &n) const {
  CALLBINOP(pprod, m_node, n.m_node);
}

SNode SNode::operator/(const SNode &n) const {
  CALLBINOP(pquot, m_node, n.m_node);
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
  CALLBINOP(pmod, m_node, n.m_node);
}

SNode SNode::operator&&(const SNode &n) const {
  return m_node->getTree()->and(m_node, n.m_node);
}

SNode SNode::operator||(const SNode &n) const {
  return m_node->getTree()->or(m_node, n.m_node);
}

SNode SNode::operator!() const {
  return m_node->getTree()->unaryExpression(NOT, m_node);
}

bool SNode::operator==(const SNode &n) const {
  return m_node->getExpr()->treesEqual(m_node, n.m_node);
}

SNode reciprocal(const SNode &x) {
  CALLUNARYOP(preciprocal, x.m_node);
}

SNode sqrt(const SNode &x) {
  CALLUNARYOP(psqrt, x.m_node);
}

SNode sqr(const SNode &x) {
  CALLUNARYOP(psqr, x.m_node);
}

SNode pow(const SNode &x, const SNode &y) {
  CALLBINOP(ppower, x.m_node, y.m_node);
}

SNode root(const SNode &x, const SNode &y) {
  CALLBINOP(proot, x.m_node, y.m_node);
}

SNode exp(const SNode &x) {
  CALLUNARYOP(pexp, x.m_node);
}

SNode exp10(const SNode &x) {
  CALLUNARYOP(pexp10, x.m_node);
}

SNode exp2(const SNode &x) {
  CALLUNARYOP(pexp2, x.m_node);
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
  CALLUNARYOP(pcot, x.m_node);
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

SNode csc(const SNode &x) {
  CALLUNARYOP(pcsc, x.m_node);
}

SNode sec(const SNode &x) {
  CALLUNARYOP(psec, x.m_node);
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
  return n.node()->getTree()->unaryExpression(symbol, n.node());
}

SNode binExp(ExpressionInputSymbol symbol, SNode n1, SNode n2) {
  return n1.node()->getTree()->binaryExpression(symbol, n1.node(), n2.node());
}

SNode condExp(SNode condition, SNode nTrue, SNode nFalse) {
  return condition.node()->getTree()->conditionalExpression(condition.node(), nTrue.node(), nFalse.node());
}

SNode polyExp(SExprList &coefficientArray, SNode argument) {
  return argument.node()->getTree()->fetchPolyNode(coefficientArray, argument);
}

SNode indexSum(SNode assignStmt, SNode endExpr, SNode expr) {
  return assignStmt.node()->getTree()->indexedSum(assignStmt.node(), endExpr.node(), expr.node());
}

SNode indexProd(SNode assignStmt, SNode endExpr, SNode expr) {
  return assignStmt.node()->getTree()->indexedProduct(assignStmt.node(), endExpr.node(), expr.node());
}

SNode assignStmt(SNode leftSide, SNode expr) {
  return leftSide.node()->getTree()->assignStatement(leftSide.node(), expr.node());
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
  ExpressionNodeArray stmtList = getStatementList(n.node());
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
