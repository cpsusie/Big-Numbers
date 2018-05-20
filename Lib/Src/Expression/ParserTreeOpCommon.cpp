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
  return binaryExpr(AND, n1, n2);
}

ExpressionNode *ParserTree::or(ExpressionNode *n1, ExpressionNode *n2) {
  if(n1->isTrue()) return n1; else if(n1->isFalse()) return n2;
  if(n2->isTrue()) return n2; else if(n2->isFalse()) return n1;
  return binaryExpr(OR, n1, n2);
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

ExpressionNode *ParserTree::assignStmt(ExpressionNode *leftSide, ExpressionNode *expr) {
//  assert(leftSide->getNodeType() == EXPRESSIONNODEVARIABLE);
  ExpressionNode *n = new ExpressionNodeAssign(leftSide, expr); TRACE_NEW(n);
  return n;
}

ExpressionFactor *ParserTree::fetchFactorNode(ExpressionNode *base, ExpressionNode *exponent) {
  if(exponent == NULL) {
    exponent = getOne();
  }
  ExpressionFactor *f;
  if(exponent->isOne()) {
    if(base->getSymbol() == POW) {
      if(base->getNodeType() == EXPRESSIONNODEFACTOR) {
        return (ExpressionFactor*)base;
      } else {
        f = new ExpressionFactor(base->left(), base->right());
      }
    } else {
      f = new ExpressionFactor(base, exponent);
    }
  } else { // exponent != 1
    if(base->getSymbol() == POW) {
      f = new ExpressionFactor(base->left(), prod(base->right(), exponent));
    } else {
      f = new ExpressionFactor(base, exponent);
    }
  }
  TRACE_NEW(f);
  return f;
}

// -----------------------------------------------------------------------------------------

ExpressionFactor *ParserTree::getFactor(SNode base) {
  return fetchFactorNode(base.node());
}

ExpressionFactor *ParserTree::getFactor(SNode base, SNode exponent) {
  return fetchFactorNode(base.node(), exponent.node());
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

ExpressionNode *ParserTree::getPoly(const SNodeArray &coefArray, SNode arg) {
  ExpressionNode *n = new ExpressionNodePoly(this, coefArray, arg); TRACE_NEW(n);
  return n;
}

ExpressionNode *ParserTree::getStmtList(const SNodeArray &stmtArray) {
  ExpressionNode *n = new ExpressionNodeStmtList(this, stmtArray); TRACE_NEW(n);
  return n;
}

// -------------------------------------------------------------------------------------------

ExpressionFactor *ParserTree::getFactor(SNode oldFactor, SNode newBase, SNode newExpo) {
  SNode oldBase = oldFactor.left();
  SNode oldExpo = oldFactor.right();
  return (newBase.isSameNode(oldBase) && newExpo.isSameNode(oldExpo)) ? (ExpressionFactor*)oldFactor.node() : getFactor(newBase, newExpo);
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

ExpressionNode *ParserTree::getPoly(SNode oldPoly, SNodeArray &newCoefArray, SNode newArgument) {
  const SNodeArray &oldCoefArray = oldPoly.getCoefArray();
  const SNode       oldArgument  = oldPoly.getArgument().node();
  SNode             result       = (newCoefArray == oldCoefArray)
                                && (newArgument.isSameNode(oldArgument))
                                 ? oldPoly
                                 : getPoly(newCoefArray, newArgument);
  return result.node();
}

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

ExpressionNode *ParserTree::getTree(ExpressionInputSymbol symbol, SNodeArray &a) {
  ExpressionNode *n = new ExpressionNodeTree(this, symbol, a); TRACE_NEW(n);
  return n;
}

ExpressionNode *ParserTree::getTree(SNode oldTree, SNodeArray &newChildArray) {
  const SNodeArray &oldChildArray = oldTree.getChildArray();
  SNode             result = (newChildArray == oldChildArray) ? oldTree : getTree(oldTree.getSymbol(), newChildArray);
  return result.node();
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
    b = root(b.node(), SNode(this, den).node());
  }
  for(int i = 0; i < num; i++) {
    result *= b;
  }
  return result.node();
}

}; // namespace Expr
