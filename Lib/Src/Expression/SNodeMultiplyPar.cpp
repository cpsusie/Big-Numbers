#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>
#include <Math/Expression/SNodeReduceDbgStack.h>

namespace Expr {

#define N( n) SNode(n)
#define NV(v) SNode(getTree(),v)

/* ------------------------------------------- multiplyParentheses ----------------------------------------- */

SNode SNode::multiplyParentheses() const {
  ENTERMETHOD();

  switch(getNodeType()) {
  case NT_NUMBER   :
  case NT_BOOLCONST:
  case NT_VARIABLE : RETURNNODE( *this );
  case NT_FACTOR   :
  case NT_TREE     : RETURNNODE( multiplyTreeNode() );
  case NT_BOOLEXPR : RETURNNODE( multiplyBoolExpr() );
  case NT_POLY     : RETURNNODE( multiplyParenthesesInPoly() );
  case NT_ASSIGN   : RETURNNODE( multiplyAssignStmt() );
  case NT_STMTLIST : RETURNNODE( multiplyStmtList() );
  case NT_SUM      : RETURNNODE( multiplyParenthesesInSum() );
  case NT_PRODUCT  : RETURNNODE( multiplyParenthesesInProduct() );
  default          : throwUnknownNodeTypeException(__TFUNCTION__);
  }
  RETURNNODE( *this );
}

SNode SNode::multiplyTreeNode() const {
  ENTERMETHOD();
  const SNodeArray &a = getChildArray();
  SNodeArray        newChildArray(a.size());
  for(size_t i = 0; i < a.size(); i++) {
    newChildArray.add(a[i].multiplyParentheses());
  }
  SNode result = treeExp(getSymbol(), newChildArray);
  RETURNNODE( result );
}

SNode SNode::multiplyBoolExpr() const {
  ENTERMETHOD();
  const SNodeArray &a = getChildArray();
  SNodeArray        newChildArray(a.size());
  for(size_t i = 0; i < a.size(); i++) {
    newChildArray.add(a[i].multiplyParentheses());
  }
  SNode result = boolExp(getSymbol(), newChildArray);
  RETURNNODE( result );
}

SNode SNode::multiplyParenthesesInPoly() const {
  ENTERMETHOD();
  const SNodeArray &coefArray = getCoefArray();
  SNode             newArg    = getArgument().multiplyParentheses();

  SNodeArray newCoefArray(coefArray.size());
  for(size_t i = 0; i < coefArray.size(); i++) {
    newCoefArray.add(coefArray[i].multiplyParentheses());
  }
  SNode result = polyExp(newCoefArray, newArg);
  RETURNNODE( result );
}


SNode SNode::multiplyAssignStmt() const {
  ENTERMETHOD();
  SNode result = assignStmt(left(), right().multiplyParentheses());
  RETURNNODE( result );
}

SNode SNode::multiplyStmtList() const {
  ENTERMETHOD();
  const SNodeArray &a = getChildArray();
  SNodeArray        newChildArray(a.size());
  for(size_t i = 0; i < a.size(); i++) {
    newChildArray.add(a[i].multiplyParentheses());
  }
  SNode result = stmtList(newChildArray);
  RETURNNODE( result );
}

SNode SNode::multiplyParenthesesInSum() const {
  ENTERMETHOD();
  const AddentArray &a = getAddentArray();
  AddentArray        newAddentArray(a.size());
  for(size_t i = 0; i < a.size(); i++) {
    SumElement *e = a[i];
    newAddentArray.add(N(e->getNode()).multiplyParentheses(), e->isPositive());
  }
  SNode result = NV(newAddentArray);
  RETURNNODE( result );
}

SNode SNode::multiplyParenthesesInProduct() const {
  ENTERMETHOD();

  const FactorArray &a = getFactorArray();
  const size_t       n = a.size();
  FactorArray        newFactorArray(n);
  for(size_t i = 0; i < n; i++) {
    ExpressionFactor *f = a[i];
    newFactorArray.add(f->base().multiplyParentheses(), f->exponent().multiplyParentheses());
  }

  BitSet done(newFactorArray.size() + 1);
  do {
    FactorArray tmp = newFactorArray;
    newFactorArray.clear();
    done.setCapacity(tmp.size() + 1);
    done.clear();
    for(size_t i1 = 1; i1 < tmp.size(); i1++) {
      if(done.contains(i1)) continue;
      ExpressionFactor *f1 = tmp[i1];
      if((f1->base().getSymbol() == SUM) && !f1->exponent().isOne()) {
        continue;
      }
      for(size_t i2 = 0; i2 < i1; i2++) {
        if(done.contains(i1)) break;
        if(done.contains(i2)) continue;
        ExpressionFactor *f2 = tmp[i2];
        if(f2->base().getSymbol() == SUM && !f2->exponent().isOne()) {
          continue;
        }
        if(f1->base().getSymbol() == SUM) {
          newFactorArray.add(multiplyFactorSum(f2, f1->base()));
          done.add(i1);
          done.add(i2);
        } else if(f2->base().getSymbol() == SUM) {
          newFactorArray.add(multiplyFactorSum(f1, f2->base()));
          done.add(i1);
          done.add(i2);
        }
      }
    }
    for(size_t i = 0; i < tmp.size(); i++) {
      if(!done.contains(i)) {
        newFactorArray.add(tmp[i]);
      }
    }
  } while(!done.isEmpty());
  SNode result = NV(newFactorArray);
  RETURNNODE( result );
}

SNode SNode::multiplyFactorSum(SNode factor, SNode sum) const {
  ENTERMETHOD2(factor,sum);

  if((factor.base().getSymbol() == SUM) && factor.exponent().isOne()) {
    RETURNNODE(multiplySumSum(factor.base(),sum));
  } else {
    const AddentArray &sa = sum.getAddentArray();
    AddentArray        tmp(sa.size());
    for(size_t i = 0; i < sa.size(); i++) {
      SumElement *e = sa[i];
      tmp.add(factor * N(e->getNode()),e->isPositive());
    }
    RETURNNODE( factorExp(NV(tmp),_1()) );
  }
}

};
