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

  SNode l = b[0], r = b[1];

  static const CompareOpPairAndMap opPairAndMap;
  static const CompareOpPairOrMap  opPairOrMap;

  if(l.isCompareOperator() && r.isCompareOperator()) {
    SNode ll = l.left(), lr = l.right();
    SNode rl = r.left(), rr = r.right();
    ExpressionInputSymbol rightRelation = r.getSymbol();
    for(int i = 0; i < 2; i++) {
      if(ll.equal(rl) && lr.equal(rr)) {
        const CompareOpPairMap &pairMap = (getSymbol() == AND) ? (CompareOpPairMap&)opPairAndMap : (CompareOpPairMap&)opPairOrMap;
        switch(pairMap.lookup(l.getSymbol(),rightRelation)) {
        case _F : RETURNNODE(_false());
        case _T : RETURNNODE(_true());
        case _L : RETURNNODE( l );
        case _R : RETURNNODE( r );
        case _EQ: RETURNNODE( boolExp(EQ,ll,lr));
        case _NE: RETURNNODE( boolExp(NE,ll,lr));
        case _LT: RETURNNODE( boolExp(LT,ll,lr));
        case _LE: RETURNNODE( boolExp(LE,ll,lr));
        case _GT: RETURNNODE( boolExp(GT,ll,lr));
        case _GE: RETURNNODE( boolExp(GE,ll,lr));
        default : throwException(_T("opPairMap:unknown operatorPair <%s,%s>")
                                ,l.toString().cstr(), r.toString().cstr());
        }
      }
      // try to swap r.left<->r.right, and reverse relation
      rightRelation = ExpressionNode::reverseComparator(rightRelation);
      rl = r.right(); rr = r.left();
    }
  }
  switch(getSymbol()) {
  case AND: RETURNNODE( l && r );
  case OR : RETURNNODE( l || r );
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("not AND/OR-expression:&s"),toString().cstr());
  RETURNNODE(*this);
}

// assume symbol == NOT
SNode SNode::reduceNot() {
  SNode l = left();
  if(l.isCompareOperator()) {
    return boolExp(ExpressionNode::negateComparator(l.getSymbol()), l.left(), l.right());
  } else {
    switch(l.getSymbol()) {
    case AND: return !l.left() || !l.right(); // !(a && b) = !a || !b
    case OR : return !l.left() && !l.right(); // !(a || b) = !a && !b
    case NOT: return l.left();                // !(!b) == b
    default :
      throwUnknownSymbolException(__TFUNCTION__);
      return *this;
    }
  }
}

}; // namespace Expr