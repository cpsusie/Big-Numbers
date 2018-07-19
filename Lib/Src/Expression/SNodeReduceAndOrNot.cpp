#include "pch.h"
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/SNodeReduceDbgStack.h>

namespace Expr {

typedef enum {
  _F  ,_T           // false, true
 ,_L  ,_R           // left , right
 ,_EQ ,_NE          // ==   , !=
 ,_LT ,_LE          // <    , <=
 ,_GT ,_GE          // >    , >=
} ReducedCompare;

class CompareOpPair {
private:
  ExpressionInputSymbol m_op1, m_op2;
public:
  CompareOpPair() : m_op1(EOI), m_op2(EOI) {
  }
  CompareOpPair(ExpressionInputSymbol op1, ExpressionInputSymbol op2) : m_op1(op1), m_op2(op2) {
  }
  bool operator==(const CompareOpPair &copPair) const {
    return (m_op1 == copPair.m_op1) && (m_op2 == copPair.m_op2);
  }
  ULONG hashCode() const {
    return (UINT)m_op1 * 229 + m_op2;
  }
};

typedef ReducedCompare ReduceCompareMatrix[6][6];

class CompareOpPairMap : public CompactHashMap<CompareOpPair, ReducedCompare> {
private:
  void add(ExpressionInputSymbol op1, ExpressionInputSymbol op2, ReducedCompare rc) {
    put(CompareOpPair(op1,op2),rc);
  }
protected:
  CompareOpPairMap(const ReduceCompareMatrix m);
public:
  ReducedCompare lookup(ExpressionInputSymbol op1, ExpressionInputSymbol op2) const;
};

CompareOpPairMap::CompareOpPairMap(const ReduceCompareMatrix m) {
  const ExpressionInputSymbol ops[] = { EQ,NE,LT,LE,GT,GE};  // dont change order. Must match matrices below
  for(UINT r = 0; r < ARRAYSIZE(ops); r++) {
    const ExpressionInputSymbol opL = ops[r];
    for(UINT c = 0; c < ARRAYSIZE(ops); c++) {
      const ExpressionInputSymbol opR = ops[c];
      add(opL,opR,m[r][c]);
    }
  }
}

ReducedCompare CompareOpPairMap::lookup(ExpressionInputSymbol op1, ExpressionInputSymbol op2) const {
  const ReducedCompare *rc = get(CompareOpPair(op1,op2));
  if(rc == NULL) {
    throwInvalidArgumentException(__TFUNCTION__,_T("op1=%s, op2=%s")
                                 ,SNode::getSymbolName(op1).cstr()
                                 ,SNode::getSymbolName(op2).cstr());
  }
  return *rc;
}

class CompareOpPairAndMap : public CompareOpPairMap {
private:
  static const ReduceCompareMatrix s_m;
public:
  CompareOpPairAndMap::CompareOpPairAndMap() : CompareOpPairMap(s_m) {
  }
};

class CompareOpPairOrMap : public CompareOpPairMap {
private:
  static const ReduceCompareMatrix s_m;
public:
  CompareOpPairOrMap::CompareOpPairOrMap() : CompareOpPairMap(s_m) {
  }
};

/*
36 combinations of left and right oprand
AND        ==   !=   <    <=   >    >=

  ==       l    F    F    l    F    l
  !=       F    l    r    <    r    >
  <        F    l    l    l    F    F
  <=       r    <    r    l    F    ==
  >        F    l    F    F    l    l
  >=       r    >    F    ==   r    l
*/
const ReduceCompareMatrix CompareOpPairAndMap::s_m = {
//         ==   !=   <    <=   >    >=
/*==*/   { _L  ,_F  ,_F  ,_L  ,_F  ,_L  }
/*!=*/  ,{ _F  ,_L  ,_R  ,_LT ,_R  ,_GT }
/*< */  ,{ _F  ,_L  ,_L  ,_L  ,_F  ,_F  }
/*<=*/  ,{ _R  ,_LT ,_R  ,_L  ,_F  ,_EQ }
/*> */  ,{ _F  ,_L  ,_F  ,_F  ,_L  ,_L  }
/*>=*/  ,{ _R  ,_GT ,_F  ,_EQ ,_R  ,_L  }
};

/*
36 combinations of left and right oprand
OR         ==   !=   <    <=   >    >=

  ==       l    T    <=   r    >=   r
  !=       T    l    l    T    l    T
  <        <=   r    l    r    !=   T
  <=       l    T    l    l    T    T
  >        >=   r    !=   T    l    r
  >=       l    T    T    T    l    l
*/
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
                               ,c1.toString().cstr()      ,c2.toString().cstr()
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
                               ,c1.toString().cstr()      ,c2.toString().cstr()
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
      throwUnknownSymbolException(__TFUNCTION__);
      RETURNTHIS;
    }
  }
}

}; // namespace Expr
