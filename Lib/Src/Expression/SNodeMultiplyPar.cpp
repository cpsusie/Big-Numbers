#include "pch.h"
#include <Math/Expression/ParserTree.h>
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
  SNodeArray        newChildArray(a.getTree(), a.size());
  for(size_t i = 0; i < a.size(); i++) {
    newChildArray.add(a[i].multiplyParentheses());
  }
  SNode result = treeExp(getSymbol(), newChildArray);
  RETURNNODE( result );
}

SNode SNode::multiplyBoolExpr() const {
  ENTERMETHOD();
  CHECKNODETYPE(*this,NT_BOOLEXPR);
  const SNodeArray &a = getChildArray();
  SNodeArray        newChildArray(a.getTree(),a.size());
  for(size_t i = 0; i < a.size(); i++) {
    newChildArray.add(a[i].multiplyParentheses());
  }
  SNode result = boolExp(getSymbol(), newChildArray);
  RETURNNODE( result );
}

SNode SNode::multiplyParenthesesInPoly() const {
  ENTERMETHOD();
  CHECKNODETYPE(*this,NT_POLY);
  const SNodeArray &coefArray = getCoefArray();
  SNode             newArg    = getArgument().multiplyParentheses();

  SNodeArray newCoefArray(coefArray.getTree(),coefArray.size());
  for(size_t i = 0; i < coefArray.size(); i++) {
    newCoefArray.add(coefArray[i].multiplyParentheses());
  }
  SNode result = polyExp(newCoefArray, newArg);
  RETURNNODE( result );
}

SNode SNode::multiplyAssignStmt() const {
  ENTERMETHOD();
  CHECKNODETYPE(*this,NT_ASSIGN);
  SNode result = assignStmt(left(), right().multiplyParentheses());
  RETURNNODE( result );
}

SNode SNode::multiplyStmtList() const {
  ENTERMETHOD();
  CHECKNODETYPE(*this,NT_STMTLIST);
  const SNodeArray &a = getChildArray();
  SNodeArray        newChildArray(a.getTree(),a.size());
  for(size_t i = 0; i < a.size(); i++) {
    newChildArray.add(a[i].multiplyParentheses());
  }
  SNode result = stmtList(newChildArray);
  RETURNNODE( result );
}

SNode SNode::multiplyParenthesesInSum() const {
  ENTERMETHOD();
  CHECKNODETYPE(*this,NT_SUM);
  const AddentArray &a = getAddentArray();
  AddentArray        newAddentArray(a.getTree(), a.size());
  for(size_t i = 0; i < a.size(); i++) {
    const SNode &e = a[i];
    newAddentArray.add(addentExp(e.left().multiplyParentheses(), e.isPositive()));
  }
  SNode result = sumExp(newAddentArray);
  RETURNNODE( result );
}

SNode SNode::multiplyParenthesesInProduct() const {
  ENTERMETHOD();
  CHECKNODETYPE(*this,NT_PRODUCT);

  const FactorArray &a = getFactorArray();
  const size_t       n = a.size();
  FactorArray        newFactorArray(a.getTree(), n);
  for(size_t i = 0; i < n; i++) {
    SNode f = a[i];
    newFactorArray.add(factorExp(f.base().multiplyParentheses(), f.exponent().multiplyParentheses()));
  }

  BitSet done(newFactorArray.size() + 1);
  do {
    FactorArray tmp = newFactorArray;
    newFactorArray.clear();
    done.setCapacity(tmp.size() + 1);
    done.clear();
    for(size_t i1 = 1; i1 < tmp.size(); i1++) {
      if(done.contains(i1)) continue;
      SNode f1 = tmp[i1];
      if((f1.base().getSymbol() == SUM) && !f1.exponent().isOne()) {
        continue;
      }
      for(size_t i2 = 0; i2 < i1; i2++) {
        if(done.contains(i1)) break;
        if(done.contains(i2)) continue;
        SNode f2 = tmp[i2];
        if((f2.base().getSymbol() == SUM) && !f2.exponent().isOne()) {
          continue;
        }
        if(f1.base().getSymbol() == SUM) {
          newFactorArray.add(factorExp(multiplyFactorSum(f2, f1.base()),1));
          done.add(i1);
          done.add(i2);
        } else if(f2.base().getSymbol() == SUM) {
          newFactorArray.add(factorExp(multiplyFactorSum(f1, f2.base()),1));
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
  SNode result = productExp(newFactorArray);
  RETURNNODE( result );
}

SNode SNode::multiplyFactorSum(SNode factor, SNode sum) const {
  ENTERMETHOD2(factor,sum);
  CHECKNODETYPE(factor,NT_FACTOR);
  CHECKNODETYPE(sum   ,NT_SUM   );

  if((factor.base().getSymbol() == SUM) && factor.exponent().isOne()) {
    RETURNNODE(multiplySumSum(factor.base(),sum));
  } else {
    const AddentArray &sa = sum.getAddentArray();
    AddentArray        tmp(sa.getTree(),sa.size());
    for(size_t i = 0; i < sa.size(); i++) {
      SNode e = sa[i];
      tmp.add(addentExp(factor * e.left(),e.isPositive()));
    }
    RETURNNODE( sumExp(tmp) );
  }
}

};
