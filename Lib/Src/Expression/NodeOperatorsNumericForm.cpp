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

  // assume !n.isEmpty
  inline NNode(const SNode &n) : SNode(n) {
    CHECKISCONSISTENT(n);
  }
public:
  // assume n != NULL
  inline NNode(ExpressionNode *n) : SNode(n) {
    CHECKISCONSISTENT(*n);
  }
  SNode convert() const {
    ENTERMETHOD();
    RETURNNODE(toNForm());
  }
};

#define N(n)  NNode(n)

SNode ParserTree::toNumericForm(SNode n) {
  STARTREDUCTION();
  switch(getTreeForm()) {
  case TREEFORM_STANDARD :
    if(!n.isEmpty()) {
      n = toStandardForm(N(toCanonicalForm(n).node()).convert());
    }
    m_ops = NodeOperators::s_stdNumForm;
    break;

  case TREEFORM_CANONICAL:
    if(!n.isEmpty()) {
      n = toCanonicalForm((N(n.node()).convert()));
    }
    m_ops = NodeOperators::s_canonNumForm;
    break;

  case TREEFORM_NUMERIC  :
    break;
  }
  return n;
}

NNode NNode::toNForm() const {
  ENTERMETHOD();
  switch(getSymbol()) {
  case STMTLIST       : RETURNNODE( toNFormStmtList());
  case ASSIGN         : RETURNNODE( toNFormAssign()  );
  default             : RETURNNODE( toNFormTreeNode());
  }
}

NNode NNode::toNFormStmtList() const {
  ENTERMETHOD();
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
  RETURNNODE( stmtList(newStmtList.removeUnusedAssignments()) );
}

NNode NNode::toNFormAssign() const {
  ENTERMETHOD();
  RETURNNODE( assignStmt(left(), N(right()).toNFormRealExp()) );
}

NNode NNode::toNFormRealExp() const {
  ENTERMETHOD();
  Number v;
  if(isConstant(&v)) {
    RETURNNODE( SNV(v) );
  }
  switch(getSymbol()) {
  case NUMBER    : throwException(_T("%s:Unexpected node-symbol (NUMBER)"), __TFUNCTION__);
  case NAME      : RETURNTHIS;
  case SUM       : RETURNNODE( toNFormSum()      );
  case PRODUCT   : RETURNNODE( toNFormProduct()  );
  case POLY      : RETURNNODE( toNFormPoly()     );
  default        : RETURNNODE( toNFormTreeNode() );
  }
}

NNode NNode::toNFormBoolExp() const {
  ENTERMETHOD();
  if(isConstant()) {
    RETURNNODE( SNV(evaluateBool()) );
  }
  switch(getSymbol()) {
  case NOT  :
    RETURNNODE( !N(left()).toNFormBoolExp() );
  case AND   :
    RETURNNODE( N(left()).toNFormBoolExp() && N(right()).toNFormBoolExp() );
  case OR    :
    RETURNNODE( N(left()).toNFormBoolExp() || N(right()).toNFormBoolExp() );

  case EQ    :
  case NE    :
  case LE    :
  case LT    :
  case GE    :
  case GT    :
    { NNode l = N(left()).toNFormRealExp();
      NNode r = N(right()).toNFormRealExp();
      RETURNNODE( boolExp(getSymbol(), l, r) );
    }
  default    :
    throwUnknownSymbolException(__TFUNCTION__);
    return NULL;
  }
}

NNode NNode::toNFormSum() const {
  ENTERMETHOD();
  const AddentArray &a = getAddentArray();
  if(a.size() == 0) {
    RETURNNODE( _0() );
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
    if(newArray.size() == 0) {
      RETURNNODE( SNV(constant) );
    }
    SNode acc = newArray[0].left();
    if(!newArray[0].isPositive()) acc = -acc;
    for(size_t i = 1; i < newArray.size(); i++) {
      SNode e = newArray[i], ne = e.left();
      if(e.isPositive()) acc += ne; else acc -= ne;
    }
    NNode result = (constant == 0) ? acc
                 : (constant >  0) ? acc + SNV(constant )
                                   : acc - SNV(-constant);
    RETURNNODE( result );
  }
}

NNode NNode::toNFormProduct() const {
  ENTERMETHOD();
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
      newArray *=powerExp(base, exponent);
    }
  }
  if(newArray.size() == 0) {
    RETURNNODE( SNV(constant) );
  }

  FactorArray p = newArray.selectConstantPositiveExponentFactors();
  p *= newArray.selectNonConstantExponentFactors();
  FactorArray q = newArray.selectConstantNegativeExponentFactors();

  SNode result;
  if(p.size() > 0) {
    result = newArray[0];
    for(size_t i = 1; i < p.size(); i++) {
      result *= p[i];
    }
  }
  if(result.isEmpty()) {
    result = SNV(constant);
  } else {
    if(constant != 1) result *= SNV(constant);
  }
  for(size_t i = 0; i < q.size(); i++) {
    result /= reciprocal(q[i]);
  }
  RETURNNODE( result );
}

NNode NNode::toNFormPoly() const {
  ENTERMETHOD();
  const CoefArray &coefArray = getCoefArray();
  NNode            arg       = getArgument();

  CoefArray newCoefArray(coefArray.getTree(),coefArray.size());
  for(size_t i = 0; i < coefArray.size(); i++) {
    newCoefArray.add(N(coefArray[i]).toNFormRealExp());
  }
  RETURNNODE( polyExp(newCoefArray, arg.toNFormRealExp()) );
}

NNode NNode::toNFormTreeNode() const {
  ENTERMETHOD();
  if(isBooleanOperator()) {
    RETURNNODE( toNFormBoolExp() );
  }
  switch(getSymbol()) {
  case IIF:
    { const NNode cond   = N(child(0)).toNFormBoolExp();
      const NNode eTrue  = N(child(1)).toNFormRealExp();
      const NNode eFalse = N(child(2)).toNFormRealExp();
      if(eTrue.equal(eFalse)) {
        RETURNNODE( eTrue ); // dont care about condition
      }
      if(cond.isTrue()) {
        RETURNNODE( eTrue );
      } else if(cond.isFalse()) {
        RETURNNODE( eFalse );
      }
      RETURNNODE( condExp(cond, eTrue, eFalse) );
    }
  default:
    { const SNodeArray &a = getChildArray();
      SNodeArray        newChildArray(a.getTree(),a.size());
      for(size_t i = 0; i < a.size(); i++) {
        newChildArray.add(N(a[i]).toNFormRealExp());
      }
      RETURNNODE( treeExp(getSymbol(), newChildArray) );
    }
  }
}

}; // namespace Expr
