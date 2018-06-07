#include "pch.h"
#include <Math/Expression/ParserTree.h>

namespace Expr {

// ------------------------------ toNForm -------------------------------------------------------------------------

class NNode : public SNode {
private:
  NNode        toNForm()         const;
  NNode        toNFormRealExp()  const;
  NNode        toNFormBoolExp()  const;
  NNode        toNFormSum()      const;
  NNode        toNFormProduct()  const;
  NNode        toNFormPoly()     const;
  NNode        toNFormStmtList() const;
  NNode        toNFormAssign()   const;
  NNode        toNFormTreeNode() const;

  inline NNode(const SNode &n) : SNode(n) {
    CHECKISCONSISTENT(n);
  }
public:
  inline NNode(ExpressionNode *n) : SNode(n) {
    CHECKISCONSISTENT(*n);
  }
  ExpressionNode *convert() const {
    return toNForm().node();
  }
};

#define N(n)  NNode(n)
#define NV(v) SNode(getTree(),v)

NNode NNode::toNForm() const {
  DEFINEMETHODNAME;
  switch(getSymbol()) {
  case STMTLIST       : return toNFormStmtList();
  case ASSIGN         : return toNFormAssign();
  default             : return toNFormTreeNode();
  }
}

NNode NNode::toNFormStmtList() const {
  const StmtList &childArray = getChildArray();
  const int       childCount = (int)childArray.size() - 1;
  StmtList        newStmtList(getTree());

  for(int i = 0; i < childCount; i++) {
    const SNode &stmt = childArray[i];
    newStmtList.add(N(stmt).toNForm());
  }
  NNode last = childArray.last();
  switch(last.getReturnType()) {
  case EXPR_RETURN_REAL:
    newStmtList.add(last.toNFormRealExp());
    break;
  case EXPR_RETURN_BOOL:
    newStmtList.add(last.toNFormBoolExp());
    break;
  default:
    last.throwUnknownSymbolException(__TFUNCTION__);
  }
  return stmtList(newStmtList.removeUnusedAssignments());
}

NNode NNode::toNFormAssign() const {
  return assignStmt(left(), N(right()).toNFormRealExp());
}

NNode NNode::toNFormRealExp() const {
  DEFINEMETHODNAME;
  if(isConstant()) {
    return NV(evaluateReal());
  }
  switch(getSymbol()) {
  case NUMBER    : throwException(_T("%s:Unexpected node-symbol (NUMBER)"), method);
  case NAME      : return *this;
  case SUM       : return toNFormSum();
  case PRODUCT   : return toNFormProduct();
  case POLY      : return toNFormPoly();
  default        : return toNFormTreeNode();
  }
}

NNode NNode::toNFormBoolExp() const {
  DEFINEMETHODNAME;
  if(isConstant()) {
    return NV(evaluateBool());
  }
  switch(getSymbol()) {
  case NOT  :
    return !N(left()).toNFormBoolExp();
  case AND   :
    return N(left()).toNFormBoolExp() && N(right()).toNFormBoolExp();
  case OR    :
    return N(left()).toNFormBoolExp() || N(right()).toNFormBoolExp();

  case EQ    :
  case NE    :
  case LE    :
  case LT    :
  case GE    :
  case GT    :
    { NNode l = N(left()).toNFormRealExp();
      NNode r = N(right()).toNFormRealExp();
      return boolExp(getSymbol(), l, r);
    }
  default    :
    throwUnknownSymbolException(method);
    return NULL;
  }
}

NNode NNode::toNFormSum() const {
  const AddentArray &a = getAddentArray();
  if(a.size() == 0) {
    return _0();
  } else {
    Real constant = 0;
    AddentArray newArray(a.getTree());
    for(size_t i = 0; i < a.size(); i++) {
      const SNode e = a[i], tmp = N(e.left()).toNFormRealExp();
      if(tmp.isNumber()) {
        if(e.isPositive()) constant += tmp.getReal(); else constant -= tmp.getReal();
      } else {
        newArray.add(addentExp(tmp, e.isPositive()));
      }
    }
    if(newArray.size() == 0) return NV(constant);
    SNode result = newArray[0].left();
    if(!newArray[0].isPositive()) result = -result;
    for(size_t i = 1; i < newArray.size(); i++) {
      SNode e = newArray[i], ne = e.left();
      if(e.isPositive()) result += ne; else result -= ne;
    }
    return (constant == 0) ? result
         : (constant >  0) ? result + NV(constant)
                           : result - NV(-constant);
  }
}

NNode NNode::toNFormProduct() const {
  Real constant = 1;
  const FactorArray &a = getFactorArray();
  FactorArray newArray(a.getTree());
  for(size_t i = 0; i < a.size(); i++) {
    SNode f        = a[i];
    NNode base     = N(f.base()).toNFormRealExp();
    NNode exponent = N(f.exponent()).toNFormRealExp();
    if(base.isNumber() && exponent.isNumber()) {
      constant *= N(f).evaluateReal();
    } else {
      newArray.add(factorExp(base, exponent));
    }
  }
  if(newArray.size() == 0) return NV(constant);

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
    result = NV(constant);
  } else {
    if(constant != 1) result *= NV(constant);
  }
  for(size_t i = 0; i < q.size(); i++) {
    result /= reciprocal(q[i]);
  }
  return result;
}

NNode NNode::toNFormPoly() const {
  const SNodeArray &coefArray = getCoefArray();
  NNode             arg       = getArgument();

  SNodeArray newCoefArray(coefArray.getTree(),coefArray.size());
  for(size_t i = 0; i < coefArray.size(); i++) {
    newCoefArray.add(N(coefArray[i]).toNFormRealExp());
  }
  return polyExp(newCoefArray, arg.toNFormRealExp());
}

NNode NNode::toNFormTreeNode() const {
  if(isBooleanOperator()) {
    return toNFormBoolExp();
  }
  switch(getSymbol()) {
  case IIF:
    { const NNode cond   = N(child(0)).toNFormBoolExp();
      const NNode eTrue  = N(child(1)).toNFormRealExp();
      const NNode eFalse = N(child(2)).toNFormRealExp();
      if(eTrue.equal(eFalse)) return eTrue; // dont care about condition
      if(cond.isTrue()) return eTrue; else if(cond.isFalse()) return eFalse;
      return condExp(cond, eTrue, eFalse);
    }
  default:
    { const SNodeArray &a = getChildArray();
      SNodeArray        newChildArray(a.getTree(),a.size());
      for(size_t i = 0; i < a.size(); i++) {
        newChildArray.add(N(a[i]).toNFormRealExp());
      }
      return treeExp(getSymbol(), newChildArray);
    }
  }
}

ExpressionNode *ParserTree::toNumericForm(ExpressionNode *n) {
  if(n == NULL) return n;
  switch(getTreeForm()) {
  case TREEFORM_STANDARD :
    n = toStandardForm(N(toCanonicalForm(n)).convert());
    m_ops = NodeOperators::s_stdNumForm;
    break;

  case TREEFORM_CANONICAL:
    n = toCanonicalForm((N(n).convert()));
    m_ops = NodeOperators::s_canonNumForm;
    break;

  case TREEFORM_NUMERIC  :
    return n;
  }
  return n;
}

}; // namespace Expr
