#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionNode.h>
#include <Math/Expression/SNodeReduceDbgStack.h>

using namespace std;

namespace Expr {

typedef enum {
  _F  ,_T           // false, true
 ,_L  ,_R           // left , right
 ,_EQ ,_NE          // ==   , !=
 ,_LT ,_LE          // <    , <=
 ,_GT ,_GE          // >    , >=
} ReducedCompare;

typedef ReducedCompare ReduceCompareMatrix[6][6];

class CompareOpPairMap {
private:
  static int getIndex(ExpressionInputSymbol op);
protected:
  virtual const ReduceCompareMatrix &getReduceMatrix() const = 0;
public:
  inline ReducedCompare lookup(ExpressionInputSymbol op1, ExpressionInputSymbol op2) const {
    return getReduceMatrix()[getIndex(op1)][getIndex(op2)];
  }
};

int CompareOpPairMap::getIndex(ExpressionInputSymbol op) { // static
  switch(op) {
  case EQ: return 0;
  case NE: return 1;
  case LT: return 2;
  case LE: return 3;
  case GT: return 4;
  case GE: return 5;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("op=%s"),SNode::getSymbolName(op).cstr());
    return 0;
  }
}

class CompareOpPairAndMap : public CompareOpPairMap {
private:
  static const ReduceCompareMatrix s_m;
protected:
  const ReduceCompareMatrix &getReduceMatrix() const override {
    return s_m;
  }
};

class CompareOpPairOrMap : public CompareOpPairMap {
private:
  static const ReduceCompareMatrix s_m;
protected:
  const ReduceCompareMatrix &getReduceMatrix() const override {
    return s_m;
  }
};

const ReduceCompareMatrix CompareOpPairAndMap::s_m = {
//         ==   !=   <    <=   >    >=
/*==*/   { _L  ,_F  ,_F  ,_L  ,_F  ,_L  }
/*!=*/  ,{ _F  ,_L  ,_R  ,_LT ,_R  ,_GT }
/*< */  ,{ _F  ,_L  ,_L  ,_L  ,_F  ,_F  }
/*<=*/  ,{ _R  ,_LT ,_R  ,_L  ,_F  ,_EQ }
/*> */  ,{ _F  ,_L  ,_F  ,_F  ,_L  ,_L  }
/*>=*/  ,{ _R  ,_GT ,_F  ,_EQ ,_R  ,_L  }
};

const ReduceCompareMatrix CompareOpPairOrMap::s_m = {
//         ==   !=   <    <=   >    >=
/*==*/   { _L  ,_T  ,_LE ,_R  ,_GE ,_R  }
/*!=*/  ,{ _T  ,_L  ,_L  ,_T  ,_L  ,_T  }
/*< */  ,{ _LE ,_R  ,_L  ,_R  ,_NE ,_T  }
/*<=*/  ,{ _L  ,_T  ,_L  ,_L  ,_T  ,_T  }
/*> */  ,{ _GE ,_R  ,_NE ,_T  ,_L  ,_R  }
/*>=*/  ,{ _L  ,_T  ,_T  ,_T  ,_L  ,_L  }
};

#define SFALSE       { RETURNNODE(SNV(false));                }
#define STRUE        { RETURNNODE(SNV(true ));                }
#define SCOMP(rel,c) { RETURNNODE(boolExp(rel,*this,SNV(c))); }
#define SNULL        { RETURNNULL;                            }

// rel1 and rel2 are both compare operators (=,!=,<,<=,>,>=)
// and orignal boolean expression was (*this rel1 c1) AND (*this rel2 c2)
// If reduction is not possible an empty SNode is returned
SNode SNode::reduceConstCompareSameExprAnd(const Number &c1, const Number &c2, ExpressionInputSymbol rel1, ExpressionInputSymbol rel2) {
  ENTERMETHOD();
  switch(rel1) {
  case EQ:
    switch(rel2) {
    case EQ: if(c1!=c2) SFALSE       else            SCOMP(EQ,c1);
    case NE: if(c1==c2) SFALSE       else            SCOMP(EQ,c1);
    case LT: if(c1>=c2) SFALSE       else            SCOMP(EQ,c1);
    case LE: if(c1> c2) SFALSE       else            SCOMP(EQ,c1);
    case GT: if(c1<=c2) SFALSE       else            SCOMP(EQ,c1);
    case GE: if(c1< c2) SFALSE       else            SCOMP(EQ,c1);
    }
  case NE:
    switch(rel2) {
    case EQ: if(c1==c2) SFALSE       else            SCOMP(EQ,c2);
    case NE: if(c1==c2) SCOMP(NE,c1)                                else SNULL;
    case LT: if(c1>=c2) SCOMP(LT,c2)                                else SNULL;
    case LE: if(c1> c2) SCOMP(LE,c2)                                else SNULL;
    case GT: if(c1<=c2) SCOMP(GT,c2)                                else SNULL;
    case GE: if(c1< c2) SCOMP(GE,c2)                                else SNULL;
    }
  case LT:
    switch(rel2) {
    case EQ: if(c1<=c2) SFALSE       else            SCOMP(EQ,c2);
    case NE: if(c1<=c2) SCOMP(LT,c1)                                else SNULL;
    case LT: if(c1<=c2) SCOMP(LT,c1) else            SCOMP(LT,c2);
    case LE: if(c1<=c2) SCOMP(LT,c1) else            SCOMP(LE,c2);
    case GT:
    case GE: if(c1<=c2) SFALSE                                      else SNULL;
    }
  case LE:
    switch(rel2) {
    case EQ: if(c1< c2) SFALSE       else            SCOMP(EQ,c2);
    case NE: if(c1< c2) SCOMP(LE,c1)                                else SNULL;
    case LT: if(c1< c2) SCOMP(LE,c1) else            SCOMP(LT,c2);
    case LE: if(c1<=c2) SCOMP(LE,c1) else            SCOMP(LE,c2);
    case GT: if(c1<=c2) SFALSE                                      else SNULL;
    case GE: if(c1< c2) SFALSE       else if(c1==c2) SCOMP(EQ,c1)   else SNULL;
    }
  case GT:
    switch(rel2) {
    case EQ: if(c1>=c2) SFALSE       else            SCOMP(EQ,c2);
    case NE: if(c1>=c2) SCOMP(GT,c1)                                else SNULL;
    case LT:
    case LE: if(c1>=c2) SFALSE                                      else SNULL;
    case GT: if(c1>=c2) SCOMP(GT,c1) else            SCOMP(GT,c2);
    case GE: if(c1>=c2) SCOMP(GT,c1) else            SCOMP(GE,c2);
    }
  case GE:
    switch(rel2) {
    case EQ: if(c1> c2) SFALSE       else            SCOMP(EQ,c2);
    case NE: if(c1> c2) SCOMP(GE,c1)                                else SNULL;
    case LT: if(c1>=c2) SFALSE                                      else SNULL;
    case LE: if(c1> c2) SFALSE       else if(c1==c2) SCOMP(EQ,c1)   else SNULL;
    case GT: if(c1> c2) SCOMP(GE,c1) else            SCOMP(GT,c2);
    case GE: if(c1>=c2) SCOMP(GE,c1) else            SCOMP(GE,c2);
    }
  }
  throwInvalidArgumentException(__TFUNCTION__
                               ,_T("c1=%s,c2=%s,rel1=%s,rel2=%s")
                               ,::toString(c1).cstr()     ,::toString(c2).cstr()
                               ,getSymbolName(rel1).cstr(),getSymbolName(rel2).cstr()
                               );
  SNULL;
}

// rel1 and rel2 are both compare operators (=,!=,<,<=,>,>=)
// and orignal boolean expression was (*this rel1 c1) OR (*this rel2 c2)
// If reduction is not possible an empty SNode is returned
SNode SNode::reduceConstCompareSameExprOr(const Number &c1, const Number &c2, ExpressionInputSymbol rel1, ExpressionInputSymbol rel2) {
  ENTERMETHOD();
  switch(rel1) {
  case EQ:
    switch(rel2) {
    case EQ: if(c1==c2) SCOMP(EQ,c1)                               else SNULL;
    case NE: if(c1==c2) STRUE        else            SCOMP(NE,c2);
    case LT: if(c1< c2) SCOMP(LT,c2) else if(c1==c2) SCOMP(LE,c2)  else SNULL;
    case LE: if(c1<=c2) SCOMP(LE,c2)                               else SNULL;
    case GT: if(c1> c2) SCOMP(GT,c2) else if(c1==c2) SCOMP(GE,c2)  else SNULL;
    case GE: if(c1>=c2) SCOMP(GE,c2)                               else SNULL;
    }
  case NE:
    switch(rel2) {
    case EQ: if(c1==c2) STRUE        else            SCOMP(NE,c1);
    case NE: if(c1!=c2) STRUE        else            SCOMP(NE,c1);
    case LT: if(c1< c2) STRUE        else            SCOMP(NE,c1);
    case LE: if(c1<=c2) STRUE        else            SCOMP(NE,c1);
    case GT: if(c1> c2) STRUE        else            SCOMP(NE,c1);
    case GE: if(c1>=c2) STRUE        else            SCOMP(NE,c1);
    }
  case LT:
    switch(rel2) {
    case EQ: if(c1> c2) SCOMP(LT,c1) else if(c1==c2) SCOMP(LE,c1)  else SNULL;
    case NE: if(c1> c2) STRUE        else            SCOMP(NE,c2);
    case LT: if(c1> c2) SCOMP(LT,c1) else            SCOMP(LT,c2);
    case LE: if(c1> c2) SCOMP(LT,c1) else            SCOMP(LE,c2);
    case GT: if(c1> c2) STRUE        else if(c1==c2) SCOMP(NE,c2)  else SNULL;
    case GE: if(c1>=c2) STRUE                                      else SNULL;
    }
  case LE:
    switch(rel2) {
    case EQ: if(c1>=c2) SCOMP(LE,c1)                               else SNULL;
    case NE: if(c1>=c2) STRUE        else            SCOMP(NE,c2);
    case LT: if(c1>=c2) SCOMP(LE,c1) else            SCOMP(LT,c2);
    case LE: if(c1>=c2) SCOMP(LE,c1) else            SCOMP(LE,c2);
    case GT:
    case GE: if(c1>=c2) STRUE                                      else SNULL;
    }
  case GT:
    switch(rel2) {
    case EQ: if(c1< c2) SCOMP(GT,c1) else if(c1==c2) SCOMP(GE,c1)  else SNULL;
    case NE: if(c1< c2) STRUE        else            SCOMP(NE,c2);
    case LT: if(c1< c2) STRUE        else if(c1==c2) SCOMP(NE,c2)  else SNULL;
    case LE: if(c1<=c2) STRUE                                      else SNULL;
    case GT: if(c1< c2) SCOMP(GT,c1) else            SCOMP(GT,c2);
    case GE: if(c1< c2) SCOMP(GT,c1) else            SCOMP(GE,c2);
    }
  case GE:
    switch(rel2) {
    case EQ: if(c1<=c2) SCOMP(GE,c1)                               else SNULL;
    case NE: if(c1<=c2) STRUE        else            SCOMP(NE,c2);
    case LT:
    case LE: if(c1<=c2) STRUE                                      else SNULL;
    case GT: if(c1<=c2) SCOMP(GE,c1) else            SCOMP(GT,c2);
    case GE: if(c1<=c2) SCOMP(GE,c1) else            SCOMP(GE,c2);
    }
  }
  throwInvalidArgumentException(__TFUNCTION__
                               ,_T("c1=%s,c2=%s,rel1=%s,rel2=%s")
                               ,::toString(c1).cstr()     ,::toString(c2).cstr()
                               ,getSymbolName(rel1).cstr(),getSymbolName(rel2).cstr()
                               );
  SNULL;
}

// assume symbol == AND/OR
SNode SNode::reduceAndOr() {
  ENTERMETHOD();
  SNode b[2] = { left(), right() };
  for(int i = 0; i < 2; i++) {
    switch(b[i].getSymbol()) {
    case AND:
    case OR:
    case NOT:
      b[i] = b[i].reduceBoolExp();
      break;
    }
  }

  SNode Rl = b[0], Rr = b[1];

  static const CompareOpPairAndMap opPairAndMap;
  static const CompareOpPairOrMap  opPairOrMap;

  if(Rl.isCompareOperator() && Rr.isCompareOperator()) {
    SNode ll = Rl.left(), lr = Rl.right();
    SNode rl = Rr.left(), rr = Rr.right();
    ExpressionInputSymbol rightRelation = Rr.getSymbol();
    for(int i = 0; i < 2; i++) {
      if(ll.equal(rl) && lr.equal(rr)) {
        const CompareOpPairMap &pairMap = (getSymbol() == AND) ? (CompareOpPairMap&)opPairAndMap : (CompareOpPairMap&)opPairOrMap;
        switch(pairMap.lookup(Rl.getSymbol(),rightRelation)) {
        case _F : RETURNNODE(_false());
        case _T : RETURNNODE(_true());
        case _L : RETURNNODE( Rl );
        case _R : RETURNNODE( Rr );
        case _EQ: RETURNNODE( boolExp(EQ,ll,lr));
        case _NE: RETURNNODE( boolExp(NE,ll,lr));
        case _LT: RETURNNODE( boolExp(LT,ll,lr));
        case _LE: RETURNNODE( boolExp(LE,ll,lr));
        case _GT: RETURNNODE( boolExp(GT,ll,lr));
        case _GE: RETURNNODE( boolExp(GE,ll,lr));
        default : throwException(_T("opPairMap:unknown operatorPair <%s,%s>")
                                ,Rl.toString().cstr(), Rr.toString().cstr());
        }
      }
      // try to swap r.left<->r.right, and reverse relation
      rightRelation = reverseComparator(rightRelation); swap(rl,rr);
    }

    // Check, if orignal boolean expression is (expr rel1 const1) [AND/OR] (expr rel2 const2)
    // where rel1,rel2 isCompareOperator()
    ExpressionInputSymbol leftRelation = Rl.getSymbol();
    for(int i = 0; i < 2; i++) {
      Number c1, c2;
      if(lr.isConstant(&c1)) {
        for(int j = 0; j < 2; j++) {
          if(rr.isConstant(&c2) && ll.equal(rl)) {
            SNode result;
            switch(getSymbol()) {
            case AND: result = ll.reduceConstCompareSameExprAnd(c1,c2,leftRelation,rightRelation); break;
            case OR : result = ll.reduceConstCompareSameExprOr( c1,c2,leftRelation,rightRelation); break;
            }
            if(!result.isEmpty()) {
              RETURNNODE(result);
            } else {
              i = j = 2; // to get out of the loop. We're done
            }
          }
          rightRelation = reverseComparator(rightRelation); swap(rl,rr);
        }
      }
      leftRelation = reverseComparator(leftRelation); swap(ll,lr);
    }
  }
  SNode result;
  switch(getSymbol()) {
  case AND: result = getTree().getAnd(*this,Rl,Rr); break;
  case OR : result = getTree().getOr( *this,Rl,Rr); break;
  default : throwInvalidArgumentException(__TFUNCTION__, _T("not AND/OR-expression:%s"),toString().cstr());
  }
  if(result.isSameNode(*this)) {
    setReduced();
    RETURNTHIS;
  }
  RETURNNODE(result);
}

// assume symbol == NOT
SNode SNode::reduceNot() {
  ENTERMETHOD();
  SNode l = left();
  if(l.isCompareOperator()) {
    RETURNNODE(boolExp(ExpressionNode::negateComparator(l.getSymbol()), l.left(), l.right()));
  } else {
    switch(l.getSymbol()) {
    case AND: RETURNNODE(!l.left() || !l.right()); // !(a && b) = !a || !b
    case OR : RETURNNODE(!l.left() && !l.right()); // !(a || b) = !a && !b
    case NOT: RETURNNODE(l.left());                // !(!b) == b
    default :
      UNKNOWNSYMBOL();
      RETURNTHIS;
    }
  }
}

}; // namespace Expr
