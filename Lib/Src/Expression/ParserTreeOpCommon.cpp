#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/SNode.h>

namespace Expr {

// ------------------------------------ Operators common to Standard/Canonical form --------------------------------------

ExpressionNode *ParserTree::stmtList(const SNodeArray &stmtArray) {
  ExpressionNode *n = new ExpressionNodeStmtList(this, stmtArray); TRACE_NEW(n);
  return n;
}

ExpressionNode *ParserTree::assignStmt(ExpressionNode *leftSide, ExpressionNode *expr) {
  CHECKNODEPTYPE(      leftSide,NT_VARIABLE     );
  CHECKNODEPRETURNTYPE(expr    ,EXPR_RETURN_REAL);
  ExpressionNode *n = new ExpressionNodeAssign(leftSide, expr); TRACE_NEW(n);
  return n;
}

ExpressionNode *ParserTree::assignStmt(const SNodeArray &a) {
  assert(a.size() == 2);
  return assignStmt(a[0].node(), a[1].node());
}

ExpressionNode *ParserTree::treeExpr(ExpressionInputSymbol symbol, const SNodeArray &a) {
  switch(symbol) {
  case STMTLIST: return stmtList(  a);
  case ASSIGN  : return assignStmt(a);
  case SUM     :
  case PRODUCT : throwInvalidArgumentException(__TFUNCTION__
                                              ,_T("symbol=%s not allowed")
                                              ,ExpressionNode::getSymbolName(symbol).cstr()
                                              );
  case POW     : return powerExpr(a[0].node(),a[1].node());
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

ExpressionNode *ParserTree::boolExpr(ExpressionInputSymbol symbol, const SNodeArray &childArray) {
  if(childArray.size() == 2) {
    return boolExpr(symbol,childArray[0].node(), childArray[1].node());
  } else {
    return boolExpr(symbol,childArray[0].node(), NULL);
  }
}

ExpressionNode *ParserTree::polyExpr(const CoefArray &coefArray, SNode arg) {
  ExpressionNode *n = new ExpressionNodePoly(this, coefArray, arg); TRACE_NEW(n);
  return n;
}

ExpressionNode *ParserTree::sumExpr(const AddentArray &a) {
  switch(a.size()) {
  case 0 : return numberExpr(0);
  case 1 : return a[0].isPositive() ? a[0].left().node() : minus(a[0].left().node());
  default:
    { ExpressionNode *n = new ExpressionNodeSum(this, a); TRACE_NEW(n);
      return n;
    }
  }
}

ExpressionNode *ParserTree::productExpr(const FactorArray &a) {
  switch(a.size()) {
  case 0 : return numberExpr(1);
  case 1 : return a[0].exponent().isOne() ? a[0].base().node() : a[0].node();
  default:
    { ExpressionNode *n = new ExpressionNodeProduct(this, a); TRACE_NEW(n);
      return n;
    }
  }
}

ExpressionNode *ParserTree::indexedSum(SNode assign, SNode endExpr, SNode expr) {
  CHECKNODETYPE(      assign , NT_ASSIGN      );
  CHECKNODERETURNTYPE(endExpr,EXPR_RETURN_REAL);
  CHECKNODERETURNTYPE(expr   ,EXPR_RETURN_REAL);
  return ternaryExpr(INDEXEDSUM, assign.node(), endExpr.node(), expr.node());
}

ExpressionNode *ParserTree::indexedProduct(SNode assign, SNode endExpr, SNode expr) {
  CHECKNODETYPE(      assign ,NT_ASSIGN       );
  CHECKNODERETURNTYPE(endExpr,EXPR_RETURN_REAL);
  CHECKNODERETURNTYPE(expr   ,EXPR_RETURN_REAL);
  return ternaryExpr(INDEXEDPRODUCT, assign.node(), endExpr.node(), expr.node());
}

// symbol must be INDEXEDSUM/INDEXPRODUCT
ExpressionNode *ParserTree::indexedExpr(  ExpressionInputSymbol symbol, SNode assign, SNode endExpr, SNode expr) {
  switch(symbol) {
  case INDEXEDSUM    : return indexedSum(    assign,endExpr,expr);
  case INDEXEDPRODUCT: return indexedProduct(assign,endExpr,expr);
  default            : throwInvalidArgumentException(__TFUNCTION__,_T("symbol=%s"), ExpressionNode::getSymbolName(symbol).cstr());
                       return NULL;
  }
}

ExpressionNode *ParserTree::condExpr(SNode condition, SNode exprTrue, SNode exprFalse) {
  CHECKNODETYPE(      condition ,NT_BOOLEXPR     );
  CHECKNODERETURNTYPE(exprTrue  ,EXPR_RETURN_REAL);
  CHECKNODERETURNTYPE(exprFalse ,EXPR_RETURN_REAL);
  return ternaryExpr(IIF, condition.node(), exprTrue.node(), exprFalse.node());
}

ExpressionNode *ParserTree::and(ExpressionNode *n1, ExpressionNode *n2) {
  CHECKNODEPRETURNTYPE(n1,EXPR_RETURN_BOOL);
  CHECKNODEPRETURNTYPE(n2,EXPR_RETURN_BOOL);
  if(n1->isTrue()) return n2; else if(n1->isFalse()) return n1;
  if(n2->isTrue()) return n1; else if(n2->isFalse()) return n2;
  return boolExpr(AND, n1,n2);
}

ExpressionNode *ParserTree::or(ExpressionNode *n1, ExpressionNode *n2) {
  CHECKNODEPRETURNTYPE(n1,EXPR_RETURN_BOOL);
  CHECKNODEPRETURNTYPE(n2,EXPR_RETURN_BOOL);
  if(n1->isTrue()) return n1; else if(n1->isFalse()) return n2;
  if(n2->isTrue()) return n2; else if(n2->isFalse()) return n1;
  return boolExpr(OR, n1,n2);
}

ExpressionNode *ParserTree::not(ExpressionNode *n) {
  CHECKNODEPRETURNTYPE(n,EXPR_RETURN_BOOL);
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

ExpressionNode *ParserTree::powerExpr(SNode base) {
  return powerExpr(base,base._1());
}

// will always return a node of type NT_POWER
ExpressionNode *ParserTree::powerExpr(SNode base, SNode exponent) {
  ExpressionNode *f;
  if(exponent.isOne()) {
    if(base.getSymbol() == POW) {
      return base.node();
    } else {
      f = new ExpressionNodePower(base, exponent);
    }
  } else if(base.getSymbol() != POW) { // exponent != 1
    f = new ExpressionNodePower(base, exponent);
  } else if(base.right().isOne()) {    // base is POW, with right==1
    f = new ExpressionNodePower(base.left(), exponent);
  } else {                             // both exponents are != 1
    f = new ExpressionNodePower(base.left(), multiplyExponents(base.right().node(),exponent.node()));
  }
  TRACE_NEW(f);
  return f;
}

ExpressionNode *ParserTree::powerExpr(SNode base, const Rational &exponent) {
  return powerExpr(base, numberExpr(exponent));
}

ExpressionNode *ParserTree::addentExpr(SNode n, bool positive) {
  ExpressionNode *a;
  if(n.getNodeType() == NT_ADDENT) {
    return (n.isPositive() == positive) ? n.node() : addentExpr(n.left(),!positive);
  } else {
    a = new ExpressionNodeAddent(n, positive);
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

ExpressionNode *ParserTree::getStmtList(SNode oldStmtList, const SNodeArray &newChildArray) {
  const SNodeArray &oldChildArray = oldStmtList.getChildArray();
  SNode             result = newChildArray.isSameNodes(oldChildArray) ? oldStmtList : stmtList(newChildArray);
  return result.node();
}

ExpressionNode *ParserTree::getAssignStmt(SNode oldAssign, const SNodeArray &newChildArray) {
  const SNodeArray &oldChildArray = oldAssign.getChildArray();
  SNode             result = newChildArray.isSameNodes(oldChildArray) ? oldAssign: assignStmt(newChildArray);
  return result.node();
}

ExpressionNode *ParserTree::getTreeNode(SNode oldTree, const SNodeArray &newChildArray) {
  const SNodeArray &oldChildArray = oldTree.getChildArray();
  SNode             result = newChildArray.isSameNodes(oldChildArray) ? oldTree : treeExpr(oldTree.getSymbol(), newChildArray);
  return result.node();
}

ExpressionNode *ParserTree::getAnd(SNode oldAnd, SNode left, SNode right) {
  if(left.isTrue() ) return right.node(); else if(left.isFalse( )) return left.node();
  if(right.isTrue()) return left.node() ; else if(right.isFalse()) return right.node();
  return ((oldAnd.left().isSameNode(left ) && (oldAnd.right().isSameNode(right)))
      ||  (oldAnd.left().isSameNode(right) && (oldAnd.right().isSameNode(left))))
       ? oldAnd.node()
       : and(left.node(),right.node());
}

ExpressionNode *ParserTree::getOr(SNode oldOr, SNode left, SNode right) {
  if(left.isFalse() ) return right.node(); else if(left.isTrue( )) return left.node();
  if(right.isFalse()) return left.node() ; else if(right.isTrue()) return right.node();
  return ((oldOr.left().isSameNode(left ) && (oldOr.right().isSameNode(right)))
      ||  (oldOr.left().isSameNode(right) && (oldOr.right().isSameNode(left))))
       ? oldOr.node()
       : or(left.node(),right.node());
}

ExpressionNode *ParserTree::getNot(SNode oldNot, SNode left) {
  if(left.isFalse() ) return getTrue(); else if(left.isTrue( )) return getFalse();
  return (oldNot.left().isSameNode(left)) ? oldNot.node() : not(left.node());
}

ExpressionNode *ParserTree::getPoly(SNode oldPoly, const CoefArray &newCoefArray, SNode newArgument) {
  const CoefArray &oldCoefArray = oldPoly.getCoefArray();
  const SNode      oldArgument  = oldPoly.getArgument();
  SNode            result       = (newCoefArray.isSameNodes(oldCoefArray) && newArgument.isSameNode(oldArgument))
                                ? oldPoly
                                : polyExpr(newCoefArray, newArgument);
  return result.node();
}

ExpressionNode *ParserTree::getSum(SNode oldSum, const AddentArray &newAddentArray) {
  const AddentArray &oldAddentArray = oldSum.getAddentArray();
  SNode             result          = newAddentArray.isSameNodes(oldAddentArray)
                                    ? oldSum
                                    : sumExpr(newAddentArray);
  return result.node();
}

ExpressionNode *ParserTree::getProduct(SNode oldProduct, const FactorArray &newFactorArray) {
  const FactorArray  &oldFactorArray = oldProduct.getFactorArray();
  SNode               result         = (newFactorArray.isSameNodes(oldFactorArray))
                                     ? oldProduct
                                     : productExpr(newFactorArray);
  return result.node();
}

ExpressionNode *ParserTree::getPower(SNode oldPower, SNode newBase, SNode newExpo) {
  SNode oldBase = oldPower.left();
  SNode oldExpo = oldPower.right();
  return (newBase.isSameNode(oldBase) && newExpo.isSameNode(oldExpo)) ? oldPower.node() : powerExpr(newBase, newExpo);
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
