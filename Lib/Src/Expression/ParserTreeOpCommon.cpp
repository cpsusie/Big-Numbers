#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionFactor.h>
#include <Math/Expression/SumElement.h>

// ------------------------------------ Operators common to Standard/Canonical form --------------------------------------
ExpressionNode *ParserTree::and(ExpressionNode *n1, ExpressionNode *n2) {
  if(n1->isTrue()) return n2; else if(n1->isFalse()) return n1;
  if(n2->isTrue()) return n1; else if(n2->isFalse()) return n2;
  return binaryExpr(SYMAND, n1, n2);
}

ExpressionNode *ParserTree::or(ExpressionNode *n1, ExpressionNode *n2) {
  if(n1->isTrue()) return n1; else if(n1->isFalse()) return n2;
  if(n2->isTrue()) return n2; else if(n2->isFalse()) return n1;
  return binaryExpr(SYMOR, n1, n2);
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
  return binaryExpr(ASSIGN, leftSide, expr);
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

ExpressionFactor *ParserTree::getFactor(ExpressionNode *base, ExpressionNode *exponent) {
  return fetchFactorNode(base, exponent);
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
  case 1 : return a[0]->exponent()->isOne() ? a[0]->base() : a[0];
  default: 
    { ExpressionNode *n = new ExpressionNodeProduct(this, a); TRACE_NEW(n);
      return n;
    }
  }
}

ExpressionNode *ParserTree::getPoly(const ExpressionNodeArray &coefArray, ExpressionNode *arg) {
  ExpressionNode *n = new ExpressionNodePoly(this, coefArray, arg); TRACE_NEW(n);
  return n;
}

ExpressionNode *ParserTree::getStmtList(const ExpressionNodeArray  &stmtArray) {
  ExpressionNode *n = new ExpressionNodeStmtList(this, stmtArray); TRACE_NEW(n);
  return n;
}

// -------------------------------------------------------------------------------------------

ExpressionFactor *ParserTree::getFactor(ExpressionFactor *oldFactor, ExpressionNode *newBase, ExpressionNode *newExpo) {
  ExpressionNode *oldBase = oldFactor->base();
  ExpressionNode *oldExpo = oldFactor->exponent();
  return ((newBase == oldBase) && (newExpo == oldExpo)) ? oldFactor : getFactor(newBase, newExpo);
}

ExpressionNode *ParserTree::getSum(ExpressionNode *oldSum, AddentArray &newAddentArray) {
  AddentArray    &oldAddentArray = oldSum->getAddentArray();
  ExpressionNode *result = (newAddentArray == oldAddentArray)
                                             ? oldSum
                                             : getSum(newAddentArray);
  return result;
}

ExpressionNode *ParserTree::getProduct(ExpressionNode *oldProduct, FactorArray &newFactorArray) {
  const FactorArray  &oldFactorArray = oldProduct->getFactorArray();
  ExpressionNode     *result         = (newFactorArray == oldFactorArray)
                                     ? oldProduct
                                     : getProduct(newFactorArray);
  return result;
}

ExpressionNode *ParserTree::getPoly(ExpressionNode *oldPoly, ExpressionNodeArray &newCoefficientArray, ExpressionNode *newArgument) {
  const ExpressionNodeArray  &oldCoefArray = oldPoly->getCoefficientArray();
  const ExpressionNode       *oldArgument  = oldPoly->getArgument();
  ExpressionNode             *result       = (newCoefficientArray == oldCoefArray)
                                          && (newArgument         == oldArgument )
                                           ? oldPoly
                                           : getPoly(newCoefficientArray, newArgument);
  return result;
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

ExpressionNode *ParserTree::getTree(ExpressionInputSymbol symbol, ExpressionNodeArray &a) {
  ExpressionNode *n = new ExpressionNodeTree(this, symbol, a); TRACE_NEW(n);
  return n;
}

ExpressionNode *ParserTree::getTree(ExpressionNode *oldTree, ExpressionNodeArray &newChildArray) {
  const ExpressionNodeArray &oldChildArray = oldTree->getChildArray();
  ExpressionNode            *result = (newChildArray == oldChildArray) ? oldTree : getTree(oldTree->getSymbol(), newChildArray);
  return result;
}

ExpressionNode *ParserTree::expandPower(ExpressionNode *base, const Rational &exponent) {
  SNode b(base);
  SNode result(getOne());
  __int64 num = exponent.getNumerator();
  __int64 den = exponent.getDenominator();
  if(num < 0) {
    b = reciprocal(b);
    num = -num;
  }
  if(den > 1) {
    b = root(b, SNode(this, den));
  }
  for(int i = 0; i < num; i++) {
    result *= b;
  }
  return result;
}
