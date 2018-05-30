#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionFactor.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/SNode.h>

namespace Expr {

// ------------------------------------ Operators common to Standard/Canonical form --------------------------------------
ExpressionNode *ParserTree::and(ExpressionNode *n1, ExpressionNode *n2) {
  if(n1->isTrue()) return n2; else if(n1->isFalse()) return n1;
  if(n2->isTrue()) return n1; else if(n2->isFalse()) return n2;
  return boolExpr(AND, n1,n2);
}

ExpressionNode *ParserTree::or(ExpressionNode *n1, ExpressionNode *n2) {
  if(n1->isTrue()) return n1; else if(n1->isFalse()) return n2;
  if(n2->isTrue()) return n2; else if(n2->isFalse()) return n1;
  return boolExpr(OR, n1,n2);
}

ExpressionNode *ParserTree::not(ExpressionNode *n) {
  if(n->isTrue()) return getFalse(); else if(n->isFalse()) return getTrue();
  if(n->getSymbol() == NOT) return n->left();
  return boolExpr(NOT,n,NULL);
}

ExpressionNode *ParserTree::boolExpr(ExpressionInputSymbol symbol, ExpressionNode *n1, ExpressionNode *n2) {
  ExpressionNode *result;
  if(n2) {
    result = new ExpressionNodeBoolExpr(this,symbol,n1,n2);
  } else {
    result = new ExpressionNodeBoolExpr(this,symbol,n1);
  }
  TRACE_NEW(result);
  return result;
}

ExpressionNode *ParserTree::treeExpr(ExpressionInputSymbol symbol, SNodeArray &a) {
  ExpressionNode *n = new ExpressionNodeTree(this, symbol, a); TRACE_NEW(n);
  return n;
}

ExpressionNode *ParserTree::assignStmt(ExpressionNode *leftSide, ExpressionNode *expr) {
  ExpressionNode *n = new ExpressionNodeAssign(leftSide, expr); TRACE_NEW(n);
  return n;
}

ExpressionNode *ParserTree::factorExpr(ExpressionNode *base, ExpressionNode *expo) {
  return fetchFactorNode(base,expo);
}

ExpressionNode *ParserTree::indexedSum(ExpressionNode *assign, ExpressionNode *endExpr, ExpressionNode *expr) {
//  assert(assign->getSymbol() == ASSIGN);
  return ternaryExpr(INDEXEDSUM, assign, endExpr, expr);
}

ExpressionNode *ParserTree::indexedProduct(ExpressionNode *assign, ExpressionNode *endExpr, ExpressionNode *expr) {
//  assert(assign->getSymbol() == ASSIGN);
  return ternaryExpr(INDEXEDPRODUCT, assign, endExpr, expr);
}

ExpressionNode *ParserTree::condExpr(ExpressionNode *condition, ExpressionNode *exprTrue, ExpressionNode *exprFalse) {
//  assert(condition->isBooleanOperator());
  return ternaryExpr(IIF, condition, exprTrue, exprFalse);
}

ExpressionFactor *ParserTree::fetchFactorNode(SNode base) {
  return fetchFactorNode(base,base._1());
}

ExpressionFactor *ParserTree::fetchFactorNode(SNode base, SNode exponent) {
  ExpressionFactor *f;
  if(exponent.isOne()) {
    if(base.getSymbol() != POW) {
      f = new ExpressionFactor(base, exponent);
    } else if(base.getNodeType() == NT_FACTOR) {
      return (ExpressionFactor*)(base.node());
    } else {
      f = new ExpressionFactor(base.left(), base.right());
    }
  } else if(base.getSymbol() != POW) { // exponent != 1
    f = new ExpressionFactor(base, exponent);
  } else if(base.right().isOne()) {
    f = new ExpressionFactor(base.left(), exponent);
  } else { // both exponents are != 1
    f = new ExpressionFactor(base.left(), multiplyExponents(base.right().node(),exponent.node()));
  }
  TRACE_NEW(f);
  return f;
}

ExpressionNode *ParserTree::multiplyExponents(ExpressionNode *n1, ExpressionNode *n2) {
  if(n1->isRational() && n2->isRational()) {
    const Rational r1 = n1->getRational();
    const Rational r2 = n2->getRational();
    if(rationalExponentsMultiply(r1, r2)) {
      return numberExpression(r1*r2);
    } else {
      const Rational r   = r1 * r2;
      const INT64    num = 2*r.getNumerator();
      const INT64    den = 2*r.getDenominator();
      return quot(num,den);
    }
  } else {
    return prod(n1,n2);
  }
}

ExpressionNode *ParserTree::divideExponents(ExpressionNode *n1, ExpressionNode *n2) {
  if(n1->isRational() && n2->isRational()) {
    const Rational r1 = n1->getRational();
    const Rational r2 = n2->getRational();
    if(rationalExponentsMultiply(r1, r2)) {
      return numberExpression(r1/r2);
    } else {
      const Rational r   = r1 / r2;
      const INT64    num = 2*r.getNumerator();
      const INT64    den = 2*r.getDenominator();
      return quot(num,den);
    }
  } else {
    return quot(n1,n2);
  }
}

// -------------------------------------------------------------------------------------------

ExpressionNode *ParserTree::getPoly(SNode oldPoly, SNodeArray &newCoefArray, SNode newArgument) {
  const SNodeArray &oldCoefArray = oldPoly.getCoefArray();
  const SNode       oldArgument  = oldPoly.getArgument().node();
  SNode             result       = (newCoefArray.isSameNodes(oldCoefArray) && newArgument.isSameNode(oldArgument))
                                 ? oldPoly
                                 : getPoly(newCoefArray, newArgument);
  return result.node();
}

ExpressionNode *ParserTree::getBoolExpr(SNode oldExpr, SNodeArray &newChildArray) {
  const SNodeArray &oldChildArray = oldExpr.getChildArray();
  SNode             result = newChildArray.isSameNodes(oldChildArray) ? oldExpr : getBoolExpr(oldExpr.getSymbol(), newChildArray);
  return result.node();
}

ExpressionNode *ParserTree::getTree(SNode oldTree, SNodeArray &newChildArray) {
  const SNodeArray &oldChildArray = oldTree.getChildArray();
  SNode             result = newChildArray.isSameNodes(oldChildArray) ? oldTree : getTree(oldTree.getSymbol(), newChildArray);
  return result.node();
}

ExpressionNode *ParserTree::getAssignStmt(SNode oldAssign, SNodeArray &newChildArray) {
  const SNodeArray &oldChildArray = oldAssign.getChildArray();
  SNode             result = newChildArray.isSameNodes(oldChildArray) ? oldAssign: getAssignStmt(newChildArray);
  return result.node();
}

ExpressionNode *ParserTree::getStmtList(SNode oldStmtList, SNodeArray &newChildArray) {
  const SNodeArray &oldChildArray = oldStmtList.getChildArray();
  SNode             result = newChildArray.isSameNodes(oldChildArray) ? oldStmtList : getStmtList(newChildArray);
  return result.node();
}

ExpressionNode *ParserTree::getSum(SNode oldSum, AddentArray &newAddentArray) {
  const AddentArray &oldAddentArray = oldSum.getAddentArray();
  SNode              result         = (newAddentArray == oldAddentArray)
                                    ? oldSum
                                    : getSum(newAddentArray);
  return result.node();
}

ExpressionNode *ParserTree::getProduct(SNode oldProduct, FactorArray &newFactorArray) {
  const FactorArray  &oldFactorArray = oldProduct.getFactorArray();
  SNode               result         = (newFactorArray == oldFactorArray)
                                     ? oldProduct
                                     : getProduct(newFactorArray);
  return result.node();
}

ExpressionFactor *ParserTree::getFactor(SNode oldFactor, SNode newBase, SNode newExpo) {
  SNode oldBase = oldFactor.left();
  SNode oldExpo = oldFactor.right();
  return (newBase.isSameNode(oldBase) && newExpo.isSameNode(oldExpo)) ? (ExpressionFactor*)oldFactor.node() : fetchFactorNode(newBase, newExpo);
}

// -----------------------------------------------------------------------------------------

ExpressionNode *ParserTree::getPoly(SNodeArray &coefArray, SNode arg) {
  ExpressionNode *n = new ExpressionNodePoly(this, coefArray, arg); TRACE_NEW(n);
  return n;
}

ExpressionNode *ParserTree::getBoolExpr(ExpressionInputSymbol symbol, SNodeArray &childArray) {
  if(childArray.size() == 2) {
    assert(ExpressionNode::isBooleanOperator(symbol) && (symbol!=NOT));
    return boolExpr(symbol,childArray[0].node(), childArray[1].node());
  } else {
    assert(symbol == NOT);
    return boolExpr(symbol,childArray[0].node(), NULL);
  }
}

ExpressionNode *ParserTree::getTree(ExpressionInputSymbol symbol, SNodeArray &a) {
  return treeExpr(symbol, a);
}

ExpressionNode *ParserTree::getAssignStmt(SNodeArray &a) {
  assert(a.size() == 2);
  assert(a[0].getNodeType() == NT_VARIABLE);
  return assignStmt(a[0].node(), a[1].node());
}

ExpressionNode *ParserTree::getStmtList(SNodeArray &stmtArray) {
  ExpressionNode *n = new ExpressionNodeStmtList(this, stmtArray); TRACE_NEW(n);
  return n;
}


ExpressionNode *ParserTree::getSum(AddentArray &a) {
  switch(a.size()) {
  case 0 :
    return getZero();
  case 1 :
    return a[0]->isPositive() ? a[0]->getNode() : minus(a[0]->getNode());
  default:
    { ExpressionNode *n = new ExpressionNodeSum(this, a); TRACE_NEW(n);
      return n;
    }
  }
}

ExpressionNode *ParserTree::getProduct(FactorArray &a) {
  switch(a.size()) {
  case 0 : return getOne();
  case 1 : return a[0]->exponent().isOne() ? a[0]->base().node() : a[0];
  default: 
    { ExpressionNode *n = new ExpressionNodeProduct(this, a); TRACE_NEW(n);
      return n;
    }
  }
}

// -----------------------------------------------------------------------------------------------------

ExpressionNode *ParserTree::functionExpr(ExpressionInputSymbol symbol, ExpressionNode *child) {
  return unaryExpr(symbol, child);
}

ExpressionNode *ParserTree::unaryMinus(ExpressionNode *child) {
  return unaryExpr(MINUS, child);
}

ExpressionNode *ParserTree::unaryExpr(ExpressionInputSymbol symbol, ExpressionNode *child) {
  return fetchTreeNode(symbol, child, NULL);
}

ExpressionNode *ParserTree::binaryExpr(ExpressionInputSymbol symbol
                                      ,ExpressionNode       *left
                                      ,ExpressionNode       *right) {
  return fetchTreeNode(symbol, left, right, NULL);
}

ExpressionNode *ParserTree::ternaryExpr( ExpressionInputSymbol  symbol
                                       , ExpressionNode        *child0
                                       , ExpressionNode        *child1
                                       , ExpressionNode        *child2) {
  return fetchTreeNode(symbol, child0, child1, child2, NULL);
}

ExpressionNodeTree *ParserTree::fetchTreeNode(ExpressionInputSymbol symbol,...) {
  va_list argptr;
  va_start(argptr, symbol);
  ExpressionNodeTree *result = new ExpressionNodeTree(this, symbol, argptr);
  va_end(argptr);
  TRACE_NEW(result);
  return result;
}

ExpressionNode *ParserTree::expandPower(ExpressionNode *base, const Rational &exponent) {
  SNode b(base);
  SNode result(getOne());
  __int64 num = exponent.getNumerator();
  __int64 den = exponent.getDenominator();
  if(num < 0) {
    b = reciprocal(b.node());
    num = -num;
  }
  if(den > 1) {
    b = root(b.node(), SNode(*this, den).node());
  }
  for(int i = 0; i < num; i++) {
    result *= b;
  }
  return result.node();
}

}; // namespace Expr
