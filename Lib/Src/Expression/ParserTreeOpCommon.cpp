#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/SNode.h>

namespace Expr {

// ------------------------------------ Operators common to Standard/Canonical form --------------------------------------

ExpressionNode *ParserTree::stmtList(SNodeArray &stmtArray) {
  ExpressionNode *n = new ExpressionNodeStmtList(this, stmtArray); TRACE_NEW(n);
  return n;
}

ExpressionNode *ParserTree::assignStmt(ExpressionNode *leftSide, ExpressionNode *expr) {
  assert(leftSide->getNodeType() == NT_VARIABLE);
  ExpressionNode *n = new ExpressionNodeAssign(leftSide, expr); TRACE_NEW(n);
  return n;
}

ExpressionNode *ParserTree::assignStmt(SNodeArray &a) {
  assert(a.size() == 2);
  return assignStmt(a[0].node(), a[1].node());
}

ExpressionNode *ParserTree::treeExpr(ExpressionInputSymbol symbol, SNodeArray &a) {
  switch(symbol) {
  case STMTLIST: return stmtList(  a);
  case ASSIGN  : return assignStmt(a);
  case SUM     :
  case PRODUCT : throwInvalidArgumentException(__TFUNCTION__
                                              ,_T("symbol=%s not allowed")
                                              ,ExpressionNode::getSymbolName(symbol).cstr()
                                              );
  case AND     :
  case OR      :
  case NOT     :
  case EQ      :
  case NE      :
  case LT      :
  case LE      :
  case GT      :
  case GE      : return boolExpr(symbol, a);
  default      :
    { ExpressionNode *n = new ExpressionNodeTree(this, symbol, a); TRACE_NEW(n);
      return n;
    }
  }
}

ExpressionNode *ParserTree::boolExpr(ExpressionInputSymbol symbol, ExpressionNode *n1, ExpressionNode *n2) {
  ExpressionNode *result;
  if(n2) {
    assert(ExpressionNode::isBooleanOperator(symbol) && (symbol!=NOT));
    result = new ExpressionNodeBoolExpr(this,symbol,n1,n2);
  } else {
    assert(symbol == NOT);
    result = new ExpressionNodeBoolExpr(this,symbol,n1);
  }
  TRACE_NEW(result);
  return result;
}

ExpressionNode *ParserTree::boolExpr(ExpressionInputSymbol symbol, SNodeArray &childArray) {
  if(childArray.size() == 2) {
    return boolExpr(symbol,childArray[0].node(), childArray[1].node());
  } else {
    return boolExpr(symbol,childArray[0].node(), NULL);
  }
}

ExpressionNode *ParserTree::polyExpr(SNodeArray &coefArray, SNode arg) {
  ExpressionNode *n = new ExpressionNodePoly(this, coefArray, arg); TRACE_NEW(n);
  return n;
}

ExpressionNode *ParserTree::sumExpr(AddentArray &a) {
  switch(a.size()) {
  case 0 : return numberExpr(0);
  case 1 : return a[0].isPositive() ? a[0].left().node() : minus(a[0].left().node());
  default:
    { ExpressionNode *n = new ExpressionNodeSum(this, a); TRACE_NEW(n);
      return n;
    }
  }
}

ExpressionNode *ParserTree::productExpr(FactorArray &a) {
  switch(a.size()) {
  case 0 : return numberExpr(1);
  case 1 : return a[0].exponent().isOne() ? a[0].base().node() : a[0].node();
  default:
    { ExpressionNode *n = new ExpressionNodeProduct(this, a); TRACE_NEW(n);
      return n;
    }
  }
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

ExpressionNode *ParserTree::funcExpr(ExpressionInputSymbol symbol, ExpressionNode *child) {
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

ExpressionNode *ParserTree::factorExpr(SNode base) {
  return factorExpr(base,base._1());
}

ExpressionNode *ParserTree::factorExpr(SNode base, SNode exponent) {
  ExpressionNode *f;
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

ExpressionNode *ParserTree::factorExpr(SNode base, const Rational &exponent) {
  return factorExpr(base,numberExpr(exponent));
}

ExpressionNode *ParserTree::addentExpr(SNode n, bool positive) {
  ExpressionNode *a;
  if(n.getNodeType() == NT_ADDENT) {
    return (n.isPositive() == positive) ? n.node() : addentExpr(n.left(),!positive);
  } else {
    a = new ExpressionAddent(n, positive);
  }
  TRACE_NEW(a);
  return a;

}

ExpressionNode *ParserTree::multiplyExponents(ExpressionNode *n1, ExpressionNode *n2) {
  if(n1->isRational() && n2->isRational()) {
    const Rational r1 = n1->getRational();
    const Rational r2 = n2->getRational();
    if(rationalExponentsMultiply(r1, r2)) {
      return numberExpr(r1*r2);
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
      return numberExpr(r1/r2);
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

ExpressionNode *ParserTree::getStmtList(SNode oldStmtList, SNodeArray &newChildArray) {
  const SNodeArray &oldChildArray = oldStmtList.getChildArray();
  SNode             result = newChildArray.isSameNodes(oldChildArray) ? oldStmtList : stmtList(newChildArray);
  return result.node();
}

ExpressionNode *ParserTree::getAssignStmt(SNode oldAssign, SNodeArray &newChildArray) {
  const SNodeArray &oldChildArray = oldAssign.getChildArray();
  SNode             result = newChildArray.isSameNodes(oldChildArray) ? oldAssign: assignStmt(newChildArray);
  return result.node();
}

ExpressionNode *ParserTree::getTree(SNode oldTree, SNodeArray &newChildArray) {
  const SNodeArray &oldChildArray = oldTree.getChildArray();
  SNode             result = newChildArray.isSameNodes(oldChildArray) ? oldTree : treeExpr(oldTree.getSymbol(), newChildArray);
  return result.node();
}

ExpressionNode *ParserTree::getBoolExpr(SNode oldExpr, SNodeArray &newChildArray) {
  const SNodeArray &oldChildArray = oldExpr.getChildArray();
  SNode             result = newChildArray.isSameNodes(oldChildArray) ? oldExpr : boolExpr(oldExpr.getSymbol(), newChildArray);
  return result.node();
}

ExpressionNode *ParserTree::getPoly(SNode oldPoly, SNodeArray &newCoefArray, SNode newArgument) {
  const SNodeArray &oldCoefArray = oldPoly.getCoefArray();
  const SNode       oldArgument  = oldPoly.getArgument().node();
  SNode             result       = (newCoefArray.isSameNodes(oldCoefArray) && newArgument.isSameNode(oldArgument))
                                 ? oldPoly
                                 : polyExpr(newCoefArray, newArgument);
  return result.node();
}

ExpressionNode *ParserTree::getSum(SNode oldSum, AddentArray &newAddentArray) {
  const AddentArray &oldAddentArray = oldSum.getAddentArray();
  SNode             result          = newAddentArray.isSameNodes(oldAddentArray)
                                    ? oldSum
                                    : sumExpr(newAddentArray);
  return result.node();
}

ExpressionNode *ParserTree::getProduct(SNode oldProduct, FactorArray &newFactorArray) {
  const FactorArray  &oldFactorArray = oldProduct.getFactorArray();
  SNode               result         = (newFactorArray.isSameNodes(oldFactorArray))
                                     ? oldProduct
                                     : productExpr(newFactorArray);
  return result.node();
}

ExpressionNode *ParserTree::getFactor(SNode oldFactor, SNode newBase, SNode newExpo) {
  SNode oldBase = oldFactor.left();
  SNode oldExpo = oldFactor.right();
  return (newBase.isSameNode(oldBase) && newExpo.isSameNode(oldExpo)) ? oldFactor.node() : factorExpr(newBase, newExpo);
}

// -----------------------------------------------------------------------------------------

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
  SNode result(b._1());
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
