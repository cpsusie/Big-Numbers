#pragma once

#include <Math/Number.h>
#include <TinyBitSet.h>
#include <Math/Expression/Expression.h>
#include <Math/Expression/ExpressionSymbol.h>
#include "SNodeReduceDbgStack.h"

namespace Expr {

class ExpressionVariable;
class ExpressionNode;
class ExpressionFactor;
class ExpressionNodeSum;
class AddentArray;
class FactorArray;
class CoefArray;
class ParserTree;

// Define this, to have consistency check of ExpressionNodes when doing reduction
// and transformation to Canon-form/Std-form/Num-form
#define CHECK_CONSISTENCY

#if defined(CHECK_CONSISTENCY)
#define CHECKISCONSISTENT(n) (n).checkIsConsistent()
#else
#define CHECKISCONSISTENT(n)
#endif // CHECK_CONSISTENCY

typedef enum {
  NT_NUMBER
 ,NT_BOOLCONST
 ,NT_VARIABLE
 ,NT_TREE
 ,NT_BOOLEXPR
 ,NT_POLY
 ,NT_ASSIGN
 ,NT_STMTLIST
 ,NT_SUM
 ,NT_PRODUCT
 ,NT_ADDENT
 ,NT_POWER
} ExpressionNodeType;

class NodeTypeSet : public BitSet32 {
  DECLAREDEBUGSTRING;
private:
  void init(ExpressionNodeType t1, va_list argptr); // terminate list with -1
public:
  NodeTypeSet() {}
  NodeTypeSet(ExpressionNodeType t1,...); // terminate argumentlist with -1
  String toString() const;
};

class ExpressionSymbolSet : public BitSet {
  DECLAREDEBUGSTRING;
private:
  void init(ExpressionInputSymbol s1, va_list argptr); // terminate list with EOI
public:
  ExpressionSymbolSet();
  ExpressionSymbolSet(ExpressionInputSymbol s1,...);   // terminate list with EOI
  ExpressionSymbolSet(ExpressionInputSymbol s1, va_list argptr);
  String toString() const;
};

#if defined(_DEBUG)
#define DEFINEVALIDTYPES(    ...             ) static const NodeTypeSet _validNodeTypes(__VA_ARGS__)
#define CHECKNODEPTYPE(      n,expectedType  ) ExpressionNode::checkNodeType(  __TFUNCTION__,n,expectedType   )
#define CHECKNODEPTYPESET(   n               ) ExpressionNode::checkNodeType(  __TFUNCTION__,n,_validNodeTypes)
#define CHECKNODEPRETURNTYPE(n,expectedType  ) ExpressionNode::checkReturnType(__TFUNCTION__,n,expectedType   )
#define CHECKPSYMBOL(        n,expectedSymbol) ExpressionNode::checkSymbol(    __TFUNCTION__,n,expectedSymbol )
#define CHECKNODETYPE(       n,expectedType  ) CHECKNODEPTYPE(                      (n).node(),expectedType   )
#define CHECKNODETYPESET(    n               ) CHECKNODEPTYPESET(                   (n).node()                )
#define CHECKNODERETURNTYPE( n,expectedType  ) CHECKNODEPRETURNTYPE(                (n).node(),expectedType   )
#define CHECKSYMBOL(         n,expectedSymbol) CHECKPSYMBOL(                        (n).node(),expectedSymbol )
#else // _DEBUG
#define DEFINEVALIDTYPES(    ...             )
#define CHECKNODEPTYPE(      n,expectedType  )
#define CHECKNODEPTYPESET(   n               )
#define CHECKNODEPRETURNTYPE(n,expectedType  )
#define CHECKPSYMBOL(        n,expectedSymbol)
#define CHECKNODETYPE(       n,expectedType  )
#define CHECKNODETYPESET(    n               )
#define CHECKNODERETURNTYPE( n,expectedType  )
#define CHECKSYMBOL(         n,expectedSymbol)
#endif // _DEBUG

// Wrapper class til ExpressionNode
class SNode {
private:
  friend class SNodeArray;
  ExpressionNode *m_node;
  bool operator==(const SNode &n) const; // not implemented
  bool operator!=(const SNode &n) const; // not implemented

  SNode DPoly(    const String &name) const;
  SNode DStmtList(const String &name) const;

  void               setReduced() const;
  SNode              reduceStmtList();
  SNode              reduceAssign() const;
  SNode              reduceBoolExp();
  SNode              reduceNot();
  SNode              reduceAndOr();
  SNode              reduceConstCompareSameExprAnd(const Number &c1, const Number &c2, ExpressionInputSymbol rel1, ExpressionInputSymbol rel2);
  SNode              reduceConstCompareSameExprOr( const Number &c1, const Number &c2, ExpressionInputSymbol rel1, ExpressionInputSymbol rel2);
  SNode              reduceRealExp();
  SNode              reduceTreeNode();
  SNode              reduceIndexedExpr();
  SNode              reduceSum() const;
  SNode              reduceProduct();
  SNode              reduceModulus() const;
  SNode              reducePower();
  SNode              reduceConstantFactors(       FactorArray            &factors);
  SNode              reduceRationalPower(         const Rational         &base, const Rational &exponent);
  SNode              multiplyTreeNode() const;
  SNode              multiplyBoolExpr() const;
  SNode              multiplyParenthesesInPoly() const;
  SNode              multiplyAssignStmt() const;
  SNode              multiplyStmtList() const;
  SNode              multiplyParenthesesInSum() const;
  SNode              multiplyParenthesesInProduct() const;
  // try to reduce this to poly(cn,c[n-1],...c1,c0)(v) and return poly-node if possible. return empty if not
  SNode              reduceToPoly() const;
  // Assume factor.type=NT_POWER and sum.type=NT_SUM
  SNode              multiplyFactorSum(SNode factor, SNode sum) const;

  // Assume e1.type==e2.type==NT_ADDENT
  SNode              getCommonFactor(             SNode e1, SNode e2) const;
  // Assume this.symbol = SUM. nested SUM-nodes will all be put in result at the same level, by recursive calls
  // return true if result differs from childArray()
  bool               getAddents(                  AddentArray &result, bool positive=true) const;
  FactorArray       &getFactors(                  FactorArray &result);
  FactorArray       &getFactors(                  FactorArray &result, const SNode exponent);
  FactorArray       &getFactorsInPower(           FactorArray &result, const SNode exponent);
  FactorArray       &multiplyExponents(           FactorArray &result, FactorArray &factors, const SNode exponent);
  // Assume n1.type==n2.type==NT_SUM
  SNode              multiplySumSum(              SNode n1, SNode n2) const;
  SNode              changeFirstNegativeFactor() const;
  SNode              reduceLn();
  SNode              reduceLog10();
  SNode              reduceLog2();
  SNode              getPowerOfE();
  SNode              getPowerOf10();
  SNode              getPowerOf2();
  SNode              mergeLogarithms(SNode e1, SNode e2) const;
  SNode              reduceAsymmetricFunction();
  SNode              reduceSymmetricFunction();
  SNode              reducePoly();
  SNode              reduceCondExp();
  SNode              reduceTrigonometricFactors(  SNode f1, SNode f2);

  bool               canUseIdiotRule(             SNode             n1, const SNode n2) const;
  bool               canUseReverseIdiotRule(      SNode e1, SNode e2, SNode &result) const;
  bool               isSquareOfSinOrCos() const;
  bool               sameLogarithm(               const SNode n) const;
protected:
  SNode              multiplyExponents(           SNode e1, SNode e2) const;
  SNode              divideExponents(             SNode e1, SNode e2) const;

  friend class MarkedNodeMultiplier;
public:
  inline SNode() : m_node(NULL) {
  }
  inline SNode(ExpressionNode *node) : m_node(node) {
  }
#if defined(_DEBUG)
  inline SNode &operator=(const SNode &src) {
    m_node = src.m_node;
    return *this;
  }
#endif
  SNode(ParserTree &tree, int                v);
  SNode(ParserTree &tree, INT64              v);
  SNode(ParserTree &tree, const Rational    &v);
  SNode(ParserTree &tree, const Real        &v);
  SNode(ParserTree &tree, const Number      &v);
  SNode(ParserTree &tree, bool               v);

#define SNV(v) SNode(getTree(),v)

  inline SNode _0()     const { return SNV( 0);            } // zero
  inline SNode _1()     const { return SNV( 1);            } // 1
  inline SNode _m1()    const { return SNV(-1);            } // -1
  inline SNode _2()     const { return SNV( 2);            } // 2
  inline SNode _10()    const { return SNV(10);            } // 10
  inline SNode _05()    const { return SNV(Rational(1,2)); } // 1/2
  inline SNode _false() const { return SNV(false);         } // false
  inline SNode _true()  const { return SNV(true);          } // true
  inline bool isEmpty() const {
    return m_node == NULL;
  }
  inline ExpressionNode *node() const {
    return m_node;
  }
  ExpressionInputSymbol getSymbol()                            const;
  ParserTree           &getTree()                              const;
  String                getSymbolName()                        const;
  static String         getSymbolName(ExpressionInputSymbol symbol);
  String                getNodeTypeName()                      const;
  void                  mark();
  void                  unMark();
  bool                  isMarked()                             const;
  bool                  isReduced()                            const;
  SNode                 left()                                 const;
  SNode                 right()                                const;
  SNode                 getArgument()                          const;
  SNode                 child(UINT index)                      const;
  const SNodeArray     &getChildArray()                        const;
        SNodeArray     &getChildArray();
  const AddentArray    &getAddentArray()                       const;
        AddentArray    &getAddentArray();
  const FactorArray    &getFactorArray()                       const;
        FactorArray    &getFactorArray();
  const CoefArray      &getCoefArray()                         const;
        CoefArray      &getCoefArray();
  int                   getFirstCoefIndex()                    const;
  int                   getDegree()                            const;
  const String         &getName()                              const;
  ExpressionVariable   &getVariable()                          const;
  const Number         &getNumber()                            const;
  bool                  getBool()                              const;
  int                   getValueIndex()                        const;
  bool                  isConstant(Number *v = NULL)           const;
  bool                  isBooleanOperator()                    const;
  bool                  isCompareOperator()                    const; // is symbol in { EQ,NE,LE,LT,GE,GT }
  Real                 &doAssignment()                         const;
  Real                  evaluateReal()                         const;
  bool                  evaluateBool()                         const;
  ExpressionNodeType    getNodeType()                          const;
  ExpressionReturnType  getReturnType()                        const;
  UINT                  getNodeCount(const ExpressionSymbolSet &validSymbolSet) const;
  UINT                  getHeight()                            const;

  String                toString()                             const;

  bool                  isName()                               const;
  bool                  isNumber()                             const;
  bool                  isBoolean()                            const;
  bool                  isRational()                           const;
  bool                  isInteger()                            const;
  bool                  isNameOrNumber()                       const;
  bool                  isEven()                               const;
  bool                  isOdd()                                const;
  Real                  getReal()                              const;
  Rational              getRational()                          const;
  bool                  isUnaryMinus()                         const;
  bool                  isEulersConstant()                     const;
  bool                  isPi()                                 const;
  bool                  isZero()                               const;
  bool                  isOne()                                const;
  bool                  isTwo()                                const;
  bool                  isTen()                                const;
  bool                  isMinusOne()                           const;
  bool                  isMinusTwo()                           const;
  bool                  isNegativeNumber()                     const;
  bool                  isPositiveNumber()                     const;
  bool                  isTrue()                               const;
  bool                  isFalse()                              const;
  ExpressionInputSymbol getInverseFunction()                   const;

  bool                  isCoefArrayConstant()                  const;
  bool                  isPositive()                           const;
  bool                  dependsOn(const String &name)          const;

  bool                  containsFunctionCall()                 const;
  const Real           &getValueRef()                          const;
  bool                  isTrigonomtricFunction()               const;
  bool                  isSymmetricFunction()                  const;
  bool                  isAsymmetricFunction()                 const;
  // symbol in { EQ,NE,LE,LT,GE,GT } (EQ,NE are symmetric, the rest are changed. left<->right. ie. GE->LE, etc..
  static ExpressionInputSymbol reverseComparator(ExpressionInputSymbol symbol);

  bool                  reducesToRational(        Rational *r) const; // if r==NULL) only true/false is returned
  bool                  needParentheses(SNode parent)          const;
  SNode                 base()                                 const;
  SNode                 exponent()                             const;
  bool                  hasOddExponent()                       const;
#if defined(CHECK_CONSISTENCY)
  void                  checkIsConsistent()                    const;
#endif // CHECK_CONSISTENCY

  SNode operator+(  const SNode &n) const;
  // binary -
  SNode operator-(  const SNode &n) const;
  // unary  -
  SNode operator-() const;
  SNode operator*(  const SNode &n) const;
  SNode operator/(  const SNode &n) const;
  SNode operator%(  const SNode &n) const;
  SNode &operator+=(const SNode &n);
  SNode &operator-=(const SNode &n);
  SNode &operator*=(const SNode &n);
  SNode &operator/=(const SNode &n);
  SNode operator&&( const SNode &n) const;
  SNode operator||( const SNode &n) const;
  SNode operator!() const;
  bool equal(       const SNode &n) const;
  bool equalMinus(  const SNode &n) const;

  inline bool isSameNode(const SNode n) const {
    return m_node == n.m_node;
  }

  SNode multiplyParentheses() const;

    // Differentiation
  SNode D(const String &name) const;
  SNode reduce();
  static SNode beautify(SNode n);

  void throwInvalidSymbolForTreeMode(  const TCHAR *method) const;
  void throwUnknownSymbolException(    const TCHAR *method) const;
  void throwUnknownNodeTypeException(  const TCHAR *method) const;
  void throwUnknownReturnTypeException(const TCHAR *method) const;

  friend SNode reciprocal(const SNode &x);
  friend SNode abs(       const SNode &x);
  friend SNode sqrt(      const SNode &x);
  friend SNode sqr(       const SNode &x);
  friend SNode pow(       const SNode &x, const SNode &y);
  friend SNode root(      const SNode &x, const SNode &y);
  friend SNode exp(       const SNode &x);
  friend SNode exp10(     const SNode &x);
  friend SNode exp2(      const SNode &x);
  friend SNode ln(        const SNode &x);
  friend SNode log10(     const SNode &x);
  friend SNode log2(      const SNode &x);
  friend SNode sin(       const SNode &x);
  friend SNode cos(       const SNode &x);
  friend SNode tan(       const SNode &x);
  friend SNode cot(       const SNode &x);
  friend SNode asin(      const SNode &x);
  friend SNode acos(      const SNode &x);
  friend SNode atan(      const SNode &x);
  friend SNode atan2(     const SNode &y, const SNode &x);
  friend SNode acot(      const SNode &x);
  friend SNode csc(       const SNode &x);
  friend SNode sec(       const SNode &x);

  friend SNode sinh(      const SNode &x);
  friend SNode cosh(      const SNode &x);
  friend SNode tanh(      const SNode &x);
  friend SNode gauss(     const SNode &x);
};

inline std::wostream &operator<<(std::wostream &out, const Expr::SNode &n) {
  out << n.toString();
  return out;
}

class SNodeArray : public CompactArray<SNode> {
  DECLAREARRAYDEBUGSTRING;
private:
  ParserTree &m_tree;
  bool operator==(const SNodeArray &a) const; // not implemented
  bool operator!=(const SNodeArray &a) const; // not implemented
  SNodeArray &getListFromTree(ExpressionNode *n, ExpressionInputSymbol delimiterSymbol);
protected:
  // return an array of nodes from this, containing nodes with index
  // contained in set.
  // Assume set doesn't contain elements >= this.size()!!
  SNodeArray  filterNodes(const BitSet &set) const;
public:
  SNodeArray(ParserTree &tree) : m_tree(tree) {
  }
  SNodeArray(ParserTree &tree, size_t capacity) : m_tree(tree), CompactArray(capacity) {
  }
  inline ParserTree &getTree() const {
    return m_tree;
  }
  void convertFromParserTree(ExpressionNode *n, ExpressionInputSymbol delimiterSymbol);
  void clear(intptr_t capacity=0);
  void add(SNode n);
  void addAll(const SNodeArray &a);
  void remove(size_t i);
  void sort(int (*cmp)(const SNode &e1, const SNode &e2));
  bool isConstant() const;
  bool isSameNodes(const SNodeArray &a) const; // return true, if ExpressionNode pointers are the same
  bool equal(      const SNodeArray &a) const; // recursive compare all nodes ( deep compare)
  SNodeArray &cloneNodes(SNodeArray &dst, ParserTree *tree) const;
};

class StmtList : public SNodeArray {
public:
  StmtList(ParserTree &tree) : SNodeArray(tree) {
  }
  StmtList(ParserTree &tree, size_t capacity) : SNodeArray(tree, capacity) {
  }
  StmtList(const SNodeArray &a) : SNodeArray(a) {
  }
  SNodeArray &removeUnusedAssignments();
};

class AddentArray : public SNodeArray { // don't add any members, because of typecast
private:
  void addAll(const FactorArray &src); // not implemented. should not be called
public:
  AddentArray(ParserTree &tree) : SNodeArray(tree) {
  }
  AddentArray(ParserTree &tree, size_t capacity) : SNodeArray(tree, capacity) {
  }
  AddentArray &operator=(const AddentArray &rhs);
  // must be NT_ADDENT
  void add(SNode n);
  AddentArray &operator+=(const AddentArray &rhs);
  AddentArray &operator-=(const AddentArray &rhs);
  AddentArray &operator+=(const Rational    &r  );
  AddentArray &operator-=(const Rational    &r  );
  void sort();
  void sortStdForm();
  bool equalMinus( const AddentArray &a) const;
  AddentArray selectNodes(const BitSet &set) const {
    return (AddentArray&)(__super::filterNodes(set));
  }
};

class FactorArray : public SNodeArray { // don't add any members, because of typecast
private:
  // must be NT_POWER
  void add(SNode n);
  void addAll(const FactorArray &src);
public:
  FactorArray(ParserTree &tree) : SNodeArray(tree) {
  }
  FactorArray(ParserTree &tree, size_t capacity) : SNodeArray(tree, capacity) {
  }

  FactorArray selectConstantPositiveExponentFactors() const;
  FactorArray selectConstantNegativeExponentFactors() const;
  FactorArray selectNonConstantExponentFactors() const;
  int findFactorWithChangeableSign() const;

  FactorArray &operator*=(SNode              n);
  FactorArray &operator/=(SNode              n);
  FactorArray &operator*=(const FactorArray &a);
  FactorArray &operator/=(const FactorArray &a);
  // r==1=> skip, r.den==1=>*=num, r.num==1=>/=den, else *= numberExp(r)
  FactorArray &operator*=(const Rational    &r);
  FactorArray &operator/=(const Rational    &r);
  void sort();
  bool equal(      const FactorArray &a) const;
  bool equalMinus( const FactorArray &a) const;
  FactorArray selectFactors(const BitSet &set) const {
    return (FactorArray&)(__super::filterNodes(set));
  }
};

class CoefArray : public SNodeArray {
public:
  CoefArray(ParserTree &tree) : SNodeArray(tree) {
  }
  CoefArray(ParserTree &tree, size_t capacity) : SNodeArray(tree, capacity) {
  }
  bool equalMinus( const CoefArray &a) const;
};

SNode unaryExp(   ExpressionInputSymbol symbol, SNode n);
SNode binExp(     ExpressionInputSymbol symbol, SNode n1, SNode n2);
SNode condExp(    SNode condition , SNode nTrue  , SNode nFalse);

SNode polyExp(    CoefArray &coefArray, SNode arg);
SNode boolExp(    ExpressionInputSymbol symbol, SNode left, SNode right);
SNode boolExp(    ExpressionInputSymbol symbol, SNode child);
SNode boolExp(    ExpressionInputSymbol symbol, SNodeArray &a);
SNode treeExp(    ExpressionInputSymbol symbol, SNodeArray &a); // assume a.size() > 0
SNode productExp( FactorArray &a);                              // assume a.size() > 0
SNode sumExp(     AddentArray &a);                              // assume a.size() > 0
SNode assignStmt( SNode leftSide  , SNode expr);
SNode assignStmt( SNodeArray &list);
SNode stmtList(   SNodeArray &list);
SNode indexedExp( ExpressionInputSymbol symbol, SNode assignStmt, SNode endExp, SNode exp);
SNode indexedSum( SNode assignStmt, SNode endExp, SNode exp  );
SNode indexedProd(SNode assignStmt, SNode endExp, SNode exp  );
SNode addentExp(  SNode child     , bool positive);
SNode powerExp(   SNode base      , SNode expo);
SNode powerExp(   SNode base      , const Rational &expo);

}; // namespace Expr
