#pragma once

#include <Math/Number.h>
#include <Math/Expression/ExpressionSymbol.h>
#include "SNodeReduceDbgStack.h"

namespace Expr {

class ExpressionVariable;
class ExpressionNode;
class ExpressionFactor;
class ExpressionNodeSum;
class FactorArray;
class ParserTree;

// Define this, to have consistency check of ExpressionNodes when doing reduction
// and transformation to Canon-form/Std-form/Num-form
#define CHECK_CONSISTENCY

#ifdef CHECK_CONSISTENCY
#define CHECKISCONSISTENT(n) (n).checkIsConsistent()
#else
#define CHECKISCONSISTENT(n)
#endif // CHECK_CONSISTENCY

typedef enum {
  EXPR_NORETURNTYPE
 ,EXPR_RETURN_REAL
 ,EXPR_RETURN_BOOL
} ExpressionReturnType;

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
 ,NT_FACTOR
} ExpressionNodeType;

// Wrapper class til ExpressionNode
class SNode {
private:
  ExpressionNode *m_node;
  bool operator==(const SNode &n) const; // not implemented
  bool operator!=(const SNode &n) const; // not implemented

  SNode DPoly(    const String &name) const;
  SNode DStmtList(const String &name) const;

  SNode             &setReduced(); // return *this
  bool               isReduced() const;
  SNode              reduceBoolExp();
  SNode              reduceNot();
  SNode              reduceAndOr();
  SNode              reduceRealExp();
  SNode              reduceTreeNode();
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
  SNode              multiplyFactorSum(SNode factor, SNode sum) const; //  ExpressionFactor *a, ExpressionNodeSum sum) const;

  SNode              getCommonFactor(             SNode e1, SNode e2) const;
  FactorArray       &getFactors(                  FactorArray &result);
  FactorArray       &getFactors(                  FactorArray &result, const SNode exponent);
  FactorArray       &getFactorsInPower(           FactorArray &result, const SNode exponent);
  FactorArray       &multiplyExponents(           FactorArray &result, FactorArray &factors, const SNode exponent);
  SNode              multiplySumSum(              SNode n1, SNode n2) const; // assume n1 and n2 are NT_SUM
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
  SNode              reduceTrigonometricFactors(  ExpressionFactor &f1, ExpressionFactor &f2);

  bool               canUseIdiotRule(             SNode             n1, const SNode n2) const;
  bool               canUseReverseIdiotRule(      SNode e1, SNode e2, SNode &result) const;
  bool               isSquareOfSinOrCos() const;
  bool               sameLogarithm(               const SNode n) const;
protected:
  SNode              multiplyExponents(           SNode e1, SNode e2) const;
  SNode              divideExponents(             SNode e1, SNode e2) const;

  friend class MarkedNodeMultiplier;
public:
  friend class SNodeArray;
  inline SNode() : m_node(NULL) {
  }
  inline SNode(ExpressionNode *node) : m_node(node) {
  }
#ifdef _DEBUG
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
  SNode(ParserTree &tree, FactorArray       &a);

  SNode _0()     const; // zero
  SNode _1()     const; // 1
  SNode _m1()    const; // -1
  SNode _2()     const; // 2
  SNode _10()    const; // 10
  SNode _05()    const; // 1/2
  SNode _false() const; // false
  SNode _true()  const; // true
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
  SNode                 left()                                 const;
  SNode                 right()                                const;
  SNode                 getArgument()                          const;
  SNode                 child(UINT index)                      const;
  const SNodeArray     &getChildArray()                        const;
        SNodeArray     &getChildArray();
  const FactorArray    &getFactorArray()                       const;
        FactorArray    &getFactorArray();
  const SNodeArray     &getCoefArray()                         const;
        SNodeArray     &getCoefArray();
  int                   getFirstCoefIndex()                    const;
  int                   getDegree()                            const;
  const String         &getName()                              const;
  ExpressionVariable   &getVariable()                          const;
  const Number         &getNumber()                            const;
  int                   getValueIndex()                        const;
  bool                  isConstant()                           const;
  bool                  isBooleanOperator()                    const;
  bool                  isCompareOperator()                    const; // is symbol in { EQ,NE,LE,LT,GE,GT }
  Real                 &doAssignment()                         const;
  Real                  evaluateReal()                         const;
  bool                  evaluateBool()                         const;
  ExpressionNodeType    getNodeType()                          const;
  ExpressionReturnType  getReturnType()                        const;

  String                toString()                             const;

  bool                  isNumber()                             const;
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
  bool                  reducesToRationalConstant(Rational *r) const;
  bool                  reducesToRational(        Rational *r) const;
  bool                  needParentheses(SNode parent)          const;
  SNode                 base()                                 const;
  SNode                 exponent()                             const;
#ifdef CHECK_CONSISTENCY
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

  void throwInvalidSymbolForTreeMode(const TCHAR *method) const;
  void throwUnknownSymbolException(  const TCHAR *method) const;
  void throwUnknownNodeTypeException(const TCHAR *method) const;

  friend SNode reciprocal(const SNode &x);
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

class SNodeArray : public CompactArray<SNode> {
  DECLAREARRAYDEBUGSTRING;
private:
  ParserTree &m_tree;
  bool operator==(const SNodeArray &a) const; // not implemented
  bool operator!=(const SNodeArray &a) const; // not implemented
public:
  SNodeArray(ParserTree &tree) : m_tree(tree) {
  }
  explicit SNodeArray(ParserTree &tree, size_t capacity) : m_tree(tree), CompactArray(capacity) {
  }
  inline ParserTree &getTree() const {
    return m_tree;
  }
  void convertFromParserTree(ExpressionNode *n, ExpressionInputSymbol delimiterSymbol);
  void add(SNode n);
  void remove(size_t i);
  void addAll(const SNodeArray &a);
  bool isConstant() const;
  bool isSameNodes(const SNodeArray &a) const; // return true, if ExpressionNode pointers are the same
  bool equal(      const SNodeArray &a) const; // recursive compare all nodes ( deep compare)
  bool equalMinus( const SNodeArray &a) const; // recursive compare all nodes ( deep compare)
  SNodeArray &cloneNodes(SNodeArray &dst, ParserTree *tree) const;
};

class SStmtList : public SNodeArray {
public:
  SStmtList(ParserTree &tree) : SNodeArray(tree) {
  }
  SStmtList(const SNodeArray &a) : SNodeArray(a) {
  }
  SNodeArray &removeUnusedAssignments();
};

SNode unaryExp(  ExpressionInputSymbol symbol, SNode n);
SNode binExp(    ExpressionInputSymbol symbol, SNode n1, SNode n2);
SNode condExp(   SNode condition , SNode nTrue  , SNode nFalse);

SNode polyExp(   SNodeArray &coefArray, SNode arg);
SNode boolExp(   ExpressionInputSymbol symbol, SNode left, SNode right);
SNode boolExp(   ExpressionInputSymbol symbol, SNode child);
SNode boolExp(   ExpressionInputSymbol symbol, SNodeArray &a);
SNode treeExp(   ExpressionInputSymbol symbol, SNodeArray &a); // assume a.size() > 0
SNode productExp(FactorArray &a);                              // assume a.size() > 0
SNode sumExp(    SNodeArray  &a);                              // assume a.size() > 0
SNode assignStmt(SNode leftSide  , SNode expr);
SNode assignStmt(SNodeArray &list);
SNode stmtList(  SNodeArray &list);
SNode indexSum(  SNode assignStmt, SNode endExpr, SNode expr  );
SNode indexProd( SNode assignStmt, SNode endExpr, SNode expr  );
SNode addentExp( SNode child     , bool positive);
SNode factorExp( SNode base      , SNode expo);

}; // namespace Expr
