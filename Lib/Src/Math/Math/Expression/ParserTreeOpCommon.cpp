#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionFactor.h>
#include <Math/Expression/SumElement.h>

// ------------------------------------ Operators common to Standard/Canonical form --------------------------------------
const ExpressionNode *ParserTree::and(const ExpressionNode *n1, const ExpressionNode *n2) const {
  if(n1->isTrue()) return n2; else if(n1->isFalse()) return n1;
  if(n2->isTrue()) return n1; else if(n2->isFalse()) return n2;
  return binaryExpression(AND, n1, n2);
}

const ExpressionNode *ParserTree::or(const ExpressionNode *n1, const ExpressionNode *n2) const {
  if(n1->isTrue()) return n1; else if(n1->isFalse()) return n2;
  if(n2->isTrue()) return n2; else if(n2->isFalse()) return n1;
  return binaryExpression(OR, n1, n2);
}


const ExpressionNode *ParserTree::indexedSum(const ExpressionNode *assign, const ExpressionNode *endExpr, const ExpressionNode *expr) const {
//  assert(assign->getSymbol() == ASSIGN);
  return ternaryExpression(INDEXEDSUM, assign, endExpr, expr);
}

const ExpressionNode *ParserTree::indexedProduct(const ExpressionNode *assign, const ExpressionNode *endExpr, const ExpressionNode *expr) const {
//  assert(assign->getSymbol() == ASSIGN);
  return ternaryExpression(INDEXEDPRODUCT, assign, endExpr, expr);
}

const ExpressionNode *ParserTree::conditionalExpression(const ExpressionNode *condition, const ExpressionNode *exprTrue, const ExpressionNode *exprFalse) const {
//  assert(condition->isBooleanOperator());
  return ternaryExpression(IIF, condition, exprTrue, exprFalse);
}

const ExpressionNode *ParserTree::assignStatement(const ExpressionNode *leftSide, const ExpressionNode *expr) const {
//  assert(leftSide->getNodeType() == EXPRESSIONNODEVARIABLE);
  return binaryExpression(ASSIGN, leftSide, expr);
}

const ExpressionFactor *ParserTree::fetchFactorNode(const ExpressionNode *base, const ExpressionNode *exponent) const {
  if(exponent == NULL) {
    exponent = getOne();
  }
  if(exponent->isOne()) {
    if(base->getSymbol() == POW) {
      if(base->getNodeType() == EXPRESSIONNODEFACTOR) {
        return (const ExpressionFactor*)base;
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

const ExpressionNodePoly *ParserTree::fetchPolyNode(const ExpressionNodeArray &coefficientArray, const ExpressionNode *argument) const {
  return new ExpressionNodePoly(this, coefficientArray, argument);
}

// -----------------------------------------------------------------------------------------

const ExpressionFactor *ParserTree::getFactor(const ExpressionNode *base, const ExpressionNode *exponent) const {
  return fetchFactorNode(base, exponent);
}

const ExpressionNode *ParserTree::getSum(const AddentArray &a) const {
  switch(a.size()) {
  case 0 :
    return getZero();
  case 1 :
    return a[0]->isPositive() ? a[0]->getNode() : minusC(a[0]->getNode());
  default:
    return new ExpressionNodeSum(this, a);
  }
}

const ExpressionNode *ParserTree::getProduct(const FactorArray &a) const {
  switch(a.size()) {
  case 0 : return getOne();
  case 1 : return a[0]->exponent()->isOne() ? a[0]->base() : a[0];
  default: return new ExpressionNodeProduct(this, a);
  }
}

const ExpressionNode *ParserTree::getPoly(const ExpressionNodeArray &coefficientArray, const ExpressionNode *argument) const {
  return fetchPolyNode(coefficientArray, argument);
}

// -------------------------------------------------------------------------------------------

const ExpressionFactor *ParserTree::getFactor(const ExpressionFactor *oldFactor, const ExpressionNode *newBase, const ExpressionNode *newExpo) const {
  const ExpressionNode *oldBase = oldFactor->base();
  const ExpressionNode *oldExpo = oldFactor->exponent();
  return ((newBase == oldBase) && (newExpo == oldExpo)) ? oldFactor : getFactor(newBase, newExpo);
}

const ExpressionNode *ParserTree::getSum(const ExpressionNode *oldSum, const AddentArray &newAddentArray) const {
  const AddentArray          &oldAddentArray = oldSum->getAddentArray();
  const ExpressionNode *result         = (newAddentArray == oldAddentArray)
                                             ? oldSum
                                             : getSum(newAddentArray);
  return result;
}


const ExpressionNode *ParserTree::getProduct(const ExpressionNode *oldProduct, const FactorArray &newFactorArray) const {
  const FactorArray          &oldFactorArray = oldProduct->getFactorArray();
  const ExpressionNode *result         = (newFactorArray == oldFactorArray)
                                             ? oldProduct
                                             : getProduct(newFactorArray);
  return result;
}

const ExpressionNode *ParserTree::getPoly(const ExpressionNode *oldPoly, const ExpressionNodeArray &newCoefficientArray, const ExpressionNode *newArgument) const {
  const ExpressionNodeArray  &oldCoefArray = oldPoly->getCoefficientArray();
  const ExpressionNode *oldArgument  = oldPoly->getArgument();
  const ExpressionNode *result       = (newCoefficientArray == oldCoefArray) 
                                          && (newArgument         == oldArgument )
                                           ? oldPoly 
                                           : getPoly(newCoefficientArray, newArgument);
  return result;
}

const ExpressionNode *ParserTree::functionExpression(ExpressionInputSymbol symbol, const ExpressionNode *child) const {
  return unaryExpression(symbol, child);
}

const ExpressionNode *ParserTree::unaryMinus(const ExpressionNode *child) const {
  return unaryExpression(MINUS, child);
}

const ExpressionNode *ParserTree::unaryExpression(   ExpressionInputSymbol symbol, const ExpressionNode *child) const {
  return fetchTreeNode(symbol, child, NULL);
}

const ExpressionNode *ParserTree::binaryExpression(  ExpressionInputSymbol       symbol
                                                         , const ExpressionNode *left
                                                         , const ExpressionNode *right) const {
  return fetchTreeNode(symbol, left, right, NULL);
}

const ExpressionNode *ParserTree::ternaryExpression( ExpressionInputSymbol       symbol
                                                         , const ExpressionNode *child0
                                                         , const ExpressionNode *child1
                                                         , const ExpressionNode *child2) const {
  return fetchTreeNode(symbol, child0, child1, child2, NULL);
}

const ExpressionNodeTree *ParserTree::fetchTreeNode(ExpressionInputSymbol symbol,...) const {
  va_list argptr;
  va_start(argptr, symbol);
  ExpressionNodeTree *result = new ExpressionNodeTree(this, symbol, argptr);
  va_end(argptr);
  return result;
}

const ExpressionNode *ParserTree::getTree(ExpressionInputSymbol symbol, const ExpressionNodeArray &a) const {
  return new ExpressionNodeTree(this, symbol, a);
}

const ExpressionNode *ParserTree::getTree(const ExpressionNode *oldTree, const ExpressionNodeArray &newChildArray) const {
  const ExpressionNodeArray &oldChildArray = oldTree->getChildArray();
  const ExpressionNode *result = (newChildArray == oldChildArray) ? oldTree : getTree(oldTree->getSymbol(), newChildArray);
  return result;
}

const ExpressionNode *ParserTree::expandPower(const ExpressionNode *base, const Rational &exponent) const {
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
