#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionFactor.h>
#include <Math/Expression/SumElement.h>

// ------------------------------------ Operators common to Standard/Canonical form --------------------------------------
ExpressionNode *ParserTree::and(ExpressionNode *n1, ExpressionNode *n2) {
  if(n1->isTrue()) return n2; else if(n1->isFalse()) return n1;
  if(n2->isTrue()) return n1; else if(n2->isFalse()) return n2;
  return binaryExpression(SYMAND, n1, n2);
}

ExpressionNode *ParserTree::or(ExpressionNode *n1, ExpressionNode *n2) {
  if(n1->isTrue()) return n1; else if(n1->isFalse()) return n2;
  if(n2->isTrue()) return n2; else if(n2->isFalse()) return n1;
  return binaryExpression(SYMOR, n1, n2);
}


ExpressionNode *ParserTree::indexedSum(ExpressionNode *assign, ExpressionNode *endExpr, ExpressionNode *expr) {
//  assert(assign->getSymbol() == ASSIGN);
  return ternaryExpression(INDEXEDSUM, assign, endExpr, expr);
}

ExpressionNode *ParserTree::indexedProduct(ExpressionNode *assign, ExpressionNode *endExpr, ExpressionNode *expr) {
//  assert(assign->getSymbol() == ASSIGN);
  return ternaryExpression(INDEXEDPRODUCT, assign, endExpr, expr);
}

ExpressionNode *ParserTree::conditionalExpression(ExpressionNode *condition, ExpressionNode *exprTrue, ExpressionNode *exprFalse) {
//  assert(condition->isBooleanOperator());
  return ternaryExpression(IIF, condition, exprTrue, exprFalse);
}

ExpressionNode *ParserTree::assignStatement(ExpressionNode *leftSide, ExpressionNode *expr) {
//  assert(leftSide->getNodeType() == EXPRESSIONNODEVARIABLE);
  return binaryExpression(ASSIGN, leftSide, expr);
}

ExpressionFactor *ParserTree::fetchFactorNode(ExpressionNode *base, ExpressionNode *exponent) {
  if(exponent == NULL) {
    exponent = getOne();
  }
  if(exponent->isOne()) {
    if(base->getSymbol() == POW) {
      if(base->getNodeType() == EXPRESSIONNODEFACTOR) {
        return (ExpressionFactor*)base;
      } else {
        return new ExpressionFactor(base->left(), base->right());
      }
    } else {
      return new ExpressionFactor(base, exponent);
    }
  } else { // exponent != 1
    if(base->getSymbol() == POW) {
      return new ExpressionFactor(base->left(), productC(base->right(), exponent));
    } else {
      return new ExpressionFactor(base, exponent);
    }
  }
}

ExpressionNodePoly *ParserTree::fetchPolyNode(const ExpressionNodeArray &coefficientArray, ExpressionNode *argument) {
  return new ExpressionNodePoly(this, coefficientArray, argument);
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
    return a[0]->isPositive() ? a[0]->getNode() : minusC(a[0]->getNode());
  default:
    return new ExpressionNodeSum(this, a);
  }
}

ExpressionNode *ParserTree::getProduct(FactorArray &a) {
  switch(a.size()) {
  case 0 : return getOne();
  case 1 : return a[0]->exponent()->isOne() ? a[0]->base() : a[0];
  default: return new ExpressionNodeProduct(this, a);
  }
}

ExpressionNode *ParserTree::getPoly(ExpressionNodeArray &coefficientArray, ExpressionNode *argument) {
  return fetchPolyNode(coefficientArray, argument);
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

ExpressionNode *ParserTree::functionExpression(ExpressionInputSymbol symbol, ExpressionNode *child) {
  return unaryExpression(symbol, child);
}

ExpressionNode *ParserTree::unaryMinus(ExpressionNode *child) {
  return unaryExpression(MINUS, child);
}

ExpressionNode *ParserTree::unaryExpression(ExpressionInputSymbol symbol, ExpressionNode *child) {
  return fetchTreeNode(symbol, child, NULL);
}

ExpressionNode *ParserTree::binaryExpression(ExpressionInputSymbol symbol
                                            ,ExpressionNode       *left
                                            ,ExpressionNode       *right) {
  return fetchTreeNode(symbol, left, right, NULL);
}

ExpressionNode *ParserTree::ternaryExpression( ExpressionInputSymbol  symbol
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
  return result;
}

ExpressionNode *ParserTree::getTree(ExpressionInputSymbol symbol, ExpressionNodeArray &a) {
  return new ExpressionNodeTree(this, symbol, a);
}

ExpressionNode *ParserTree::getTree(ExpressionNode *oldTree, ExpressionNodeArray &newChildArray) {
  const ExpressionNodeArray &oldChildArray = oldTree->getChildArray();
  ExpressionNode            *result = (newChildArray == oldChildArray) ? oldTree : getTree(oldTree->getSymbol(), newChildArray);
  return result;
}

ExpressionNode *ParserTree::expandPower(ExpressionNode *base, const Rational &exponent) {
  SNode b(base);
  SNode result(_1());
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
