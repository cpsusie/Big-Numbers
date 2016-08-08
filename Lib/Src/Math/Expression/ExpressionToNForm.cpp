#include "pch.h"
#include <Math/Expression/Expression.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>

Expression &Expression::toNumericForm() {
  if(getRoot() == NULL) return *this;
  switch(getTreeForm()) {
  case TREEFORM_STANDARD :
    toCanonicalForm();
    setRoot(toNForm(getRoot()));
    setTreeForm(TREEFORM_NUMERIC);
    toStandardForm();
    break;

  case TREEFORM_CANONICAL:
    setRoot(toNForm(getRoot()));
    setTreeForm(TREEFORM_NUMERIC);
    toCanonicalForm();
    break;

  case TREEFORM_NUMERIC  :
    break;
  }
  return *this;
}

// ------------------------------ toNForm -------------------------------------------------------------------------

SNode Expression::toNForm(ExpressionNode *n) {
  DEFINEMETHODNAME;

  const SStmtList stmtList(n);
  SStmtList       newStmtList;

  const int stmtCount = (int)stmtList.size() - 1;
  for(int i = 0; i < stmtCount; i++) {
    const SNode &stmt = stmtList[i];
    newStmtList.add(assignStmt(stmt.left(), toNFormRealExp(stmt.right())));
  }
  SNode last = stmtList.last();
  switch(last.getSymbol()) {
  case RETURNREAL:
    newStmtList.add(unaryExp(RETURNREAL, toNFormRealExp(last.left())));
    break;
  case RETURNBOOL:
    newStmtList.add(unaryExp(RETURNBOOL, toNFormBoolExp(last.left())));
    break;
  default:
    throwUnknownSymbolException(method, last);
  }
  return newStmtList.removeUnusedAssignments();
}

SNode Expression::toNFormRealExp(ExpressionNode *n) {
  DEFINEMETHODNAME;
  if(n->isConstant()) {
    return numberExpression(evaluateRealExpr(n));
  }
  switch(n->getSymbol()) {
  case NUMBER    : throwMethodException(s_className, method, _T("Unexpected node-symbol (NUMBER)"));
  case NAME      : return n;
  case SUM       : return toNFormSum(n);
  case PRODUCT   : return toNFormProduct(n);
  case POLY      : return toNFormPoly(n);
  default        : return toNFormTreeNode(n);
  }
}

SNode Expression::toNFormBoolExp(ExpressionNode *n) {
  DEFINEMETHODNAME;
  if(n->isConstant()) {
    return booleanExpression(evaluateBoolExpr(n));
  }
  switch(n->getSymbol()) {
  case NOT   :
    return !toNFormBoolExp(n->left());
  case AND   :
    return toNFormBoolExp(n->left()) && toNFormBoolExp(n->right());
  case OR    :
    return toNFormBoolExp(n->left()) || toNFormBoolExp(n->right());

  case EQ    :
  case NE    :
  case LE    :
  case LT    :
  case GE    :
  case GT    :
    { SNode l = toNFormRealExp(n->left());
      SNode r = toNFormRealExp(n->right());
      return binExp(n->getSymbol(), l, r);
    }
  default    :
    throwUnknownSymbolException(method, n);
    return NULL;
  }
}

SNode Expression::toNFormSum(ExpressionNode *n) {
  const AddentArray &a = n->getAddentArray();
  if(a.size() == 0) {
    return _0();
  } else {
    Real constant = 0;
    AddentArray newArray;
    for(size_t i = 0; i < a.size(); i++) {
      SumElement *e = a[i];
      SNode tmp = toNFormRealExp(e->getNode());
      if(tmp.isNumber()) {
        if(e->isPositive()) constant += tmp.getReal(); else constant -= tmp.getReal();
      } else {
        newArray.add(tmp, e->isPositive());
      }
    }
    if(newArray.size() == 0) return numberExpression(constant);
    SNode result = newArray[0]->getNode();
    if(!newArray[0]->isPositive()) result = -result;
    for(size_t i = 1; i < newArray.size(); i++) {
      SumElement *e  = newArray[i];
      SNode       ne = e->getNode();
      if(e->isPositive()) result += ne; else result -= ne;
    }
    return (constant == 0) ? result
         : (constant >  0) ? result + numberExpression(constant)
                           : result - SNode(numberExpression(-constant));
  }
}

SNode Expression::toNFormProduct(ExpressionNode *n) {
  Real constant = 1;
  const FactorArray &a = n->getFactorArray();
  FactorArray newArray;
  for(size_t i = 0; i < a.size(); i++) {
    ExpressionFactor *f        = a[i];
    ExpressionNode   *base     = toNFormRealExp(f->base());
    ExpressionNode   *exponent = toNFormRealExp(f->exponent());
    if(base->isNumber() && exponent->isNumber()) {
      constant *= evaluateRealExpr(f);
    } else {
      newArray.add(base, exponent);
    }
  }
  if(newArray.size() == 0) return numberExpression(constant);

  FactorArray p = newArray.selectConstantPositiveExponentFactors();
  p.addAll(newArray.selectNonConstantExponentFactors());
  FactorArray q = newArray.selectConstantNegativeExponentFactors();

  SNode result;
  if(p.size() > 0) {
    result = newArray[0];
    for(size_t i = 1; i < p.size(); i++) {
      result *= p[i];
    }
  }
  if(result.isEmpty()) {
    result = numberExpression(constant);
  } else {
    if(constant != 1) result *= numberExpression(constant);
  }
  for(size_t i = 0; i < q.size(); i++) {
    result /= reciprocal(q[i]);
  }
  return result;
}

SNode Expression::toNFormPoly(ExpressionNode *n) {
  const ExpressionNodeArray &coefficientArray = n->getCoefficientArray();
  ExpressionNode            *argument         = n->getArgument();

  ExpressionNodeArray newCoefficientArray(coefficientArray.size());
  for(size_t i = 0; i < coefficientArray.size(); i++) {
    newCoefficientArray.add(toNFormRealExp(coefficientArray[i]));
  }
  return getPoly(n, newCoefficientArray, toNFormRealExp(argument));
}

SNode Expression::toNFormTreeNode(ExpressionNode *n) {
  if(n->isBooleanOperator()) {
    return toNFormBoolExp(n);
  }
  switch(n->getSymbol()) {
  case IIF:
    { const SNode cond   = toNFormBoolExp(n->child(0));
      const SNode eTrue  = toNFormRealExp(n->child(1));
      const SNode eFalse = toNFormRealExp(n->child(2));
      if(cond.isTrue()) return eTrue; else if(cond.isFalse()) return eFalse;
      return condExp(cond, eTrue, eFalse);
    }
  default:
    { const ExpressionNodeArray &a = n->getChildArray();
      ExpressionNodeArray        newChildArray(a.size());
      for(size_t i = 0; i < a.size(); i++) {
        newChildArray.add(toNFormRealExp(a[i]));
      }
      return getTree(n, newChildArray);
    }
  }
}
